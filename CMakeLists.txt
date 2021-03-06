PROJECT( CreateStainVectorProfile )
CMAKE_MINIMUM_REQUIRED( VERSION 3.13 )
# Enable C++17 features
SET(CMAKE_CXX_STANDARD 17)
INCLUDE(FetchContent)

#Note which version of Sedeen Viewer this plugin was last compiled and tested with
SET(SEDEENSDK_VERSION "5.4.4.20191024" CACHE STRING "Last version of Sedeen Viewer the plugin was compiled and tested with")

# Define project description variables
SET( DISPLAY_NAME_TEXT "Create Stain Vector Profile" CACHE STRING "Name of the plugin as it should be displayed in Sedeen Viewer")
SET( SUPPORT_URL_TEXT "http://pathcore.com/support/plugin/info/${PROJECT_NAME}" CACHE STRING "Location users can find help with the plugin" )
SET( DEVELOPER_TEXT "Sunnybrook Research Institute" CACHE STRING "Name of the author or organization that created the plugin" )

# Load the Sedeen dependencies
SET(PROGRAMFILESX86 "PROGRAMFILES\(X86\)")
FIND_PACKAGE( SEDEENSDK REQUIRED 
  HINTS ../../.. 
        "$ENV{${PROGRAMFILESX86}}/Sedeen Viewer SDK/v5.4.4.20191024/msvc2017"
        "$ENV{PROGRAMFILES}/Sedeen Viewer SDK/v5.4.4.20191024/msvc2017" )
        
# Load the included OpenCV libs
FIND_PACKAGE(SEDEENSDK_OPENCV REQUIRED
  HINTS ../../..
        "$ENV{${PROGRAMFILESX86}}/Sedeen Viewer SDK/v5.4.4.20191024/msvc2017"
        "$ENV{PROGRAMFILES}/Sedeen Viewer SDK/v5.4.4.20191024/msvc2017" )

#Configure MLPACK (requires some Boost libraries)
SET(MLPACK_REQUIRED_BOOST_COMPONENTS "serialization" "program_options")
FOREACH(component IN ITEMS ${MLPACK_REQUIRED_BOOST_COMPONENTS})
  SET(MLPACK_REQUIRED_BOOST_LIBRARIES ${MLPACK_REQUIRED_BOOST_LIBRARIES} "Boost::${component}")
ENDFOREACH(component)

SET(MLPACK_INCLUDE_DIR "MLPACK_INCLUDE_DIR-NOTFOUND" CACHE PATH "Base directory of the MLPACK include files")
SET(MLPACK_LIBRARY_DIR "MLPACK_LIBRARY_DIR-NOTFOUND" CACHE PATH "Directory containing mlpack.lib")
#List all .lib files in MLPACK_LIBRARY_DIR
FILE(GLOB MLPACK_LIBRARIES "${MLPACK_LIBRARY_DIR}/*.lib")

#Configure the Armadillo library (used by MLPACK)
SET(ARMADILLO_INCLUDE_DIR "ARMADILLO_INCLUDE_DIR-NOTFOUND" CACHE PATH "Location of armadillo include file and subdirectory")
SET(ARMADILLO_LIBRARY "ARMADILLO_LIBRARY-NOTFOUND" CACHE FILEPATH "Full path to the library file armadillo.lib")
GET_FILENAME_COMPONENT(ARMADILLO_LIBRARY_DIR ARMADILLO_LIBRARY DIRECTORY)

#Configure the LAPACK linear algebra library (used by MLPACK)
SET(LAPACK_STATIC_LIBRARY "LAPACK_STATIC_LIBRARY-NOTFOUND" CACHE FILEPATH "Full path to the static library file liblapack.lib")
GET_FILENAME_COMPONENT(LAPACK_STATIC_LIBRARY_DIR LAPACK_STATIC_LIBRARY DIRECTORY)
SET(LAPACK_DYNAMIC_LIBRARY "LAPACK_DYNAMIC_LIBRARY-NOTFOUND" CACHE FILEPATH "Full path to the dynamic library file liblapack.dll")
GET_FILENAME_COMPONENT(LAPACK_DYNAMIC_LIBRARY_DIR LAPACK_DYNAMIC_LIBRARY DIRECTORY)

