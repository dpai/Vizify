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

	bool									m_FileLoaded;

	double									m_XYSliceOrientation[2];

	InternalPixelDataType			        m_InternalPixelType;

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

	bool isFileLoaded() {
		return m_FileLoaded;
	}

	InputImageType::Pointer GetImage3D(unsigned int);

	InputImage4DType::Pointer GetImage4D() {
		return m_FileImage4D;
	}

	void OpenFile(std::string);

	void Clean();

	unsigned int Get4thDimension();

	void GetImageXYOrientation(double *dir);

	void RescaleDataOn() {
		m_RescaleData = true;
	}

	void RescaleDataOff() {
		m_RescaleData = false;
	}

private:

	bool CanITKRead(std::string);

	InputImage4DType::Pointer ReadImage(const char *, InternalPixelDataType);

	template<typename T>
	InputImage4DType::Pointer InternalImageReader(const char *);


};

#endif // CFILELOADER_H
