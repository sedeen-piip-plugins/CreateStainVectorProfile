/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

#include "StainProfile.h"

#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <numeric>

#include <tinyxml2.h>

StainProfile::StainProfile() {
    //Keep a list of possible stain separation algorithm names here
    m_stainSeparationAlgorithmOptions.push_back("Ruifrok Colour Deconvolution");
    //Just one for now

    //Build the XML document structure
    BuildXMLDocument();
}//end constructor

StainProfile::~StainProfile() {
    //Smartpointer used for m_xmlDoc, 
    //and an XMLDocument handles memory management for all its child objects.
    //No explicit delete required. reset() is an option, though.
}

bool StainProfile::SetNameOfStainProfile(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_rootElement->SetAttribute(nameOfStainProfileAttribute(), name.c_str());
    return true;
}

std::string StainProfile::GetNameOfStainProfile() {
    const char* name = m_rootElement->Attribute(nameOfStainProfileAttribute());
    std::string result(name);
    return result;
}

bool StainProfile::SetNumberOfStainComponents(int components) {
    //Check if number is 0 or greater.
    //If not, do not assign value and return false
    if (components >= 0) {
        m_componentsElement->SetAttribute(numberOfStainsAttribute(), components);
        return true;
    }//else
    return false;
}

int StainProfile::GetNumberOfStainComponents() {
    int components = m_componentsElement->IntAttribute(numberOfStainsAttribute());
    return components;
}

bool StainProfile::SetNameOfStainOne(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_stainOneElement->SetAttribute(nameOfStainAttribute(), name.c_str());
    return true;
}

std::string StainProfile::GetNameOfStainOne() {
    const char* name = m_stainOneElement->Attribute(nameOfStainAttribute());
    std::string result(name);
    return result;
}

bool StainProfile::SetNameOfStainTwo(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_stainTwoElement->SetAttribute(nameOfStainAttribute(), name.c_str());
    return true;
}

std::string StainProfile::GetNameOfStainTwo() {
    const char* name = m_stainTwoElement->Attribute(nameOfStainAttribute());
    std::string result(name);
    return result;
}

bool StainProfile::SetNameOfStainThree(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_stainThreeElement->SetAttribute(nameOfStainAttribute(), name.c_str());
    return true;
}

std::string StainProfile::GetNameOfStainThree() {
    const char* name = m_stainThreeElement->Attribute(nameOfStainAttribute());
    std::string result(name);
    return result;
}

bool StainProfile::SetNameOfStainSeparationAlgorithm(std::string name) {
    //Check if the stain separation algorithm name given is in the valid list
    //If not, do not assign value and return false
    if (std::find(m_stainSeparationAlgorithmOptions.begin(), m_stainSeparationAlgorithmOptions.end(), name) 
        != m_stainSeparationAlgorithmOptions.end()) {
        //Found. Assign name 
        m_algorithmElement->SetAttribute(algorithmNameAttribute(), name.c_str());
        return true;
    }//else
    return false;
}

std::string StainProfile::GetNameOfStainSeparationAlgorithm() {
    return m_algorithmElement->Attribute(algorithmNameAttribute());
}

bool StainProfile::SetStainOneRGB(double r, double g, double b) {
    //Place data into a std::array, pass to other overload of this method
    std::array<double, 3> rgb = { r,g,b };
    return SetStainOneRGB(rgb);
}

bool StainProfile::SetStainOneRGB(double rgb[])
{
    //Check length of rgb first
    //This is a C-style array, so get the size the old way
    int len = sizeof(rgb) / sizeof(*rgb); //(total size divided by size of first element)
    if (len != 3) {
        return false;
    }
    else {
        //Place data into a std::array, pass to other overload of this method
        std::array<double, 3> arrayRGB;
        for (int i = 0; i < 3; ++i) {
            try {
                arrayRGB.at(i) = rgb[i];
            }
            catch (const std::out_of_range& rangeerr) {
                rangeerr.what();
                //The index is out of range. Return ""
                return false;
            }
        }
        return SetStainOneRGB(arrayRGB);
    }
    return false;
}//end SetStainOneRGB(C array)

