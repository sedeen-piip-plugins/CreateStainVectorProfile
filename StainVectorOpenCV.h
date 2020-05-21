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

#ifndef SEDEEN_SRC_FILTER_STAINVECTOROPENCV_H
#define SEDEEN_SRC_FILTER_STAINVECTOROPENCV_H

#include "Global.h"
#include "Geometry.h"
#include "Image.h"

#include "StainVectorBase.h"

 //OpenCV include
#include <opencv2/core/core.hpp>

namespace sedeen {
namespace image {

class PATHCORE_IMAGE_API StainVectorOpenCV : public StainVectorBase {
public:
    StainVectorOpenCV(std::shared_ptr<tile::Factory> source);
    virtual ~StainVectorOpenCV();

    ///Utility method to check the equality of the contents of two CV InputArrays (mat, vec, etc.)
    static const bool AreEqual(cv::InputArray array1, cv::InputArray array2);

protected:
    ///Convert stain vector data as 9-element double C array to OpenCV matrix (as row vectors)
    void StainCArrayToCVMat(double (&inutVectors)[9], cv::OutputArray outputData, 
        const bool normalize = false, const int _numRows = -1) const;
    ///Convert stain vector data from OpenCV matrix (as row vectors) to 9-element double C array
    void StainCVMatToCArray(cv::InputArray inputData, double (&outputVectors)[9], 
        const bool normalize = false) const;

};

} // namespace image
} // namespace sedeen
#endif
