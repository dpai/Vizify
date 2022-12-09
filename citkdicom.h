#pragma once

#include "cdicom.h"

/** C++ Includes **/
#include <string>

/** ITK Inlcudes **/
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkExtractImageFilter.h"
#include "itkImage.h"

/** CITKDicom: A concrete class implementation of the DICOM loader based on the GDCM library wrapped by ITK **/

class CITKDicom final : public CDicom
{

/** Typedefs **/
private:
    using SeriesReaderType = itk::ImageSeriesReader< InputImage4DType >;
    using GDCMImageIOType = itk::GDCMImageIO;
    using InputNamesGeneratorType = itk::GDCMSeriesFileNames;
    using Input2DImageType =  itk::Image< PixelDataType, 2>;

/** Data **/
private:
    InputImage4DType::Pointer                                   m_DicomImage4D;
    InputImageType::Pointer										m_DicomImage3D;
    Input2DImageType::Pointer								    m_DicomImage2D;
    std::string                                                 m_DicomDir;
    SeriesReaderType::Pointer                                   m_dicomReader;
    bool                                                        m_DicomLoaded;

/** Constructors and Destructors **/
private:
    CITKDicom();

public:
    ~CITKDicom() = default;
    CITKDicom(const CITKDicom& rhs) = delete;
    CITKDicom& operator =(const CITKDicom& rhs) = delete;

/** Function Members **/
public:
    static CITKDicom* CreateNew() {
            return new CITKDicom{};
        }

    void OpenDicomDir(const std::string&) override;

    InputImage4DType::Pointer GetImage4D() override;

    InputImageType::Pointer  GetImage3D(int) override;

    bool isDicomLoaded() noexcept override;

    const std::string& GetDicomDir() override;

    const std::string& GetDescription() override;

    void Clean() override;

    unsigned int Get4thDimension() override;

private:
    bool ExtractVolumefrom4D(unsigned int iIndex);
    bool ExtractVolumefrom3D(unsigned int iIndex);
};
