/*=============================================================================
 *
 *  Copyright (c) 2020 Sunnybrook Research Institute
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 *=============================================================================*/

#include "StainVectorMacenko.h"

//#include <chrono>
//#include <random>
//#include <fstream>
#include <sstream>

#include "ODConversion.h"
#include "StainVectorMath.h"
#include "MacenkoHistogram.h"
#include "BasisTransform.h"

namespace sedeen {
namespace image {

StainVectorMacenko::StainVectorMacenko(std::shared_ptr<tile::Factory> source,
    double ODthreshold /* = 0.15 */, double percentileThreshold /* = 1.0 */,
    int numHistoBins /* = 1024 */)
    : StainVectorOpenCV(source),
    m_sampleSize(0), //Must set to greater than 0 to ComputeStainVectors
    m_avgODThreshold(ODthreshold), //assign default value
    m_percentileThreshold(percentileThreshold), //assign default value
    m_numHistogramBins(numHistoBins) //assign default value
{}//end constructor

StainVectorMacenko::~StainVectorMacenko(void) {
}//end destructor

void StainVectorMacenko::ComputeStainVectors(double (&outputVectors)[9]) {
    if (this->GetSourceFactory() == nullptr) { return; }
    //Using this overload of the method requires setting sample size in advance
    long int sampleSize = this->GetSampleSize();
    if (sampleSize <= 0) { return; }
    double ODthreshold = this->GetODThreshold();
    double percentileThreshold = this->GetPercentileThreshold();
    if (percentileThreshold <= 0.0) { return; }

    //Sample a set of pixel values from the source
    cv::Mat samplePixels;
    auto theSampler = this->GetRandomWSISampler();
    if (theSampler == nullptr) { return; }
    
    bool samplingSuccess = theSampler->ChooseRandomPixels(samplePixels, sampleSize, ODthreshold);
    if (!samplingSuccess) { return; }

    //Create a class to perform the basis transformation of the sample pixels.
    std::unique_ptr<BasisTransform> theBasisTransform = std::make_unique<BasisTransform>(samplePixels, true); //optimizeDirections=true
    //The basis vectors are computed in the constructor and stored as members, so project points using them
    cv::Mat projectedPoints;
    bool projectSuccess = theBasisTransform->projectPoints(samplePixels, projectedPoints, false); //useMean=false
    if (!projectSuccess) { return; }

    //Create a class to histogram the results and find 2D vectors corresponding to percentile thresholds
    std::unique_ptr<MacenkoHistogram> theHistogram
        = std::make_unique<MacenkoHistogram>(this->GetPercentileThreshold(), this->GetNumHistogramBins());
    cv::Mat percentileThreshVectors;
    bool histoSuccess = theHistogram->PercentileThresholdVectors(projectedPoints, percentileThreshVectors);
    if (!histoSuccess) { return; }

    //Back-project to get un-normalized stain vectors. DO NOT translate to the mean after backprojection.
    cv::Mat backProjectedVectors;
    bool backProjectSuccess = theBasisTransform->backProjectPoints(percentileThreshVectors, backProjectedVectors, false); //useMean=false
    if (!backProjectSuccess) { return; }

    //Convert to C array and normalize rows
    double tempStainVecOutput[9] = {0.0};
    StainCVMatToCArray(backProjectedVectors, tempStainVecOutput, true);
    std::copy(std::begin(tempStainVecOutput), std::end(tempStainVecOutput), std::begin(outputVectors));
}//end single-parameter ComputeStainVectors

//This overload does not have a default value for sampleSize, so it requires at two arguments
void StainVectorMacenko::ComputeStainVectors(double (&outputVectors)[9], const long int sampleSize) {
    if (this->GetSourceFactory() == nullptr) { return; }
    //Set member variables with the argument values
    this->SetSampleSize(sampleSize);
    //Call the single-parameter version of this method, which uses member variables
    this->ComputeStainVectors(outputVectors);
}//end multi-parameter ComputeStainVectors

} // namespace image
} // namespace sedeen
