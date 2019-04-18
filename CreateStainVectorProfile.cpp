/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

// CreateStainVectorProfile.cpp : Defines the exported functions for the DLL application.
//
// Primary header
#include "CreateStainVectorProfile.h"

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
    m_regionListStainOne(),
    m_nameOfStainTwo(),
    m_regionListStainTwo(),
    m_nameOfStainThree(),
    m_regionListStainThree(),
    m_stainSeparationAlgorithm(),
    m_stainToDisplay(),
    m_showPreviewOnly(),
    m_saveFileAs(),
    m_numComponentsOptions(),
    m_stainToDisplayOptions()
{
    //Define the numberOfStainComponents options
    //TEMP: rely on the text and the index being the same
    m_numComponentsOptions.push_back("0");
    m_numComponentsOptions.push_back("1");
    m_numComponentsOptions.push_back("2");
    m_numComponentsOptions.push_back("3");

    //Define the default list of names of stains to display
    m_stainToDisplayOptions.push_back("Stain 1");
    m_stainToDisplayOptions.push_back("Stain 2");
    m_stainToDisplayOptions.push_back("Stain 3");

    //Create a stain vector profile. It is only updated in the 'run' method
    //A list of available stain separation algorithms is defined in its constructor
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

    //Names of stains and ROIs associated with them
    m_nameOfStainOne = createTextFieldParameter(*this, "Name of Stain 1",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainOne = createRegionListParameter(*this, "Stain 1 Regions", 
        "List of Regions of Interest for Stain 1", true);

    m_nameOfStainTwo = createTextFieldParameter(*this, "Name of Stain 2",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainTwo = createRegionListParameter(*this, "Stain 2 Regions", 
        "List of Regions of Interest for Stain 2", true);

    m_nameOfStainThree = createTextFieldParameter(*this, "Name of Stain 3",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainThree = createRegionListParameter(*this, "Stain 3 Regions", 
        "List of Regions of Interest for Stain 3", true);

    //Get the list of available stain separation algorithms from the stain profile object
    std::vector<std::string> theStainSeparationOptions 
        = m_localStainProfile->GetStainSeparationAlgorithmOptions();
    m_stainSeparationAlgorithm = createOptionParameter(*this, "Stain Separation Algorithm", 
        "Select the stain separation algorithm to use to separate the stain components", 0, 
        theStainSeparationOptions, false);

    //List of options of the stains currently defined, to show in preview
    m_stainToDisplay = createOptionParameter(*this, "Show Separated Stain", 
        "Choose which of the defined stains to preview in the display area", 0, m_stainToDisplayOptions, false);
 
    //Allow the user to create visible output, without saving the stain vector profile to a file
    m_showPreviewOnly = createBoolParameter(*this, "Preview Only",
        "If set to Preview Only, clicking Run will create separated images, but will not save the vectors to file",
        true, false);

    //Allow the user to choose where to save the new stain vector profile
    sedeen::file::FileDialogOptions saveFileDialogOptions = defineSaveFileDialogOptions();
    m_saveFileAs = createSaveFileDialogParameter(*this, "Save As...", 
        "Choose where to save the stain vector profile. If Preview Only is checked, the profile will not be saved.",
        saveFileDialogOptions, true);

    //Further initial configuration settings
    //... stuff.

}//end init

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



///Method called when the 'Run' button is clicked
void CreateStainVectorProfile::run() {

    /*
    //Determine if this is a preview only run, or if an output file should be written
    if (m_showPreviewOnly == false) {



    }
    else if (m_showPreviewOnly == true) {


    }
    else {
        //Indeterminate state. Throw error or do nothing. TBD.
    }
    */

    //std::string outFileName = m_saveFileAs;
    //bool fileNameIsValid = this->GetLocalStainProfile()->checkFile(m_saveFileAs);


    //If m_showPreviewOnly is false, assign new values to m_localStainProfile
    //if (m_showPreviewOnly == false) {


    //TEMP!!!!
    if (true) {
        //Assign values from the parameters to the local stain profile object
        //Take advantage of the implicit conversion operators in the parameter definitions
        m_localStainProfile->SetNameOfStainProfile(m_nameOfStainProfile);
        m_localStainProfile->SetNumberOfStainComponents(m_numberOfStainComponents);
        m_localStainProfile->SetNameOfStainOne(m_nameOfStainOne);
        m_localStainProfile->SetNameOfStainTwo(m_nameOfStainTwo);
        m_localStainProfile->SetNameOfStainThree(m_nameOfStainThree);
        //The implicit conversion for m_stainSeparationAlgorithm is an int of the option number
        //Get the text of the name of the stain separation algorithm from the vector
        //of names stored in m_localStainProfile
        int stainAlgNumber = m_stainSeparationAlgorithm;
        std::string stainAlgName = m_localStainProfile->GetStainSeparationAlgorithmName(stainAlgNumber);
        m_localStainProfile->SetNameOfStainSeparationAlgorithm(stainAlgName);

        //Assign the RGB values after processing


        //TEMP VALUES!!!
        m_localStainProfile->SetStainOneRGB(0.1, 0.1, 0.1);
        m_localStainProfile->SetStainTwoRGB(0.3, 0, 0);
        m_localStainProfile->SetStainThreeRGB(0, 255, 0);

        //Check if the data are valid, write to the 
        //m_localStainProfile->checkFile(m_saveFileAs);


        //Write to some kind of temp file!
        bool writeSuccessful = m_localStainProfile->writeStainProfile("temp");

        //Make a new temporary stain profile
        std::shared_ptr<StainProfile> readBack = std::make_shared<StainProfile>();
        bool readSuccessful = readBack->readStainProfile("temp");

    }


}//end run



/*
if (selectedFileTobeProcessed_.empty())
{
    sedeen::algorithm::parameter::OpenFileDialog::DataType openFileDialogDataType = openFileDialogParam_;
    selectedFileTobeProcessed_ = openFileDialogDataType.at(0).getFilename();
    if (!openFileDialogParam_.isUserDefined() || selectedFileTobeProcessed_.empty())
    {
        //msgBox.setText("Out put directory not set.");
        //int ret = msgBox.exec();
        throw std::runtime_error("You did not select the correct file format!");
        //MessageBox(nullptr, LPCSTR("Please select a directory to save tiles!"), LPCSTR("Notification"), MB_OKCANCEL);
        return;
    }
}
*/



/*
std::string CreateStainVectorProfile::openFile(std::string path)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    //HACK
    std::string m_path_to_root = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "*.csv";
    ofn.lpstrFile = (LPSTR)szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    //ofn.lpstrDefExt = (LPSTR)L"tif";
    ofn.lpstrInitialDir = (LPSTR)m_path_to_root.c_str();
    GetOpenFileName(&ofn);

    return ofn.lpstrFile;
}//end openFile
*/



} // namespace algorithm
} // namespace sedeen