#Configure the LAPACK linear algebra library (used by MLPACK)
SET(BLAS_STATIC_LIBRARY "BLAS_STATIC_LIBRARY-NOTFOUND" CACHE FILEPATH "Full path to the static library file libblas.lib")
GET_FILENAME_COMPONENT(BLAS_STATIC_LIBRARY_DIR BLAS_STATIC_LIBRARY DIRECTORY)
SET(BLAS_DYNAMIC_LIBRARY "BLAS_DYNAMIC_LIBRARY-NOTFOUND" CACHE FILEPATH "Full path to the dynamic library file libblas.dll")
GET_FILENAME_COMPONENT(BLAS_DYNAMIC_LIBRARY_DIR BLAS_DYNAMIC_LIBRARY DIRECTORY)

IF(NOT BOOST_ROOT)
  SET(BOOST_ROOT "BOOST_ROOT-NOTFOUND" CACHE PATH "Preferred installation prefix of the Boost C++ library")
ENDIF()
IF(NOT BOOST_VERSION)
  SET(BOOST_VERSION "BOOST_VERSION-NOTFOUND" CACHE STRING "Boost library version number")
ENDIF()
FIND_PACKAGE(Boost ${BOOST_VERSION} REQUIRED COMPONENTS ${MLPACK_REQUIRED_BOOST_COMPONENTS})

# Fetch TinyXML2 files. Do not build as a subproject
FetchContent_Declare(
  TinyXML2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG 61a4c7d507322c9f494f5880d4c94b60e4ce9590
)
# Check if the tinyxml2 files have already been populated
FetchContent_GetProperties(TinyXML2)
STRING(TOLOWER "TinyXML2" TinyXML2Name)
IF(NOT ${TinyXML2Name}_POPULATED)
  #Fetch TinyXML2 using the details from FetchContent_Declare 	
  FetchContent_Populate(TinyXML2)
ENDIF()

#Fetch the OpticalDensityThreshold respository. Do not build as a subproject
FetchContent_Declare(
  OpticalDensityThreshold
  GIT_REPOSITORY https://github.com/sedeen-piip-plugins/OpticalDensityThreshold.git
  GIT_TAG 4c13adceeccbc51384d198541759be3b48280ef3
)
FetchContent_GetProperties(OpticalDensityThreshold)
STRING(TOLOWER "OpticalDensityThreshold" OpticalDensityThresholdName)
IF(NOT ${OpticalDensityThresholdName}_POPULATED)
  FetchContent_Populate(OpticalDensityThreshold)
ENDIF()
SET(OPTICAL_DENSITY_THRESHOLD_DIR ${${OpticalDensityThresholdName}_SOURCE_DIR})

# Fetch the StainAnalysis-plugin repository. Do not build as a subproject
FetchContent_Declare(
  StainAnalysisplugin
  GIT_REPOSITORY https://github.com/sedeen-piip-plugins/StainAnalysis-plugin.git
  GIT_TAG 4be9e69d71869026d824d7959ba30fa20e88c12e
)
FetchContent_GetProperties(StainAnalysisplugin)
STRING(TOLOWER "StainAnalysisplugin" StainAnalysispluginName)
IF(NOT ${StainAnalysispluginName}_POPULATED)
  FetchContent_Populate(StainAnalysisplugin)
ENDIF()
SET(STAIN_ANALYSIS_DIR ${${StainAnalysispluginName}_SOURCE_DIR})

INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} 
                     ${SEDEENSDK_INCLUDE_DIR} 
                     ${SEDEENSDK_OPENCV_INCLUDE_DIR}
                     ${BOOST_ROOT} 
                     ${${TinyXML2Name}_SOURCE_DIR}
                     ${OPTICAL_DENSITY_THRESHOLD_DIR}
                     ${STAIN_ANALYSIS_DIR}
                     ${MLPACK_INCLUDE_DIR}
                     ${ARMADILLO_INCLUDE_DIR}
                     )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} 
                  ${SEDEENSDK_LIBRARY_DIR} 
                  ${SEDEENSDK_OPENCV_LIBRARY_DIR}
                  ${MLPACK_LIBRARY_DIR}
                  ${ARMADILLO_LIBRARY_DIR}
                  ${LAPACK_STATIC_LIBRARY_DIR}
                  ${BLAS_STATIC_LIBRARY_DIR}
                  )