bool StainProfile::SetStainOneRGB(std::array<double, 3> rgb) {
    //Normalize the values in the input array
    std::array<double, 3> normRGB = this->NormalizeArray(rgb);
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainOneElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return false; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            sVals->SetText(normRGB[0]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            sVals->SetText(normRGB[1]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            sVals->SetText(normRGB[2]);
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    return true;
}//end SetStainOneRGB(std::array)

std::array<double, 3> StainProfile::GetStainOneRGB() {
    //Create a zero array to return
    std::array<double, 3> out = { 0.0,0.0,0.0 };
    double r, g, b; //temp values
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainOneElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return out; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            tinyxml2::XMLError rResult = sVals->QueryDoubleText(&r);
            if (rResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            tinyxml2::XMLError gResult = sVals->QueryDoubleText(&g);
            if (gResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            tinyxml2::XMLError bResult = sVals->QueryDoubleText(&b);
            if (bResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    out[0] = r;
    out[1] = g;
    out[2] = b;
    return out;
}//end GetStainOneRGB

bool StainProfile::SetStainTwoRGB(double r, double g, double b) {
    //Place data into a std::array, pass to other overload of this method
    std::array<double, 3> rgb = { r,g,b };
    return SetStainTwoRGB(rgb);
}

bool StainProfile::SetStainTwoRGB(double rgb[]) {
    //Check length of rgb first
    //This is a C-style array, so get the size the old way
    int len = sizeof(rgb) / sizeof(*rgb);
    if (len != 3) {
        return false;
    }
    else {
        //Place data into a std::array, pass to other overload of this method
        std::array<double, 3> arrayRGB;
        for (int i = 0; i < 3; ++i) {
            try {
                arrayRGB.at(i) = rgb[i];
            }
            catch (const std::out_of_range& rangeerr) {
                rangeerr.what();
                //The index is out of range. Return ""
                return false;
            }
        }
        return SetStainTwoRGB(arrayRGB);
    }
    return false;
}//end SetStainTwoRGB(C array)

bool StainProfile::SetStainTwoRGB(std::array<double, 3> rgb) {
    //Normalize the values in the input array
    std::array<double, 3> normRGB = this->NormalizeArray(rgb);
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainTwoElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return false; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            sVals->SetText(normRGB[0]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            sVals->SetText(normRGB[1]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            sVals->SetText(normRGB[2]);
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    return true;
}//end SetStainTwoRGB(std::array)

std::array<double, 3> StainProfile::GetStainTwoRGB() {
    //Create a zero array to return
    std::array<double, 3> out = { 0.0,0.0,0.0 };
    double r, g, b; //temp values
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainTwoElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return out; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            tinyxml2::XMLError rResult = sVals->QueryDoubleText(&r);
            if (rResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            tinyxml2::XMLError gResult = sVals->QueryDoubleText(&g);
            if (gResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            tinyxml2::XMLError bResult = sVals->QueryDoubleText(&b);
            if (bResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    out[0] = r;
    out[1] = g;
    out[2] = b;
    return out;
}//end GetStainTwoRGB

bool StainProfile::SetStainThreeRGB(double r, double g, double b) {
    //Place data into a std::array, pass to other overload of this method
    std::array<double, 3> rgb = { r,g,b };
    return SetStainThreeRGB(rgb);
}

bool StainProfile::SetStainThreeRGB(double rgb[])
{
    //Check length of rgb first
    //This is a C-style array, so get the size the old way
    int len = sizeof(rgb) / sizeof(*rgb);
    if (len != 3) {
        return false;
    }
    else {
        //Place data into a std::array, pass to other overload of this method
        std::array<double, 3> arrayRGB;
        for (int i = 0; i < 3; ++i) {
            try {
                arrayRGB.at(i) = rgb[i];
            }
            catch (const std::out_of_range& rangeerr) {
                rangeerr.what();
                //The index is out of range. Return ""
                return false;
            }
        }
        return SetStainThreeRGB(arrayRGB);
    }
    return false;
}//end SetStainThreeRGB(C array)

bool StainProfile::SetStainThreeRGB(std::array<double, 3> rgb) {
    //Normalize the values in the input array
    std::array<double, 3> normRGB = this->NormalizeArray(rgb);
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainThreeElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return false; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            sVals->SetText(normRGB[0]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            sVals->SetText(normRGB[1]);
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            sVals->SetText(normRGB[2]);
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    return true;
}//end SetStainThreeRGB(std::array)

std::array<double, 3> StainProfile::GetStainThreeRGB() {
    //Create a zero array to return
    std::array<double, 3> out = { 0.0,0.0,0.0 };
    double r, g, b; //temp values
    //Get the first stain value element, or return false if not found
    tinyxml2::XMLElement* sVals = m_stainThreeElement->FirstChildElement(stainValueTag());
    if (sVals == nullptr) { return out; }
    while (sVals != nullptr) {
        if (sVals->Attribute(valueTypeAttribute(), "r")) {
            tinyxml2::XMLError rResult = sVals->QueryDoubleText(&r);
            if (rResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "g")) {
            tinyxml2::XMLError gResult = sVals->QueryDoubleText(&g);
            if (gResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        else if (sVals->Attribute(valueTypeAttribute(), "b")) {
            tinyxml2::XMLError bResult = sVals->QueryDoubleText(&b);
            if (bResult != tinyxml2::XML_SUCCESS) { return out; }
        }
        sVals = sVals->NextSiblingElement(stainValueTag());
    }
    out[0] = r;
    out[1] = g;
    out[2] = b;
    return out;
}//end GetStainThreeRGB


bool StainProfile::checkFile(std::string fileString, std::string op) {
    //If read, check file exists and is readable
    //If write, check file is not read-only if it exists
    return true;
}



///Public write method, calls private write method
bool StainProfile::writeStainProfile(std::string fileString) {
    bool checkResult = this->checkFile(fileString, "w");
    if (!checkResult) {
        return false;
    }
    else {
        tinyxml2::XMLError eResult = this->writeStainProfileToXML("D:/mschumaker/projects/Sedeen/testData/output/tempout.txt"); // fileString);
        if (eResult == tinyxml2::XML_SUCCESS) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}//end writeStainProfile

///Public read method, calls private write method
bool StainProfile::readStainProfile(std::string fileString) {
    bool checkResult = this->checkFile(fileString, "r");
    if (!checkResult) {
        return false;
    }
    else {
        tinyxml2::XMLError eResult = this->readStainProfileFromXML("D:/mschumaker/projects/Sedeen/testData/output/tempout.txt"); // fileString);
        if (eResult == tinyxml2::XML_SUCCESS) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}//end readStainProfile








///Private write method that deals with TinyXML2
tinyxml2::XMLError StainProfile::writeStainProfileToXML(std::string fileString) {
    //TODO: Check the fileString!


    //Write it!
    tinyxml2::XMLError eResult = this->GetXMLDoc()->SaveFile(fileString.c_str());
    XMLCheckResult(eResult);
    //else
    return tinyxml2::XML_SUCCESS;
}//end writeStainProfileToXML

tinyxml2::XMLError StainProfile::readStainProfileFromXML(std::string fileString) {
    //TODO: Check the fileString!


    //Read it!
    tinyxml2::XMLError eResult = this->GetXMLDoc()->LoadFile(fileString.c_str());
    XMLCheckResult(eResult);
    //else
    return tinyxml2::XML_SUCCESS;
}//end readStainProfileFromXML



std::vector<std::string> StainProfile::GetStainSeparationAlgorithmOptions() {
    return m_stainSeparationAlgorithmOptions;
}

std::string StainProfile::GetStainSeparationAlgorithmName(int index) {
    //Check that the given index value is valid
    //Use the vector::at operator to do bounds checking
    std::string name;
    try {
        name = m_stainSeparationAlgorithmOptions.at(index);
    }
    catch (const std::out_of_range& rangeerr) {
        rangeerr.what();
        //The index is out of range. Return ""
        return "";
    }
    //name found. Return it.
    return name;
}//end GetStainSeparationAlgorithmOptions

bool StainProfile::BuildXMLDocument() {
    //Instantiate the XMLDocument as shared_ptr
    //This isn't really a necessary part of the document object model
    m_xmlDoc = std::make_shared<tinyxml2::XMLDocument>();
    //Create a root element and assign it as a child of the XML document
    m_rootElement = m_xmlDoc->NewElement(rootTag());
    m_xmlDoc->InsertFirstChild(m_rootElement);

    //Build the next levels of the document structure
    m_componentsElement = m_xmlDoc->NewElement(componentsTag());
    m_componentsElement->SetAttribute(numberOfStainsAttribute(), 0); //default value 0
    m_rootElement->InsertEndChild(m_componentsElement);

    //Build stain one
    m_stainOneElement = m_xmlDoc->NewElement(stainTag());
    m_stainOneElement->SetAttribute(indexOfStainAttribute(), 1);
    m_stainOneElement->SetAttribute(nameOfStainAttribute(), "");
    m_componentsElement->InsertEndChild(m_stainOneElement);
    //Add the three stain values to stain element one
    //r
    tinyxml2::XMLElement* rValOne = m_xmlDoc->NewElement(stainValueTag());
    rValOne->SetAttribute(valueTypeAttribute(), "r");
    rValOne->SetText(0);
    m_stainOneElement->InsertEndChild(rValOne);
    //g
    tinyxml2::XMLElement* gValOne = m_xmlDoc->NewElement(stainValueTag());
    gValOne->SetAttribute(valueTypeAttribute(), "g");
    gValOne->SetText(0);
    m_stainOneElement->InsertEndChild(gValOne);
    //b
    tinyxml2::XMLElement* bValOne = m_xmlDoc->NewElement(stainValueTag());
    bValOne->SetAttribute(valueTypeAttribute(), "b");
    bValOne->SetText(0);
    m_stainOneElement->InsertEndChild(bValOne);

    //Build stain two
    m_stainTwoElement = m_xmlDoc->NewElement(stainTag());
    m_stainTwoElement->SetAttribute(indexOfStainAttribute(), 2);
    m_stainTwoElement->SetAttribute(nameOfStainAttribute(), "");
    m_componentsElement->InsertEndChild(m_stainTwoElement);
    //Add the three stain values to stain element two
    //r
    tinyxml2::XMLElement* rValTwo = m_xmlDoc->NewElement(stainValueTag());
    rValTwo->SetAttribute(valueTypeAttribute(), "r");
    rValTwo->SetText(0);
    m_stainTwoElement->InsertEndChild(rValTwo);
    //g
    tinyxml2::XMLElement* gValTwo = m_xmlDoc->NewElement(stainValueTag());
    gValTwo->SetAttribute(valueTypeAttribute(), "g");
    gValTwo->SetText(0);
    m_stainTwoElement->InsertEndChild(gValTwo);
    //b
    tinyxml2::XMLElement* bValTwo = m_xmlDoc->NewElement(stainValueTag());
    bValTwo->SetAttribute(valueTypeAttribute(), "b");
    bValTwo->SetText(0);
    m_stainTwoElement->InsertEndChild(bValTwo);

    //Build stain three
    m_stainThreeElement = m_xmlDoc->NewElement(stainTag());
    m_stainThreeElement->SetAttribute(indexOfStainAttribute(), 3);
    m_stainThreeElement->SetAttribute(nameOfStainAttribute(), "");
    m_componentsElement->InsertEndChild(m_stainThreeElement);
    //Add the three stain values to stain element three
    //r
    tinyxml2::XMLElement* rValThree = m_xmlDoc->NewElement(stainValueTag());
    rValThree->SetAttribute(valueTypeAttribute(), "r");
    rValThree->SetText(0);
    m_stainThreeElement->InsertEndChild(rValThree);
    //g
    tinyxml2::XMLElement* gValThree = m_xmlDoc->NewElement(stainValueTag());
    gValThree->SetAttribute(valueTypeAttribute(), "g");
    gValThree->SetText(0);
    m_stainThreeElement->InsertEndChild(gValThree);
    //b
    tinyxml2::XMLElement* bValThree = m_xmlDoc->NewElement(stainValueTag());
    bValThree->SetAttribute(valueTypeAttribute(), "b");
    bValThree->SetText(0);
    m_stainThreeElement->InsertEndChild(bValThree);

    //algorithm tag, which can contain parameter values needed by the algorithm
    m_algorithmElement = m_xmlDoc->NewElement(algorithmTag());
    m_rootElement->InsertEndChild(m_algorithmElement);
    m_parameterElement = m_xmlDoc->NewElement(parameterTag());
    m_algorithmElement->InsertEndChild(m_parameterElement);

    return true;
}//end BuildXMLDocument


bool StainProfile::CheckXMLDocument(tinyxml2::XMLDocument* docPtr) {
    //Check if the basic structure of this XMLDocument is complete
    if (docPtr->NoChildren()) {
        return false;
    }
    //else

    return true;
}//end CheckXMLDocument



//bool StainProfile::ClearStainXMLElement(tinyxml2::XMLElement* theStain) {
//    //Check if the stain element substructure is complete
//    //Does the XMLElement object exist? Is there an index element?
//    return true;
//}//end ClearStainXMLElement


bool StainProfile::CheckStainXMLElementStructure(tinyxml2::XMLElement* theStain) {
    //Check if there are any sub-elements
    if (theStain->NoChildren()) {
        return false;
    }
    //else

    /*
    stainTag()
        indexofStainAttribute()
        nameOfStainAttribute()

        //Are there three value tags with r,g,b value-type attributes?
        stainValueTag()
        valueTypeAttribute()

        //Create an XMLElement to define a stain, populate it with tags
        theStain = xmlDoc->NewElement(stainTag());
    theStain->SetAttribute(indexofStainAttribute(), index);
    theStain->SetAttribute(nameOfStainAttribute(), name.c_str());
    //Create
    */

    return true;
}//end CheckStainXMLElementStructure


///Populate values in a stain XMLElement
bool StainProfile::FillStainXMLElement(tinyxml2::XMLElement* theStain, int index, std::string name, std::array<double, 3> rgb) {
    //Get a pointer to the member XMLDocument object
    std::shared_ptr<tinyxml2::XMLDocument> xmlDoc = GetXMLDoc();

    return true;
}//end FillStainXMLElement











template<class Ty, std::size_t N>
std::array<Ty, N> StainProfile::NormalizeArray(std::array<Ty, N> arr) {
    std::array<Ty, N> out;
    Ty norm = Norm<std::array<Ty, N>::iterator,Ty>(arr.begin(), arr.end());
    //Check if the norm is zero. Return the input array if so.
    //use C++11 zero initialization of the type Ty
    //Also check if the input container is empty
    if ((norm == Ty{}) || (arr.empty())) {
        return arr;
    }
    else {
        //Copy the input array to the out array
        std::copy(arr.begin(), arr.end(), out.begin());
        //Iterate through the out array, divide values by norm
        for (auto p = out.begin(); p != out.end(); ++p) {
            *p = *p / norm;
        }
        return out;
    }
}//end NormalizeArray

template<typename Iter_T, class Ty>
Ty StainProfile::Norm(Iter_T first, Iter_T last) {
    return sqrt(std::inner_product(first, last, first, Ty{})); //Use C++11 zero initialization of type Ty
}//end Norm


