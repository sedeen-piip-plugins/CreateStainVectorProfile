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

#include "StainVectorOpenCV.h"
#include "StainVectorMath.h"

namespace sedeen {
namespace image {

StainVectorOpenCV::StainVectorOpenCV(std::shared_ptr<tile::Factory> source) 
    : StainVectorBase(source)
{}//end constructor

StainVectorOpenCV::~StainVectorOpenCV(void) {
}//end destructor

const bool StainVectorOpenCV::AreEqual(cv::InputArray array1, cv::InputArray array2) {
    // treat two empty arrays as identical
    if (array1.empty() && array2.empty()) {
        return true;
    }
    // if dimensionality is not identical, these arrays are not identical
    if (array1.cols() != array2.cols() || array1.rows() != array2.rows() || array1.dims() != array2.dims()) {
        return false;
    }
    //Compare NOT equal, then count NON-zero (there isn't a countZero function in OpenCV).
    cv::Mat diff;
    cv::compare(array1, array2, diff, cv::CmpTypes::CMP_NE);
    int nz = cv::countNonZero(diff);
    return (nz == 0);
}//end AreEqual

void StainVectorOpenCV::StainCArrayToCVMat(double(&inputVectors)[9], cv::OutputArray outputData,
    const bool normalize /* = false*/, const int _numRows /*= -1 */) const {
    //If _numRows == 0, no output should be produced
    if (_numRows == 0) { return; }
    //Possible values for outRows are 1, 2, or 3
    int outRows = (_numRows < 0) || (_numRows > 3) ? 3 : _numRows;

    //If normalize is true, fill an array made unitary, else make a copy of the input data
    double inputCopy[9] = { 0.0 };
    if (normalize) {
        StainVectorMath::Make3x3MatrixUnitary(inputVectors, inputCopy);
    }
    else {
        std::copy(std::begin(inputVectors), std::end(inputVectors), std::begin(inputCopy));
    }
    //Create a cv::Mat of type double, copy inputCopy data
    cv::Mat inputMatFlat(1, 9, cv::DataType<double>::type);
    std::copy(std::begin(inputCopy), std::end(inputCopy), inputMatFlat.begin<double>());
    //Reshape the matrix (does not reallocate or copy data)
    cv::Mat inputMatSquare = inputMatFlat.reshape(0, 3);
    cv::Mat outputMat = inputMatSquare.rowRange(cv::Range(0, outRows));
    outputData.assign(outputMat);
}//end StainCArrayToCVMat

void StainVectorOpenCV::StainCVMatToCArray(cv::InputArray inputData, double(&outputVectors)[9], 
    const bool normalize /* = false*/) const {
    double tempOutput[9] = { 0.0 };
    if (inputData.empty()) { return; }
    cv::Mat inputMatSquare, _inputMat(inputData.getMat());
    _inputMat.convertTo(inputMatSquare, cv::DataType<double>::type);
    //Reshape the matrix and get the data
    int numElements = static_cast<int>(inputMatSquare.total());
    if (numElements % 3 != 0) { return; }
    //Reshape the matrix (does not reallocate memory)
    cv::Mat inputMatFlat = inputMatSquare.reshape(0, 1);
    //Fill the tempOutput array with data elements
    std::copy(inputMatFlat.begin<double>(), inputMatFlat.end<double>(), std::begin(tempOutput));
    //Pad the tempOutput array if necessary
    std::fill(std::begin(tempOutput) + numElements, std::end(tempOutput), 0.0);
    //If normalize is true, make outputVectors normalized, else directly copy from tempOutput
    if (normalize) {
        StainVectorMath::Make3x3MatrixUnitary(tempOutput, outputVectors);
    }
    else {
        std::copy(std::begin(tempOutput), std::end(tempOutput), std::begin(outputVectors));
    }
}//end StainCVMatToCArray

} // namespace image
} // namespace sedeen
