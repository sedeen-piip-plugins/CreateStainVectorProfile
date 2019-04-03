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
#include <math.h>

// Sedeen headers
#include "Algorithm.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"
#include "image/io/Image.h"
#include "image/tile/Factory.h"

// Poco header for manifest declaration
#include <Poco/ClassLibrary.h>
//Poco headers for Events


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
    m_separationAlgorithmOptions(),
    m_stainToDisplayOptions()
{
    //Define the numberOfStainComponents options
    m_numComponentsOptions.push_back("0");
    m_numComponentsOptions.push_back("1");
    m_numComponentsOptions.push_back("2");
    m_numComponentsOptions.push_back("3");

    //Define the list of available stain separation algorithms
    m_separationAlgorithmOptions.push_back("Ruifrok Colour Deconvolution");

    //Define the default list of names of stains to display
    m_stainToDisplayOptions.push_back("Stain 1");
    m_stainToDisplayOptions.push_back("Stain 2");
    m_stainToDisplayOptions.push_back("Stain 3");

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
        "Choose the number of stains in the image", 0, m_numComponentsOptions, true);

    //Names of stains and ROIs associated with them
    m_nameOfStainOne = createTextFieldParameter(*this, "Name of Stain 1",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainOne = createRegionListParameter(*this, "Stain 1 Regions", "List of Regions of Interest for Stain 1", true);

    m_nameOfStainTwo = createTextFieldParameter(*this, "Name of Stain 2",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainTwo = createRegionListParameter(*this, "Stain 2 Regions", "List of Regions of Interest for Stain 2", true);

    m_nameOfStainThree = createTextFieldParameter(*this, "Name of Stain 3",
        "Enter the name of a stain in the image", "", true);
    m_regionListStainThree = createRegionListParameter(*this, "Stain 3 Regions", "List of Regions of Interest for Stain 3", true);

    m_stainSeparationAlgorithm = createOptionParameter(*this, "Stain Separation Algorithm", 
        "Select the stain separation algorithm to use to separate the stain components", 0, 
        m_separationAlgorithmOptions, false);

    //List of options of the stains currently defined, to show in preview
    m_stainToDisplay = createOptionParameter(*this, "Show Separated Stain", 
        "Choose which of the defined stains to preview in the display area", 0, m_stainToDisplayOptions, false);
 
    //Allow the user to create visible output, without saving the stain vector profile to a file
    //m_showPreviewOnly = createMYBoolParameter(*this, "Preview Only",
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
    
    m_saveFileAs.setVisible(false);
    m_stainToDisplayOptions[0] = "Happy Dance";
}//end run


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
    //ofn.lpstrFilter = "*.jpg;*.jpeg;*.tif;*.png;*.bmp";
    ofn.lpstrFile = (LPSTR)szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    //ofn.lpstrDefExt = (LPSTR)L"tif";
    ofn.lpstrInitialDir = (LPSTR)m_path_to_root.c_str();
    GetOpenFileName(&ofn);

    return ofn.lpstrFile;
}//end openFile



} // namespace algorithm
} // namespace sedeen


