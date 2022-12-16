#pragma once

/** C++ Includes **/
#include <string>
#include "dataobjects.h"

/** ITK Includes **/
#include <itkImageIOBase.h>
#include <itkImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkMetaDataObject.h>
#include <itkExtractImageFilter.h>

class CFileLoader
{

/** Data **/
private:
    std::string m_FileName;
    std::string m_FileDir;
    InputImageType::Pointer m_FileImage3D;
    InputImage4DType::Pointer m_FileImage4D;
    InternalPixelDataType m_InternalPixelType;
    double m_XYSliceOrientation[2];
    bool m_FileLoaded;
    bool m_RescaleData;

/** Constructors and Destructors **/
private:
    CFileLoader();

public:
    ~CFileLoader() = default;
    CFileLoader(const CFileLoader& rhs) = delete;
    CFileLoader& operator =(const CFileLoader& rhs) = delete;

/** Function Members **/
public:
    static CFileLoader* CreateNew() 
    {
        return new CFileLoader();
    }

    bool isFileLoaded() const noexcept 
    {
        return m_FileLoaded;
    }

    InputImageType::Pointer GetImage3D(unsigned int);

    const InputImage4DType::Pointer& GetImage4D() const noexcept
    {
        return m_FileImage4D;
    }

    void OpenFile(const std::string&);

    void Clean();

    unsigned int Get4thDimension() const;

    void GetImageXYOrientation(double *dir) const;

    inline void RescaleDataOn() noexcept
    {
        m_RescaleData = true;
    }

    inline void RescaleDataOff() noexcept
    {
        m_RescaleData = false;
    }

private:

    bool CanITKRead(const std::string&);

    InputImage4DType::Pointer ReadImage(const char *, InternalPixelDataType);

    template<typename T>
    InputImage4DType::Pointer InternalImageReader(const char *);
};
