cmake_minimum_required(VERSION 2.8)
 
project(MrVizify)

## Addin version number
set(MrVizify_VERSION_MAJOR 1)
set(MrVizify_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/MrVizifyConfig.h.in"
  "${PROJECT_BINARY_DIR}/MrVizifyConfig.h"
  )

 
find_package(ITK REQUIRED)
message("This variale is "  ${ITK_USE_FILE})
include(${ITK_USE_FILE})
message("ITK Includes " ${ITK_INCLUDE_DIRS})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
  set(Glue ItkVtkGlue)
endif()

set (QT_LIB_VERSION "5" CACHE PATH "Specify Qt version")

if (QT_LIB_VERSION VERSION_GREATER "4")
  find_package(Qt5 REQUIRED COMPONENTS Core)
  include_directories(${Qt_INCLUDE_DIRS})
  include_directories(${Qt5Core_INCLUDE_DIRS})
else()
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

### Include ITK Wrapped DCMTK DICOM
option (USE_ITKDCMTK "Add ITK Wrapped DCMTK to the project ")
### Include MERGE Dicom Toolkit
option (USE_MERGE "Add Merge Toolkit to the project")

if (USE_ITKDCMTK)
    include_directories(${PROJECT_SOURCE_DIR}/MrVizDCMTKDICOM)
    add_subdirectory(MrVizDCMTKDICOM)
    set(EXTRA_LIBS ${EXTRA_LIBS} MrVizDCMTKDICOM)
	set(DICOM_LIB "DCMTK" CACHE STRING "DCMTK Dicom Toolkit")
endif(USE_ITKDCMTK)

if (USE_MERGE)
    include_directories(${PROJECT_SOURCE_DIR}/MrVizMERGEDICOM)
    add_subdirectory(MrVizMERGEDICOM)
    set(EXTRA_LIBS ${EXTRA_LIBS} MrVizMERGE)
	set(DICOM_LIB "MERGE" CACHE STRING "Merge Dicom Toolkit")
endif(USE_MERGE)

### Include the MrImageProcessing Library
option (USE_CIMGPROC "Add the Image Processing Routines to the project")

if (USE_CIMGPROC)
    include_directories(${PROJECT_SOURCE_DIR}/MRImageProcessing)
    add_subdirectory(MRImageProcessing)
    set(EXTRA_LIBS ${EXTRA_LIBS} MrImgProc)
endif(USE_CIMGPROC)

### Include the MrImageConvertor Library
option (USE_CIMGCONV "Add the Image Convertor Routines to the project")

if (USE_CIMGCONV)
    include_directories(${PROJECT_SOURCE_DIR}/MRImageConvertor)
    add_subdirectory(MRImageConvertor)
    set(EXTRA_LIBS ${EXTRA_LIBS} MrImgConv)
endif(USE_CIMGCONV)

### Include the MrImageConvertor Library
option (USE_CIMGSTAT "Add the Image Statistics Routines to the project")

if (USE_CIMGSTAT)
    include_directories(${PROJECT_SOURCE_DIR}/MRImageStatistics)
    add_subdirectory(MRImageStatistics)
    set(EXTRA_LIBS ${EXTRA_LIBS} MrImgStats)
endif(USE_CIMGSTAT)

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cpp)
file(GLOB CC_FILES *.cc)

if (QT_LIB_VERSION VERSION_GREATER "4")
  QT5_WRAP_UI(UISrcs ${UI_FILES})
  QT5_WRAP_CPP(MOCSrcs ${QT_WRAP})
else()
  QT4_WRAP_UI(UISrcs ${UI_FILES})
  QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
endif()

add_executable(MrVizify MACOSX_BUNDLE ${CXX_FILES} ${CC_FILES} ${UISrcs} ${MOCSrcs} ${QT_WRAP})


 
if( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(MrVizify ITKReview ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${EXTRA_LIBS} )
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(MrVizify ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${EXTRA_LIBS} )
endif( "${ITK_VERSION_MAJOR}" LESS 4 )

