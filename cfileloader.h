#ifndef CFILELOADER_H
#define CFILELOADER_H

/** C++ Includes **/
#include <string>
#include "dataobjects.h"

/** ITK Includes **/
#include <itkImageIOBase.h>
#include <itkImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkMetaDataObject.h>
#include <itkExtractImageFilter.h>


using namespace std;

class CFileLoader
{

/** Data **/
private:
    string									m_FileName;
    string									m_FileDir;
    InputImageType::Pointer					m_FileImage3D;
    InputImage4DType::Pointer               m_FileImage4D;
	InternalPixelDataType			        m_InternalPixelType;
	double									m_XYSliceOrientation[2];
    bool									m_FileLoaded;
    bool									m_RescaleData;

/** Constructors and Destructors **/
private:
    CFileLoader();

public:
    ~CFileLoader() = default;
	CFileLoader(const CFileLoader& rhs) = delete;
    CFileLoader& operator =(const CFileLoader& rhs) = delete;

/** Function Members **/
public:
    static CFileLoader* CreateNew() {
        return new CFileLoader();
    }

	bool isFileLoaded() const {
		return m_FileLoaded;
	}

	InputImageType::Pointer GetImage3D(unsigned int);

	InputImage4DType::Pointer GetImage4D() {
		return m_FileImage4D;
	}

	void OpenFile(std::string);

	void Clean();

	unsigned int Get4thDimension() const;

	void GetImageXYOrientation(double *dir);

	inline void RescaleDataOn() {
		m_RescaleData = true;
	}

	inline void RescaleDataOff() {
		m_RescaleData = false;
	}

private:

	bool CanITKRead(std::string);

	InputImage4DType::Pointer ReadImage(const char *, InternalPixelDataType);

	template<typename T>
	InputImage4DType::Pointer InternalImageReader(const char *);
};

#endif // CFILELOADER_H
