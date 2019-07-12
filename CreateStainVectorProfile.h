/*=============================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
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
#include <fstream>
#include <memory>

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

    ///Define the save file dialog options outside of init
    sedeen::file::FileDialogOptions defineSaveFileDialogOptions();

	/// Creates the Color Deconvolution pipeline with a cache
	//
	/// \return 
	/// TRUE if successful, false on error or failure
	bool buildPipeline(std::shared_ptr<StainProfile>);
    /// Test whether the values or states of the UI parameters have changed
    bool checkParametersChanged(bool);

	///Create a text report that combines the output of the stain profile and any other reports
	std::string generateCompleteReport() const;
	///Create a text report summarizing the stain vector profile
	std::string generateStainProfileReport(std::shared_ptr<StainProfile>) const;

    ///Save the stain profile as defined in the parameters to the file in the save file dialog
    bool SaveStainProfileToFile();

private:
    //Member parameters
    DisplayAreaParameter m_displayArea;
    TextFieldParameter m_nameOfStainProfile;
    OptionParameter m_numberOfStainComponents;

	//TODO: switch from single region per stain to multiple (RegionListParameter)
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

    OptionParameter m_stainSeparationAlgorithm;
    OptionParameter m_stainToDisplay;
    BoolParameter m_applyThreshold;
    /// User defined Threshold value.
    algorithm::DoubleParameter m_threshold;

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
	std::vector<std::string> m_separationAlgorithmOptions;
    std::vector<std::string> m_numComponentsOptions;
    std::vector<std::string> m_stainToDisplayOptions;
    double m_thresholdDefaultVal;
    double m_thresholdMaxVal;

    ///The stain vector profile and its XML file handling
    std::shared_ptr<StainProfile> m_localStainProfile;
    ///Returns the shared_ptr to the local stain profile
    inline std::shared_ptr<StainProfile> GetLocalStainProfile() { return m_localStainProfile; }

};

} // namespace algorithm
} // namespace sedeen

#endif