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

set(UI_FILES
    mrvizifymain.ui
    cmrvizthreshold.ui)

set(QT_WRAP 
    mrvizifymain.h
    cmrvizthreshold.h
    cvtkwidget.h)

if (QT_LIB_VERSION VERSION_GREATER "4")
  QT5_WRAP_UI(UISrcs ${UI_FILES})
  QT5_WRAP_CPP(MOCSrcs ${QT_WRAP})
else()
  QT4_WRAP_UI(UISrcs ${UI_FILES})
  QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
endif()

set(HEADER_FILES
    cfileloader.h
    cdicom.h
    citkdicom.h
    citk.h
    cthresholdfactory.h
    cthresholdobject.h
    dataobjects.h
    thresholdobjects.h)

set(SOURCE_FILES
    cfileloader.cpp
    citk.cpp
    citkdicom.cpp
    cthresholdfactory.cpp
    cthresholdobject.cpp
    cvtkwidget.cpp
    mrvizifymain.cpp
    cmrvizthreshold.cpp
    main.cpp)

add_executable(MrVizify MACOSX_BUNDLE ${SOURCE_FILES} ${HEADER_FILES} ${UISrcs} ${MOCSrcs} ${QT_WRAP})

if("${ITK_VERSION_MAJOR}" LESS 4)
  target_link_libraries(MrVizify ITKReview ${ITK_LIBRARIES} ${VTK_LIBRARIES})
else("${ITK_VERSION_MAJOR}" LESS 4)
  ## ITK Targets
  target_link_libraries(MrVizify PRIVATE ITKVtkGlue) 
  target_link_libraries(MrVizify PRIVATE ITKIOGDCM)
  target_link_libraries(MrVizify PRIVATE ITKStatistics) 
  target_link_libraries(MrVizify PRIVATE ITKIOBMP)
  target_link_libraries(MrVizify PRIVATE ITKIOBioRad) 
  target_link_libraries(MrVizify PRIVATE ITKIOBruker)
  target_link_libraries(MrVizify PRIVATE ITKIOGE) 
  target_link_libraries(MrVizify PRIVATE ITKIOGIPL) 
  target_link_libraries(MrVizify PRIVATE ITKIOHDF5) 
  target_link_libraries(MrVizify PRIVATE ITKIOJPEG)
  target_link_libraries(MrVizify PRIVATE ITKIOJPEG2000) 
  target_link_libraries(MrVizify PRIVATE ITKIOLSM)
  target_link_libraries(MrVizify PRIVATE ITKIOMINC) 
  target_link_libraries(MrVizify PRIVATE ITKIOMRC) 
  target_link_libraries(MrVizify PRIVATE ITKIOMeta) 
  target_link_libraries(MrVizify PRIVATE ITKIONRRD) 
  target_link_libraries(MrVizify PRIVATE ITKIONIFTI) 
  target_link_libraries(MrVizify PRIVATE ITKIOPNG)
  target_link_libraries(MrVizify PRIVATE ITKIOStimulate) 
  target_link_libraries(MrVizify PRIVATE ITKIOVTK)
  ## VTK Targets
  target_link_libraries(MrVizify PRIVATE VTK::GUISupportQt)
  target_link_libraries(MrVizify PRIVATE VTK::RenderingAnnotation)
  target_link_libraries(MrVizify PRIVATE VTK::ImagingStencil)
endif("${ITK_VERSION_MAJOR}" LESS 4)
target_link_libraries(MrVizify PRIVATE ${EXTRA_LIBS})