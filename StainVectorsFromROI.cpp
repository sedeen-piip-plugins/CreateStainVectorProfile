/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

// StainVectorsFromROI.cpp : Defines the exported functions for the DLL application.
//
// Primary header
#include "StainVectorsFromROI.h"

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

// Poco header needed for the macros below
#include <Poco/ClassLibrary.h>

// Declare that this object has AlgorithmBase subclasses
//  and declare each of those sub-classes
POCO_BEGIN_MANIFEST(sedeen::algorithm::AlgorithmBase)
POCO_EXPORT_CLASS(sedeen::algorithm::StainVectorsFromROI)
POCO_END_MANIFEST

namespace sedeen {
namespace algorithm {

///Constructor
StainVectorsFromROI::StainVectorsFromROI()
    : m_nameOfStainProfile()
{

}//end constructor

///Destructor
StainVectorsFromROI::~StainVectorsFromROI() {
}//end destructor


///Initial setup of the plugin
void StainVectorsFromROI::init(const image::ImageHandle& image) {
    if (isNull(image)) return;

    //Bind algorithm members to UI and initialize properties

    m_nameOfStainProfile = createTextFieldParameter(*this, "Name of stain profile", "Enter a name for your new stain vector profile", "", false);




}//end init



///Method called when the 'Run' button is clicked
void StainVectorsFromROI::run() {
}//end run


std::string StainVectorsFromROI::openFile(std::string path)
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