ADD_LIBRARY( ${PROJECT_NAME} MODULE 
             ${PROJECT_NAME}.cpp 
             ${PROJECT_NAME}.h 
             ${${TinyXML2Name}_SOURCE_DIR}/tinyxml2.h 
             ${${TinyXML2Name}_SOURCE_DIR}/tinyxml2.cpp
             ${OPTICAL_DENSITY_THRESHOLD_DIR}/ODConversion.h
             ${OPTICAL_DENSITY_THRESHOLD_DIR}/ODThresholdKernel.h
             ${OPTICAL_DENSITY_THRESHOLD_DIR}/ODThresholdKernel.cpp
             ${STAIN_ANALYSIS_DIR}/StainProfile.h 
             ${STAIN_ANALYSIS_DIR}/StainProfile.cpp 
             ${STAIN_ANALYSIS_DIR}/ColorDeconvolutionKernel.h 
             ${STAIN_ANALYSIS_DIR}/ColorDeconvolutionKernel.cpp 
             ${STAIN_ANALYSIS_DIR}/StainVectorMath.h 
             ${STAIN_ANALYSIS_DIR}/StainVectorMath.cpp
             RandomWSISampler.h RandomWSISampler.cpp
             StainVectorBase.h StainVectorBase.cpp
             StainVectorOpenCV.h StainVectorOpenCV.cpp
             StainVectorMLPACK.h StainVectorMLPACK.cpp
             StainVectorPixelROI.h StainVectorPixelROI.cpp
             StainVectorMacenko.h StainVectorMacenko.cpp 
             StainVectorNMF.h StainVectorNMF.cpp 
             BasisTransform.h BasisTransform.cpp
             AngleHistogram.h AngleHistogram.cpp
             MacenkoHistogram.h MacenkoHistogram.cpp
             )

# Link the library against the Sedeen SDK libraries
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} 
                       ${SEDEENSDK_LIBRARIES} 
                       ${SEDEENSDK_OPENCV_LIBRARIES} 
                       ${MLPACK_LIBRARIES}
                       ${MLPACK_REQUIRED_BOOST_LIBRARIES}
                       ${ARMADILLO_LIBRARY}
                       ${LAPACK_STATIC_LIBRARY}
                       ${BLAS_STATIC_LIBRARY}
                       )

# Create or update the .info file in the build directory
STRING( TIMESTAMP DATE_CREATED_TEXT "%Y-%m-%d" )
CONFIGURE_FILE( "infoTemplate.info.in" "${PROJECT_NAME}.info" )

# Set the relative directory where the plugin should be located
SET( PLUGIN_RELATIVE_DIR "plugins/cpp/piip/${PROJECT_NAME}" )

# Set the install destination directory
IF( NOT PLUGIN_DESTINATION_DIR )
  IF( ${SEDEEN_FOUND} )
	SET( TEMPPLUGINDIR "${PATHCORE_DIR}/${PLUGIN_RELATIVE_DIR}" )
  ELSE()
	SET( TEMPPLUGINDIR "PLUGIN_DESTINATION_DIR-NOTFOUND" )
	MESSAGE( SEND_ERROR "PLUGIN_DESTINATION_DIR not found. Set this to the target installation directory of the plugin within Sedeen Viewer (e.g. $ENV{PROGRAMFILES}/Sedeen Viewer/plugins/cpp/piip/${PROJECT_NAME}).")
  ENDIF()
  SET(PLUGIN_DESTINATION_DIR ${TEMPPLUGINDIR} CACHE PATH "Installation directory for the plugin within Sedeen Viewer")
ENDIF()

# Install the plugin and .info file in the PLUGIN_DESTINATION_DIR directory
IF( ${SEDEEN_FOUND} )
  INSTALL(TARGETS ${PROJECT_NAME} LIBRARY 
     DESTINATION "${PLUGIN_DESTINATION_DIR}")
  INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.info"
     DESTINATION "${PLUGIN_DESTINATION_DIR}" )
  INSTALL(DIRECTORY "${SEDEENSDK_DIR}/bin/" 
     DESTINATION "${PLUGIN_DESTINATION_DIR}" 
     FILES_MATCHING PATTERN "opencv*0.dll" )
  INSTALL(FILES "${LAPACK_DYNAMIC_LIBRARY}" 
     DESTINATION "${PLUGIN_DESTINATION_DIR}" )
  INSTALL(FILES "${BLAS_DYNAMIC_LIBRARY}" 
     DESTINATION "${PLUGIN_DESTINATION_DIR}" )
ENDIF()

#For debugging: shows all variables and their values
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
