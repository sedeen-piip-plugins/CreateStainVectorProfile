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

// CreateStainVectorProfile.cpp : Defines the exported functions for the DLL application.
//
// Primary header
#include "CreateStainVectorProfile.h"
#include "StainRGBFromROI.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

// Sedeen headers
#include "Algorithm.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"
#include "image/io/Image.h"
#include "image/tile/Factory.h"

// Poco header for manifest declaration
#include <Poco/ClassLibrary.h>

// Declare that this object has AlgorithmBase subclasses
//  and declare each of those sub-classes
POCO_BEGIN_MANIFEST(sedeen::algorithm::AlgorithmBase)
POCO_EXPORT_CLASS(sedeen::algorithm::CreateStainVectorProfile)
POCO_END_MANIFEST

namespace sedeen {
namespace algorithm {

///Constructor
CreateStainVectorProfile::CreateStainVectorProfile()
    : m_displayArea(),
    m_nameOfStainProfile(),
    m_numberOfStainComponents(),
    m_nameOfStainOne(),
    m_regionStainOne(),
    m_nameOfStainTwo(),
    m_regionStainTwo(),
    m_nameOfStainThree(),
    m_regionStainThree(),
    m_stainAnalysisModel(),
    m_stainSeparationAlgorithm(),
    m_useSubsampleOfPixels(),
    m_subsamplePixelsMantissa(),
    m_subsamplePixelsMagnitude(),
    m_preComputationThreshold(),
    m_stainToDisplay(),
    m_applyDisplayThreshold(),
    m_displayThreshold(),
    m_showPreviewOnly(),
    m_saveFileAs(),
	m_result(),
	m_outputText(),
	m_report(""),
    //Set member const values
    m_subsampleMantissaDefaultVal(1.0),
    m_subsampleMagnitudeDefaultVal(5),
    m_computationThresholdDefaultVal(15.0),
    m_computationThresholdMaxVal(300.0),
    m_displayThresholdDefaultVal(20.0),
    m_displayThresholdMaxVal(300.0),
    m_algorithmPercentileDefaultVal(1.0),
    m_algorithmHistogramBinsDefaultVal(1024),
	m_colorDeconvolution_factory(nullptr),
    //Define the numberOfStainComponents options
    m_numComponentsOptions({"0", "1", "2", "3"})
{
    //Define the default list of names of stains to display (someday I hope these can be mutable)
    m_stainToDisplayOptions.push_back("Stain 1");
    m_stainToDisplayOptions.push_back("Stain 2");
    m_stainToDisplayOptions.push_back("Stain 3");

    //Populate the analysis model and separation algorithm lists with a temporary StainProfile
    auto tempStainProfile = std::make_shared<StainProfile>();
    //The stain analysis model options
    m_stainAnalysisModelOptions = tempStainProfile->GetStainAnalysisModelOptions();
    //The stain separation algorithm options
    m_separationAlgorithmOptions = tempStainProfile->GetStainSeparationAlgorithmOptions();
    //Clean up
    tempStainProfile.reset();

    //Create a stain vector profile. It is only updated in the 'run' method
    //Lists of available analysis models and separation algorithms are defined in its constructor
    m_localStainProfile = std::make_shared<StainProfile>();

}//end constructor

///Destructor
CreateStainVectorProfile::~CreateStainVectorProfile() {
}//end destructor

///Initial setup of the plugin
void CreateStainVectorProfile::init(const image::ImageHandle& image) {
    if (isNull(image)) return;

    // Bind system parameter for current view
    m_displayArea = createDisplayAreaParameter(*this);

    //Bind algorithm members to UI and initialize properties
    m_nameOfStainProfile = createTextFieldParameter(*this, "Name of stain profile", 
        "Enter a name for your new stain vector profile", "", true);

    //Create list of options for number of stain components
    m_numberOfStainComponents = createOptionParameter(*this, "Number of Stain Components", 
        "Choose the number of stains in the image", 0, m_numComponentsOptions, false);

    //The displayed list of available stain analysis models (one: stain deconvolution)
    m_stainAnalysisModel = createOptionParameter(*this, "Stain Analysis Model",
        "Select the analysis model to use (currently one option: stain deconvolution)", 0,
        m_stainAnalysisModelOptions, false);

    //The displayed list of available stain separation algorithms
    m_stainSeparationAlgorithm = createOptionParameter(*this, "Separation Algorithm",
        "Select the stain separation algorithm to use to separate the stain components", 0,
        m_separationAlgorithmOptions, false);

    m_useSubsampleOfPixels = createBoolParameter(*this, "Use sub-sample of pixels",
        "If checked, only a sub-set of the total number of pixels will be used to compute the stain vectors",
        true, false); //default value, optional

    m_subsamplePixelsMantissa = createDoubleParameter(*this, "Num pixels (sci notation: m x 10^n)",
        "The number of pixels to include in a sub-sample for stain vector computation is set using two values: the mantissa and the order of magnitude (m x 10^n)", 
        m_subsampleMantissaDefaultVal, 0.0, 10.0, false);

    auto imageSize = sedeen::image::getDimensions(image, 0);
    int maxPower = static_cast<int>(std::ceil((std::log10(static_cast<double>(imageSize.width()*imageSize.height())))));
    int magDefaultVal = (m_subsampleMagnitudeDefaultVal <= maxPower) ? m_subsampleMagnitudeDefaultVal : maxPower;
    m_subsamplePixelsMagnitude = createIntegerParameter(*this, "Num pixels order of magnitude",
        "The number of pixels to include in a sub-sample for stain vector computation is set using two values: the mantissa and the order of magnitude (m x 10^n)",
        magDefaultVal, 0, maxPower, false);

    //Set the threshold applied before computing the stain vectors (by whichever method)
    m_preComputationThreshold = createDoubleParameter(*this,
        "OD x100 Threshold (pre-computation)",   // Widget label
        "Threshold applied before computing stain vectors (threshold for display is set below)",
        m_computationThresholdDefaultVal, // Initial value
        0.0,                              // minimum value
        m_computationThresholdMaxVal,     // maximum value
        false);

    //Names of stains and ROIs associated with them
    m_nameOfStainOne = createTextFieldParameter(*this, "Name of Stain 1",
        "Enter the name of a stain in the image", "", true);
    //m_regionListStainOne = createRegionListParameter(*this, "Stain 1 Regions", 
    //    "List of Regions of Interest for Stain 1", true);
	m_regionStainOne = createGraphicItemParameter(*this, "Stain 1 Region",
		"Region of Interest for Stain 1", true);

    m_nameOfStainTwo = createTextFieldParameter(*this, "Name of Stain 2",
        "Enter the name of a stain in the image", "", true);
    //m_regionListStainTwo = createRegionListParameter(*this, "Stain 2 Regions", 
    //    "List of Regions of Interest for Stain 2", true);
	m_regionStainTwo = createGraphicItemParameter(*this, "Stain 2 Region",
		"Region of Interest for Stain 2", true);

    m_nameOfStainThree = createTextFieldParameter(*this, "Name of Stain 3",
        "Enter the name of a stain in the image", "", true);
    //m_regionListStainThree = createRegionListParameter(*this, "Stain 3 Regions", 
    //    "List of Regions of Interest for Stain 3", true);
	m_regionStainThree = createGraphicItemParameter(*this, "Stain 3 Region",
		"Region of Interest for Stain 3", true);

    //List of options of the stains currently defined, to show in preview
    m_stainToDisplay = createOptionParameter(*this, "Show Separated Stain", 
        "Choose which of the defined stains to preview in the display area", 0, m_stainToDisplayOptions, false);
 
    //User can choose whether to apply the threshold or not
    m_applyDisplayThreshold = createBoolParameter(*this, "Display with Threshold Applied",
        "If Display with Threshold Applied is set, the threshold value in the slider below will be applied to the stain-separated image",
        true, false); //default value, optional

    // Init the user defined threshold value
    //TEMPORARY!! Can't set precision on DoubleParameter right now, so use 1/100 downscale
    //auto color = getColorSpace(image);
    m_displayThreshold = createDoubleParameter(*this,
        "OD x100 Threshold (display)",   // Widget label
        "Threshold applied to the DISPLAYED image (the threshold to use when computing stain vectors is a separate slider)",   // Widget tooltip
        m_displayThresholdDefaultVal, // Initial value
        0.0,                          // minimum value
        m_displayThresholdMaxVal,     // maximum value
        false);

    //Allow the user to create visible output, without saving the stain vector profile to a file
    m_showPreviewOnly = createBoolParameter(*this, "Preview Only",
        "If set to Preview Only, clicking Run will create separated images, but will not save the vectors to file",
        false, false);

    //Allow the user to choose where to save the new stain vector profile
    sedeen::file::FileDialogOptions saveFileDialogOptions = defineSaveFileDialogOptions();
    m_saveFileAs = createSaveFileDialogParameter(*this, "Save As...", 
        "Choose where to save the stain vector profile. If Preview Only is checked, the profile will not be saved.",
        saveFileDialogOptions, true);

    // Bind result
    m_outputText = createTextResult(*this, "Text Result");
    m_result = createImageResult(*this, " StainAnalysisResult");

}//end init

void CreateStainVectorProfile::initialVisibility() {
    m_nameOfStainProfile.setVisible(true);
    m_numberOfStainComponents.setVisible(true);
    m_stainAnalysisModel.setVisible(false);
    m_stainSeparationAlgorithm.setVisible(true);
    m_useSubsampleOfPixels.setVisible(true);
    m_subsamplePixelsMantissa.setVisible(true);
    m_subsamplePixelsMagnitude.setVisible(true);
    m_preComputationThreshold.setVisible(true);

    m_nameOfStainOne.setVisible(true);
    m_regionStainOne.setVisible(true);
    m_nameOfStainTwo.setVisible(true);
    m_regionStainTwo.setVisible(true);
    m_nameOfStainThree.setVisible(true);
    m_regionStainThree.setVisible(true);

    m_stainToDisplay.setVisible(true);
    m_applyDisplayThreshold.setVisible(true);
    m_showPreviewOnly.setVisible(true);
    m_saveFileAs.setVisible(true);
}//end initialVisibility

///Method called when the 'Run' button is clicked
void CreateStainVectorProfile::run() {
    //Get the reference to the localStainProfile
    auto theProfile = this->GetLocalStainProfile();

	// Has display area changed
	bool display_changed = m_displayArea.isChanged();
    //Have the states of the many parameters changed
    bool parameters_changed = checkParametersChanged(false);

	// Update results
	if (parameters_changed || display_changed) {
        //Check whether to write to file, that the field is not blank,
        //and that the file can be created or written to
        if (m_showPreviewOnly == false) {
            //Get the full path file name from the file dialog parameter
            sedeen::algorithm::parameter::SaveFileDialog::DataType fileDialogDataType = this->m_saveFileAs;
            std::string theFile = fileDialogDataType.getFilename();
            //Is the file field blank?
            if (theFile.empty()) {
                m_outputText.sendText("The filename is blank. Please choose a file to save the profile to, or select Preview Only.");
                return;
            }
            //Does it exist or can it be created, and can it be written to?
            bool validFileCheck = StainProfile::checkFile(theFile, "w");
            if (!validFileCheck) {
                m_outputText.sendText("The filename selected cannot be written to. Please choose another, or check the permissions of the directory.");
                return;
            }
        }

        //Clear the values in the StainProfile
        theProfile->ClearProfile();
        //Assign values from the parameters to the local stain profile object
        //Take advantage of the implicit conversion operators in the parameter definitions
        theProfile->SetNameOfStainProfile(m_nameOfStainProfile);
        theProfile->SetNumberOfStainComponents(m_numberOfStainComponents);
        theProfile->SetNameOfStainOne(m_nameOfStainOne);
        theProfile->SetNameOfStainTwo(m_nameOfStainTwo);
        theProfile->SetNameOfStainThree(m_nameOfStainThree);

        //The implicit conversion for m_stainAnalysisModel is an int of the option number
        //Get the text of the name of the stain analysis model from the vector of
        //names stored in the localStainProfile
        int stainModelNumber = m_stainAnalysisModel;
        std::string stainModelName = theProfile->GetStainAnalysisModelName(stainModelNumber);
        theProfile->SetNameOfStainAnalysisModel(stainModelName);

        //The implicit conversion for m_stainSeparationAlgorithm is an int of the option number
        //Get the text of the name of the stain separation algorithm from the vector of
        //names stored in the localStainProfile
        int stainAlgNumber = m_stainSeparationAlgorithm;
        std::string stainAlgName = theProfile->GetStainSeparationAlgorithmName(stainAlgNumber);
        theProfile->SetNameOfStainSeparationAlgorithm(stainAlgName);


        //Test writing parameter values to the stain profile. Need to get percentile, num histogram bins from Macenko
        //set computation threshold and number of pixels used, if NNMF or Macenko

        //number of pixels and the threshold can both be obtained from the GUI parameters
        //It is possible for this value to exceed the size of a 32-bit int, so use long
        double numPixelsDouble = m_subsamplePixelsMantissa * std::pow(10.0, m_subsamplePixelsMagnitude);
        long int numPixels = static_cast<long int>(numPixelsDouble);
        double compThreshold = m_preComputationThreshold;

        //For the Mackenko (and Niethammer) method, set the percentile limit 
        //and number of bins in the histogram from the default values set in this class
        double percentileThreshold = m_algorithmPercentileDefaultVal;
        int numHistoBins = m_algorithmHistogramBinsDefaultVal;

        //Set the four parameter values
        theProfile->SetSeparationAlgorithmNumPixelsParameter(numPixels);
        theProfile->SetSeparationAlgorithmThresholdParameter(compThreshold);
        theProfile->SetSeparationAlgorithmPercentileParameter(percentileThreshold);
        theProfile->SetSeparationAlgorithmHistogramBinsParameter(numHistoBins);


        //test clear the analysis model parameters
        theProfile->ClearAnalysisModelParameters();
        //Test setting a single value
        theProfile->SetSingleAnalysisModelParameter(theProfile->pTypeNumPixels(), "12345");



        //Calculate the stain vectors and build the operational pipeline
        bool buildSuccessful = buildPipeline(theProfile);
        if (!buildSuccessful) {
            m_outputText.sendText("Could not calculate the stain vectors. Please check your regions of interest and try again.");
            return;
        }





        //A previous version included an "intermediate result" here, to display
        //a blurry temporary image (rather than just black) while calculations proceeded

        //Update the display area with the deconvolution output
        if (nullptr != m_colorDeconvolution_factory) {
            m_result.update(m_colorDeconvolution_factory, m_displayArea, *this);
        }

        // Update the output text report
        if (false == askedToStop()) {
            auto report = generateCompleteReport();
            m_outputText.sendText(report); 
        }

        //If an output file should be written, save the localStainProfile to it
        if (m_showPreviewOnly == false) {
            //Send the contents of the localStainProfile to the given file
            bool saveResult = SaveStainProfileToFile();

            //Check whether saving to the file was successful
            if (!saveResult) {
                m_outputText.sendText("Could not save the stain profile to the chosen file. Verify that the file name is correct, and try again.");
                return;
            }
        }
        else { //m_showPreviewOnly == true
            //do nothing 
        }
	}//end if (pipeline_changed || display_changed)

    // Ensure we run again after an abort
    // a small kludge that causes buildPipeline() to return TRUE
    if (askedToStop() && (nullptr != m_colorDeconvolution_factory)) {
        m_colorDeconvolution_factory.reset();
    }
}//end run

///Define the save file dialog options outside of init
sedeen::file::FileDialogOptions CreateStainVectorProfile::defineSaveFileDialogOptions() {
    sedeen::file::FileDialogOptions theOptions;
    theOptions.caption = "Save stain vector profile as...";
    //theOptions.flags = sedeen::file::FileDialogFlags:: None currently needed
    //theOptions.startDir; //no current preference
    //Define the file type dialog filter
    sedeen::file::FileDialogFilter theDialogFilter;
    theDialogFilter.name = "Stain Vector Profile (*.xml)";
    theDialogFilter.extensions.push_back("xml");
    theOptions.filters.push_back(theDialogFilter);
    return theOptions;
}//end defineSaveFileDialogOptions

bool CreateStainVectorProfile::SaveStainProfileToFile() {
    //Get the full path file name from the file dialog parameter
    sedeen::algorithm::parameter::SaveFileDialog::DataType fileDialogDataType = this->m_saveFileAs;
    std::string theFile = fileDialogDataType.getFilename();
    //Does it exist or can it be created, and can it be written to?
    if (StainProfile::checkFile(theFile, "w")) {
        m_localStainProfile->writeStainProfile(theFile);
        return true;
    }
    else {
        return false;
    }
    return false;
}//end SaveStainProfileToFile

bool CreateStainVectorProfile::checkParametersChanged(bool somethingChanged) {
    //Check if any of the parameters changed, plus an external value to check
    if (somethingChanged
        || m_nameOfStainProfile.isChanged()
        || m_numberOfStainComponents.isChanged()
        || m_stainAnalysisModel.isChanged()
        || m_stainSeparationAlgorithm.isChanged()
        || m_useSubsampleOfPixels.isChanged()
        || m_subsamplePixelsMantissa.isChanged()
        || m_subsamplePixelsMagnitude.isChanged()
        || m_preComputationThreshold.isChanged()
        || m_nameOfStainOne.isChanged()
        || m_regionStainOne.isChanged()
        || m_nameOfStainTwo.isChanged()
        || m_regionStainTwo.isChanged()
        || m_nameOfStainThree.isChanged()
        || m_regionStainThree.isChanged()
        || m_stainToDisplay.isChanged()
        || m_applyDisplayThreshold.isChanged()
        || m_displayThreshold.isChanged()
        || m_showPreviewOnly.isChanged()
        || m_displayArea.isChanged()
        || (nullptr == m_colorDeconvolution_factory))
    {
        return true;
    }
    else
    {
        return false;
    }
}//end checkParametersChanged

bool CreateStainVectorProfile::buildPipeline(std::shared_ptr<StainProfile> theProfile) {
    using namespace image::tile;
    bool buildSuccessful = false;

    // Get source image properties
    auto source_factory = image()->getFactory();
    auto source_color = source_factory->getColorSpace();

    //Choose value from the enumeration in ColorDeconvolution
    image::tile::ColorDeconvolution::DisplayOptions DisplayOption;
    switch (m_stainToDisplay)
    {
    case 0:
        DisplayOption = image::tile::ColorDeconvolution::DisplayOptions::STAIN1;
        break;
    case 1:
        DisplayOption = image::tile::ColorDeconvolution::DisplayOptions::STAIN2;
        break;
    case 2:
        DisplayOption = image::tile::ColorDeconvolution::DisplayOptions::STAIN3;
        break;
    default:
        break;
    }

    //Build the color deconvolution channel
    int numStains = m_numberOfStainComponents;
    bool oneDefined   = m_regionStainOne.isUserDefined();
    bool twoDefined   = m_regionStainTwo.isUserDefined();
    bool threeDefined = m_regionStainThree.isUserDefined();

    std::vector<std::shared_ptr<GraphicItemBase>> regionsOfInterestVector;
    if ((numStains <= 0) || (numStains > 3)) {
        return false;
    }
    else if (numStains > 0) {
        if (oneDefined) {
            std::shared_ptr<GraphicItemBase> region = m_regionStainOne;
            regionsOfInterestVector.push_back(region);
            Rect rect = containingRect(regionsOfInterestVector.at(0)->graphic());
        }
        else {
            m_outputText.sendText("Stain 1 region of interest is not defined. Please define a region to use to calculate the stain vector.");
            return false;
        }
    }
    //These if statements are cumulative, not "else if"
    if (numStains > 1) {
        if (twoDefined) {
            std::shared_ptr<GraphicItemBase> region = m_regionStainTwo;
            regionsOfInterestVector.push_back(region);
            Rect rect = containingRect(regionsOfInterestVector.at(1)->graphic());
        }
        else {
            m_outputText.sendText("Stain 2 region of interest is not defined. Please define a region to use to calculate the stain vector.");
            return false;
        }
    }
    //Cumulative
    if (numStains > 2) {
        if (threeDefined) {
            std::shared_ptr<GraphicItemBase> region = m_regionStainThree;
            regionsOfInterestVector.push_back(region);
            Rect rect = containingRect(regionsOfInterestVector.at(2)->graphic());
        }
        else {
            m_outputText.sendText("Stain 3 region of interest is not defined. Please define a region to use to calculate the stain vector.");
            return false;
        }
    }
    //Pass the regions of interest to the getStainsComponents function
    double conv_matrix[9] = { 0.0 };
    auto display_resolution = getDisplayResolution(image(), m_displayArea);

    sedeen::image::getStainsComponents(source_factory,
        regionsOfInterestVector, display_resolution, conv_matrix);

    //Assign the output of getStainsComponents to the StainProfile 
    bool assignCheck = theProfile->SetProfilesFromDoubleArray(conv_matrix);

    if (!assignCheck) {
        return false;
    }

    //Send information to the kernel
    //TEMPORARY! Note that the threshold value must be divided by 100 here,
    //because it is not possible to set the precision of a double parameter as of Sedeen 5.4.1
    auto colorDeconvolution_kernel =
        std::make_shared<image::tile::ColorDeconvolution>(DisplayOption, theProfile,
            m_applyDisplayThreshold, m_displayThreshold/100.0);  //Need to tell it whether to use the threshold or not

    // Create a Factory for the composition of these Kernels
    auto non_cached_factory =
        std::make_shared<FilterFactory>(source_factory, colorDeconvolution_kernel);

    // Wrap resulting Factory in a Cache for speedy results
    m_colorDeconvolution_factory =
        std::make_shared<Cache>(non_cached_factory, RecentCachePolicy(30));

    buildSuccessful = true;
    return buildSuccessful;
}//end buildPipeline

std::string CreateStainVectorProfile::generateCompleteReport() const {
    //Combine the output of the stain profile report
    //and the pixel fraction report, return the full string
    std::ostringstream ss;
    ss << generateStainProfileReport(m_localStainProfile);
    return ss.str();
}//end generateCompleteReport

std::string CreateStainVectorProfile::generateStainProfileReport(std::shared_ptr<StainProfile> theProfile) const
{
    //I think using assert is a little too strong here. Use different error handling.
    assert(nullptr != theProfile);

    int numStains = theProfile->GetNumberOfStainComponents();
    if (numStains < 0) {
        return "Error reading the stain profile. Please change your settings and try again.";
    }
    //Get the profile contents, place in the output stringstream
    std::ostringstream ss;
    ss << std::left << std::setw(5);
    ss << "Using stain profile: " << theProfile->GetNameOfStainProfile() << std::endl;
    ss << "Number of component stains: " << numStains << std::endl;
    ss << std::endl;

    //These are cumulative, not if...else
    //Stain one
    if (numStains >= 1) {
        std::array<double, 3> rgb = theProfile->GetStainOneRGB();
        ss << std::left;
        ss << "Stain 1: " << theProfile->GetNameOfStainOne() << std::endl;
        ss << "R: " << std::setw(10) << std::setprecision(5) << rgb[0] <<
              "G: " << std::setw(10) << std::setprecision(5) << rgb[1] <<
              "B: " << std::setw(10) << std::setprecision(5) << rgb[2] <<
              std::endl;
    }
    //Stain two
    if (numStains >= 2) {
        std::array<double, 3> rgb = theProfile->GetStainTwoRGB();
        ss << std::left;
        ss << "Stain 2: " << theProfile->GetNameOfStainTwo() << std::endl;
        ss << "R: " << std::setw(10) << std::setprecision(5) << rgb[0] <<
              "G: " << std::setw(10) << std::setprecision(5) << rgb[1] <<
              "B: " << std::setw(10) << std::setprecision(5) << rgb[2] <<
              std::endl;
    }
    //Stain three
    if (numStains == 3) {
        std::array<double, 3> rgb = theProfile->GetStainThreeRGB();
        ss << std::left;
        ss << "Stain 3: " << theProfile->GetNameOfStainThree() << std::endl;
        ss << "R: " << std::setw(10) << std::setprecision(5) << rgb[0] <<
              "G: " << std::setw(10) << std::setprecision(5) << rgb[1] <<
              "B: " << std::setw(10) << std::setprecision(5) << rgb[2] <<
              std::endl;
    }
    //Complete, return the string
    return ss.str();
}//end generateStainProfileReport

} // namespace algorithm
} // namespace sedeen

