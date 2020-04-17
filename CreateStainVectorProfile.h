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

#ifndef SEDEEN_SRC_PLUGINS_CREATESTAINVECTORPROFILE_CREATESTAINVECTORPROFILE_H
#define SEDEEN_SRC_PLUGINS_CREATESTAINVECTORPROFILE_CREATESTAINVECTORPROFILE_H

// Sedeen headers
#include "algorithm/AlgorithmBase.h"
#include "algorithm/Parameters.h"
#include "algorithm/Results.h"

#include <omp.h>
#include <Windows.h>
//#include <fstream>
#include <memory>
#include <vector>

// Plugin headers
#include "ColorDeconvolutionKernel.h"
#include "StainProfile.h"

namespace sedeen {
namespace tile {

} // namespace tile

namespace algorithm {
//#define round(x) ( x >= 0.0f ? floor(x + 0.5f) : ceil(x - 0.5f) )

///Create Stain Vector Profile
///This plugin creates a way for the user to define a combination
///of stain vectors to be used in the Stain Analysis plugin
class CreateStainVectorProfile : public algorithm::AlgorithmBase {
public:
    CreateStainVectorProfile();
    virtual ~CreateStainVectorProfile();

private:
    // virtual functions
    virtual void run();
    virtual void init(const image::ImageHandle& image);

    //Set the visibility of GUI elements at the end of init
    //This doesn't work if called from init. It crashes Sedeen.
    //void initialVisibility();
    
	/// Creates the Color Deconvolution pipeline with a cache
	//
	/// \return 
	/// TRUE if successful, false on error or failure. Error message is placed in pointer to string.
	bool buildPipeline(std::shared_ptr<StainProfile>, std::shared_ptr<std::string>);
    /// Test whether the values or states of the UI parameters have changed
    bool checkParametersChanged(bool);
    ///build the pipeline for getting the stain vectors from pixel values within ROIs. Error message is placed in pointer to string.
    bool buildPixelROIPipeline(std::shared_ptr<StainProfile>, std::shared_ptr<std::string>);
    ///build the pipeline for getting the stain vectors from the Macenko method. Error message is placed in pointer to string.
    bool buildMacenkoPipeline(std::shared_ptr<StainProfile>, std::shared_ptr<std::string>);
    ///build the pipeline for getting the stain vectors from non-negative matrix factorization. Error message is placed in pointer to string.
    bool buildNMFPipeline(std::shared_ptr<StainProfile>, std::shared_ptr<std::string>);

	///Create a text report that combines the output of the stain profile and any other reports
	std::string generateCompleteReport() const;
	///Create a text report summarizing the stain vector profile
	std::string generateStainProfileReport(std::shared_ptr<StainProfile>) const;
    ///Create a text report for the list of parameters from a model or algorithm
    std::string generateParameterMapReport(std::map<std::string, std::string> p) const;

    ///Define the save file dialog options outside of init
    sedeen::file::FileDialogOptions defineSaveFileDialogOptions();

    ///Save the stain profile as defined in the parameters to the file in the save file dialog
    bool SaveStainProfileToFile();

private:
    //Member parameters
    DisplayAreaParameter m_displayArea;
    TextFieldParameter m_nameOfStainProfile;
    OptionParameter m_numberOfStainComponents;
    
    ///Analysis model is how to create stain vectors, and there is one choice: Ruifrok and Johnston
    OptionParameter m_stainAnalysisModel;

    ///Choices are Manual Regions-of-Interest, Macenko Decomposition, Non-Negative Matrix Factorization
    OptionParameter m_stainSeparationAlgorithm;

    ///If using Macenko or NNMF, this determines whether to use all available pixels or not (NOT is the better choice)
    BoolParameter m_useSubsampleOfPixels;

    ///If useSubsampleOfPixels is True, set the number of pixels to sample from the WSI as m x 10^n
    algorithm::DoubleParameter  m_subsamplePixelsMantissa;
    ///Order of magnitude for the subsample of pixels
    algorithm::IntegerParameter m_subsamplePixelsMagnitude;

    ///Set the optical density threshold to omit pixels before computing stain vectors
    algorithm::DoubleParameter m_preComputationThreshold;

    //Stain One
    TextFieldParameter m_nameOfStainOne;
    //RegionListParameter m_regionListStainOne;
	GraphicItemParameter m_regionStainOne;
    //Stain Two
    TextFieldParameter m_nameOfStainTwo;
    //RegionListParameter m_regionListStainTwo;
	GraphicItemParameter m_regionStainTwo;
    //Stain Three
    TextFieldParameter m_nameOfStainThree;
    //RegionListParameter m_regionListStainThree;
	GraphicItemParameter m_regionStainThree;

    OptionParameter m_stainToDisplay;
    BoolParameter m_applyDisplayThreshold;
    /// User defined Threshold value for DISPLAY of the image (not computation).
    algorithm::DoubleParameter m_displayThreshold;

    BoolParameter m_showPreviewOnly;
    SaveFileDialogParameter m_saveFileAs;

	/// The output result
	ImageResult m_result;
	TextResult m_outputText;
	std::string m_report;

	/// The intermediate image factory after color deconvolution
	std::shared_ptr<image::tile::Factory> m_colorDeconvolution_factory;

private:
    //Member variables
    const std::vector<std::string> m_numComponentsOptions;
    std::vector<std::string> m_stainAnalysisModelOptions;
    std::vector<std::string> m_separationAlgorithmOptions;
    std::vector<std::string> m_stainToDisplayOptions;
    const double m_subsampleMantissaDefaultVal;
    const int    m_subsampleMagnitudeDefaultVal;
    const double m_computationThresholdDefaultVal;
    const double m_computationThresholdMaxVal;
    const double m_displayThresholdDefaultVal;
    const double m_displayThresholdMaxVal;

    const double m_algorithmPercentileDefaultVal;
    const int    m_algorithmHistogramBinsDefaultVal;

    ///The stain vector profile and its XML file handling
    std::shared_ptr<StainProfile> m_localStainProfile;
    ///Returns the shared_ptr to the local stain profile
    inline std::shared_ptr<StainProfile> GetLocalStainProfile() { return m_localStainProfile; }

};

} // namespace algorithm
} // namespace sedeen

#endif