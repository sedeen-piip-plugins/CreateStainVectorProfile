PROJECT( CreateStainVectorProfile )
CMAKE_MINIMUM_REQUIRED( VERSION 3.8 )
# Enable C++17 features
SET(CMAKE_CXX_STANDARD 17)

# Define project description variables
SET( DISPLAY_NAME_TEXT "Create Stain Vector Profile 5.4.1.20190327" CACHE STRING "Name of the plugin as it should be displayed in Sedeen Viewer")
SET( SUPPORT_URL_TEXT "http://pathcore.com/support/plugin/info/${PROJECT_NAME}" CACHE STRING "Location users can find help with the plugin" )
SET( DEVELOPER_TEXT "Sunnybrook Research Institute" CACHE STRING "Name of the author or organization that created the plugin" )

# Load the Sedeen dependencies
SET(PROGRAMFILESX86 "PROGRAMFILES\(X86\)")
FIND_PACKAGE( SEDEENSDK REQUIRED 
                HINTS ../../.. 
                    "$ENV{${PROGRAMFILESX86}}/Sedeen Viewer SDK/v5.4.1.20190327/msvc2017"
                    "$ENV{PROGRAMFILES}/Sedeen Viewer SDK/v5.4.1.20190327/msvc2017" )

# The ColorDeconvolutionKernel depends on boost.filesystem
IF(NOT BOOST_ROOT)
  SET(BOOST_ROOT "BOOST_ROOT-NOTFOUND" CACHE PATH "Installation prefix of the Boost C++ libraries")
ENDIF()
IF(NOT BOOST_VERSION)
  SET(BOOST_VERSION "BOOST_VERSION-NOTFOUND" CACHE STRING "Boost library version number")
ENDIF()
FIND_PACKAGE(Boost ${BOOST_VERSION} REQUIRED COMPONENTS
                 filesystem )

#Get components of Poco that weren't included in the Sedeen SDK
#IF(NOT Poco_DIR)
#  SET(Poco_DIR "POCO_ROOT-NOTFOUND" CACHE PATH "Installation prefix of the Poco C++ libraries")
#ENDIF()
#FIND_PACKAGE(Poco REQUIRED Foundation XML)

#Inlude TinyXML2 for XML file reading and writing
IF(NOT TinyXML2_INCLUDE_DIR)
  SET(TinyXML2_INCLUDE_DIR "TinyXML2_INCLUDE_DIR-NOTFOUND" CACHE PATH "Location of include files for TinyXML2")
ENDIF()
IF(NOT TinyXML2_LIB_DIR)
  SET(TinyXML2_LIB_DIR "TinyXML2_LIB_DIR-NOTFOUND" CACHE PATH "Location of library files for TinyXML2")
ENDIF()
FIND_PACKAGE(TinyXML2)

# Name of the directory for files from external sources (change this to the binary dir, and set up a git fetch command)
SET(EXTERNAL_SOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/external-sources")
FILE(MAKE_DIRECTORY ${EXTERNAL_SOURCES_DIR})

#This is temporary.
CONFIGURE_FILE(StainProfile.h ${EXTERNAL_SOURCES_DIR}/StainProfile.h COPYONLY)
CONFIGURE_FILE(StainProfile.cpp ${EXTERNAL_SOURCES_DIR}/StainProfile.cpp COPYONLY)
CONFIGURE_FILE(FileDialogHandling.h ${EXTERNAL_SOURCES_DIR}/FileDialogHandling.h COPYONLY)

INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES} ${SEDEENSDK_INCLUDE_DIR} ${BOOST_ROOT} ${TinyXML2_INCLUDE_DIR})
LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${SEDEENSDK_LIBRARY_DIR} "${BOOST_ROOT}/stage/lib" ${TinyXML2_LIB_DIR}) 

# Build the code into a module library --- we'll link the kernel in soon, but we'll leave it in its source repo
#ADD_LIBRARY( ${PROJECT_NAME} MODULE ${PROJECT_NAME}.cpp ${PROJECT_NAME}.h ColorDeconvolutionKernel.cpp ColorDeconvolutionKernel.h)

ADD_LIBRARY( ${PROJECT_NAME} MODULE 
                 ${PROJECT_NAME}.cpp ${PROJECT_NAME}.h 
                 ${EXTERNAL_SOURCES_DIR}/StainProfile.h ${EXTERNAL_SOURCES_DIR}/StainProfile.cpp 
                 ${EXTERNAL_SOURCES_DIR}/FileDialogHandling.h
                 #ColorDeconvolutionKernel.cpp ColorDeconvolutionKernel.h
                 )

# Link the library against the Sedeen SDK libraries
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${SEDEENSDK_LIBRARIES}
                           Boost::filesystem 
                           TinyXML2 )

#Create or update the .info file in the build directory
STRING( TIMESTAMP DATE_CREATED_TEXT "%Y-%m-%d" )
CONFIGURE_FILE( "infoTemplate.info.in" "${PROJECT_NAME}.info" )

# Set the install destination directory
IF( NOT PLUGIN_DESTINATION_DIR )
  IF( ${SEDEEN_FOUND} )
	SET( TEMPPLUGINDIR "${PATHCORE_DIR}/plugins/cpp/piip/${PROJECT_NAME}" )
  ELSE()
	SET( TEMPPLUGINDIR "PLUGIN_DESTINATION_DIR-NOTFOUND" )
	MESSAGE( SEND_ERROR "PLUGIN_DESTINATION_DIR not found. Set this to the target installation directory of the plugin within Sedeen Viewer (e.g. $ENV{PROGRAMFILES}/Sedeen Viewer/plugins/cpp/piip/${PROJECT_NAME}).")
  ENDIF()
  SET(PLUGIN_DESTINATION_DIR ${TEMPPLUGINDIR} CACHE PATH "Installation directory for the plugin within Sedeen Viewer")
ENDIF()

# Install the plugin and .info file in the PLUGIN_DESTINATION_DIR directory
IF( ${SEDEEN_FOUND} )
  install(TARGETS ${PROJECT_NAME}
      LIBRARY DESTINATION "${PLUGIN_DESTINATION_DIR}")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.info"
      DESTINATION "${PLUGIN_DESTINATION_DIR}")
ENDIF()

#Shows all variables and their values
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
