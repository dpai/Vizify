#-------------------------------------------------
#
# Project created by QtCreator 2014-03-19T12:41:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

ITKINC = /usr/local/include/ITK-4.3
VTKINC = /usr/local/include/vtk-5.10

ITKLIBPATH = /usr/local/lib
VTKLIBPATH = /usr/local/lib/vtk-5.10


TARGET = MrVizify
TEMPLATE = app

INCLUDEPATH += $$ITKINC
INCLUDEPATH += $$VTKINC

LIBS += -L$$ITKLIBPATH -lITKIOGDCM-4.3 -litkgdcmMSFF-4.3 -litkopenjpeg-4.3 -litkgdcmjpeg16-4.3 -litkgdcmjpeg8-4.3 -lITKIOXML-4.3 -litkgdcmjpeg12-4.3 -litkgdcmCommon-4.3 -litkgdcmIOD-4.3 \
                       -litkgdcmDICT-4.3 -litkgdcmuuid-4.3 -litkgdcmDSED-4.3 -litksys-4.3 -litkvnl_algo-4.3 -litkvnl-4.3 -litkv3p_netlib-4.3 -lITKCommon-4.3 -litkNetlibSlatec-4.3 \
                       -lITKStatistics-4.3 -lITKIOImageBase-4.3 -lITKMesh-4.3 -litkzlib-4.3 -lITKMetaIO-4.3 -lITKSpatialObjects-4.3 -lITKPath-4.3 -lITKLabelMap-4.3 -lITKQuadEdgeMesh-4.3 \
                       -lITKOptimizers-4.3 -lITKPolynomials-4.3 -lITKBiasCorrection-4.3 -lITKBioCell-4.3 -lITKDICOMParser-4.3 -lITKEXPAT-4.3 -lITKIOXML-4.3 -lITKIOSpatialObjects-4.3 \
                       -lITKFEM-4.3 -lITKIOBMP-4.3 -lITKIOBioRad-4.3 -lITKIOIPL-4.3 -lITKIOGE-4.3 -lITKIOGIPL-4.3 -litkjpeg-4.3 -lITKIOJPEG-4.3 -lITKIOMeta-4.3 -lITKznz-4.3 \
                       -lITKniftiio-4.3 -lITKIONIFTI-4.3 -lITKNrrdIO-4.3 -lITKIONRRD-4.3 -litkpng-4.3 -lITKIOPNG-4.3 -litktiff-4.3 -lITKIOTIFF-4.3 -lITKIOTransformBase-4.3 \
                       -lITKIOSiemens-4.3 -lITKIOStimulate-4.3 -litkhdf5_cpp-4.3 -litkhdf5-4.3 -lITKIOTransformHDF5-4.3 -lITKIOTransformInsightLegacy-4.3 -lITKIOTransformMatlab-4.3 \
                       -lITKIOVTK-4.3 -lITKKLMRegionGrowing-4.3 -lITKIOLSM-4.3 -lITKVTK-4.3 -lITKWatersheds-4.3 -lITKDeprecated-4.3 -lITKgiftiio-4.3 -lITKIOCSV-4.3 -lITKIOHDF5-4.3 \
                       -lITKIOMesh-4.3 -lITKOptimizersv4-4.3 -lITKReview-4.3 -lITKVideoCore-4.3 -lITKVideoIO-4.3 -lITKVtkGlue-4.3 -lITKDICOMParser-4.3 -lITKVNLInstantiation-4.3 \
                       -litkvnl-4.3 -litkvcl-4.3

LIBS += -L$$VTKLIBPATH -lvtkHybrid -lvtkImaging -lvtkGraphics -lvtkRendering -lvtkCommon -lvtkGenericFiltering -lvtkFiltering  -lGL -lvtkWidgets  -lvtkIO -lvtktiff -lvtkjpeg \
                       -lvtkzlib -lvtkpng -lvtksys -lvtkexpat -lvtkViews -lvtkmetaio -lvtkproj4 -lvtkverdict -lvtksqlite -lvtkftgl -lvtkfreetype -lvtkexoIIc -lvtkNetCDF -lvtkhdf5 \
                       -lvtklibxml2 -lvtkalglib -lQVTK -lvtkVolumeRendering -lvtkInfovis -lvtkGeovis -lvtkCharts -lvtkverdict -lvtkNetCDF_cxx -lLSDyna -lvtkhdf5_hl -lvtkalglib \
                       -lvtkDICOMParser

LIBS += -lXt -lm -ldl -lpthread -lSM -lICE -lX11 -lXext


SOURCES += main.cpp\
        mrvizifymain.cpp \
    citk.cpp \
    cdicom.cpp \
    citkdicom.cpp \
    cvtkwidget.cpp \
    cmrvizthreshold.cpp \
    cthresholdfactory.cpp \
    cthresholdobject.cpp \
    cfileloader.cpp \
    compareimageui.cpp

HEADERS  += mrvizifymain.h \
    citk.h \
    cdicom.h \
    citkdicom.h \
    cvtkwidget.h \
    cmrvizthreshold.h \
    cthresholdfactory.h \
    dataobjects.h \
    cthresholdobject.h \
    thresholdobjects.h \
    MrVizifyConfig.h.in \
    cfileloader.h \
    compareimageui.h

FORMS    += mrvizifymain.ui \
    cmrvizthreshold.ui \
    compareimageui.ui
