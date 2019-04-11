/*=========================================================================
 *
 *  Copyright (c) 2019 Sunnybrook Research Institute
 *
 *  License terms pending.
 *
 *=========================================================================*/

#ifndef SEDEEN_SRC_PLUGINS_STAINANALYSIS_STAINPROFILE_H
#define SEDEEN_SRC_PLUGINS_STAINANALYSIS_STAINPROFILE_H

#include <string>
#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <numeric>

///Class to read and write stain vector profile data to XML files. 
///Avoids invoking the Sedeen SDK. Uses Poco for XML file interactions
class StainProfile
{
public:
    ///No-parameter constructor
    StainProfile();
    ///virtual destructor
    virtual ~StainProfile();

    ///Get/Set the name of the stain profile
    bool SetNameOfStainProfile(std::string);
    ///Get/Set the name of the stain profile
    std::string GetNameOfStainProfile();

    ///Get/Set the number of stain components in the profile
    bool SetNumberOfStainComponents(int);
    ///Get/Set the number of stain components in the profile
    int GetNumberOfStainComponents();

    ///Get/Set the name of stain one
    bool SetNameOfStainOne(std::string);
    ///Get/Set the name of stain one
    std::string GetNameOfStainOne();

    ///Get/Set the name of stain two
    bool SetNameOfStainTwo(std::string);
    ///Get/Set the name of stain two
    std::string GetNameOfStainTwo();

    ///Get/Set the name of stain three
    bool SetNameOfStainThree(std::string);
    ///Get/Set the name of stain three
    std::string GetNameOfStainThree();

    ///Get/Set the name of the stain separation algorithm currently selected
    bool SetNameOfStainSeparationAlgorithm(std::string);
    ///Get/Set the name of the stain separation algorithm currently selected
    std::string GetNameOfStainSeparationAlgorithm();

    ///Set the RGB values for stain one as three doubles
    bool SetStainOneRGB(double, double, double);
    ///Overload: Set the RGB values for stain one as a three-element C-style array
    bool SetStainOneRGB(double []);
    ///Overload: Set the RGB values for stain one as a C++11 std::array of three doubles
    bool SetStainOneRGB(std::array<double, 3>);
    ///Get the RGB values for stain one as a C++11 std::array of three doubles
    std::array<double, 3> GetStainOneRGB();

    ///Set the RGB values for stain two as three doubles
    bool SetStainTwoRGB(double, double, double);
    ///Overload: Set the RGB values for stain two as a three-element C-style array
    bool SetStainTwoRGB(double []);
    ///Overload: Set the RGB values for stain two as a C++11 std::array of three doubles
    bool SetStainTwoRGB(std::array<double, 3>);
    ///Get the RGB values for stain two as a C++11 std::array of three doubles
    std::array<double, 3> GetStainTwoRGB();

    ///Set the RGB values for stain three as three doubles
    bool SetStainThreeRGB(double, double, double);
    ///Overload: Set the RGB values for stain three as a three-element C-style array
    bool SetStainThreeRGB(double []);
    ///Overload: Set the RGB values for stain three as a C++11 std::array of three doubles
    bool SetStainThreeRGB(std::array<double, 3>);
    ///Get the RGB values for stain three as a C++11 std::array of three doubles
    std::array<double, 3> GetStainThreeRGB();

    //Check if the file exists, and accessible for reading or writing, depending on the second argument
    bool checkFile(std::string, std::string);

    ///If file is able to be opened, write the current stain profile to file as XML
    bool writeStainProfileToFile(std::string);
    ///If file is able to be opened, read from an XML file and fill variables in this class
    bool readStainProfileFromFile(std::string);

    ///Request the list of possible stain separation algorithm names from the class
    std::vector<std::string> GetStainSeparationAlgorithmOptions();

    ///Request an element of the vector of stain separation algorithms. Returns "" on error.
    std::string GetStainSeparationAlgorithmName(int);

private:
    ///Keep local data private
    std::string m_nameOfStainProfile;
    int m_numberOfStainComponents;
    std::string m_nameOfStainOne;
    std::string m_nameOfStainTwo;
    std::string m_nameOfStainThree;
    std::string m_stainSeparationAlgorithm;

    //Assign the RGB values after processing
    std::array<double, 3> m_stainOneRGB;
    std::array<double, 3> m_stainTwoRGB;
    std::array<double, 3> m_stainThreeRGB;

    ///Store the list of possible stain separation algorithm names here
    std::vector<std::string> m_stainSeparationAlgorithmOptions;


private:
    ///Return an array of values of type Ty with size N normalized to unit length. Returns input array if norm is 0.
    template<class Ty, std::size_t N> std::array<Ty, N> NormalizeArray(std::array<Ty, N>);
    ///Calculate the norm of all the elements in a container, where each element is of type Ty
    template<typename Iter_T, class Ty> Ty Norm(Iter_T first, Iter_T last);

};

#endif