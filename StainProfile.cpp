#include "StainProfile.h"
/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

#include "StainProfile.h"

StainProfile::StainProfile() :
    m_nameOfStainProfile(""),
    m_numberOfStainComponents(0),
    m_nameOfStainOne(""),
    m_nameOfStainTwo(""),
    m_nameOfStainThree(""),
    m_stainSeparationAlgorithm(""),
    m_stainOneRGB({ 0,0,0 }),
    m_stainTwoRGB({ 0,0,0 }),
    m_stainThreeRGB({ 0,0,0 })
{
    //Keep a list of possible stain separation algorithm names here
    m_stainSeparationAlgorithmOptions.push_back("Ruifrok Colour Deconvolution");
    //Just one for now
}

StainProfile::~StainProfile() {
}

bool StainProfile::SetNameOfStainProfile(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_nameOfStainProfile = name;
    return true;
}

std::string StainProfile::GetNameOfStainProfile() {
    return m_nameOfStainProfile;
}

bool StainProfile::SetNumberOfStainComponents(int components) {
    //Check if number is 0 or greater.
    //If not, do not assign value and return false
    if (components >= 0) {
        m_numberOfStainComponents = components;
        return true;
    }//else
    return false;
}

int StainProfile::GetNumberOfStainComponents() {
    return m_numberOfStainComponents;
}

bool StainProfile::SetNameOfStainOne(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_nameOfStainOne = name;
    return true;
}

std::string StainProfile::GetNameOfStainOne() {
    return m_nameOfStainOne;
}

bool StainProfile::SetNameOfStainTwo(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_nameOfStainTwo = name;
    return true;
}

std::string StainProfile::GetNameOfStainTwo() {
    return m_nameOfStainTwo;
}

bool StainProfile::SetNameOfStainThree(std::string name) {
    //Direct assignment. Add checks if necessary.
    m_nameOfStainThree = name;
    return true;
}

std::string StainProfile::GetNameOfStainThree() {
    return m_nameOfStainThree;
}

bool StainProfile::SetNameOfStainSeparationAlgorithm(std::string name) {
    //Check if the stain separation algorithm name given is in the valid list
    //If not, do not assign value and return false
    if (std::find(m_stainSeparationAlgorithmOptions.begin(), m_stainSeparationAlgorithmOptions.end(), name) 
        != m_stainSeparationAlgorithmOptions.end()) {
        //Found. Assign name to m_stainSeparationAlgorithm
        m_stainSeparationAlgorithm = name;
        return true;
    }//else
    return false;
}

std::string StainProfile::GetNameOfStainSeparationAlgorithm() {
    return m_stainSeparationAlgorithm;
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
        return SetStainOneRGB(arrayRGB);
    }
    return false;
}//end SetStainOneRGB(C array)

bool StainProfile::SetStainOneRGB(std::array<double, 3> rgb) {
    //Normalize the values in the input array
    std::array<double, 3> normRGB = this->NormalizeArray(rgb);
    //Perform any required checks of the data
    m_stainOneRGB = normRGB;
    return true;
}//end SetStainOneRGB(std::array)

std::array<double, 3> StainProfile::GetStainOneRGB() {
    return m_stainOneRGB;
}

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
    //Perform any required checks of the data
    m_stainTwoRGB = normRGB;
    return true;
}//end SetStainTwoRGB(std::array)

std::array<double, 3> StainProfile::GetStainTwoRGB() {
    return m_stainTwoRGB;
}

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
    //Perform any required checks of the data
    m_stainThreeRGB = normRGB;
    return true;
}//end SetStainThreeRGB(std::array)

std::array<double, 3> StainProfile::GetStainThreeRGB() {
    return m_stainThreeRGB;
}



bool StainProfile::checkFile(std::string fileString, std::string op) {
    return true;
}







bool StainProfile::writeStainProfileToFile(std::string fileString) {

    //TEMP version of this, just to get something to file.









    return true;
}




















bool StainProfile::readStainProfileFromFile(std::string fileString) {
    return true;
}



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
        //Iterate through the out array
        for (auto p = out.begin(); p != out.end(); ++p) {
            *p = *p / norm;
        }
        return out;
    }
}//end NormalizeArray

template<typename Iter_T, class Ty>
Ty StainProfile::Norm(Iter_T first, Iter_T last) {
    return sqrt(std::inner_product(first, last, first, Ty{})); //Use C++11 zero initialization of Ty
}//end Norm
