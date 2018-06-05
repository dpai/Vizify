#ifndef CITKDICOM_H
#define CITKDICOM_H

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

class CITKDicom : public CDicom
{

/** Typedefs **/
private:
    typedef itk::ImageSeriesReader< InputImage4DType >          SeriesReaderType;

    typedef itk::GDCMImageIO                                    GDCMImageIOType;

    typedef itk::GDCMSeriesFileNames                            InputNamesGeneratorType;

	typedef itk::Image< PixelDataType, 2 >              Input2DImageType;

/** Data **/
private:
    InputImage4DType::Pointer                                   m_DicomImage4D;

	InputImageType::Pointer										m_DicomImage3D;

	Input2DImageType::Pointer										m_DicomImage2D;

    std::string                                                 m_DicomDir;

    SeriesReaderType::Pointer                                   m_dicomReader;

    bool                                                        m_DicomLoaded;

/** Constructors and Destructors **/
private:
    CITKDicom();
    CITKDicom(const CITKDicom& rhs);
    CITKDicom& operator =(const CITKDicom& rhs);

public:
    ~CITKDicom();

/** Function Members **/
public:
    static CITKDicom* CreateNew() {
            return new CITKDicom();
        }

    void OpenDicomDir(std::string );

    InputImage4DType::Pointer GetImage4D();

	InputImageType::Pointer  GetImage3D(int);

    bool isDicomLoaded();

    std::string GetDicomDir();

    std::string GetDescription();

	void Clean();

	unsigned int Get4thDimension();

private:
	bool ExtractVolumefrom4D(unsigned int iIndex);
	bool ExtractVolumefrom3D(unsigned int iIndex);

};

#endif // CITKDICOM_H
