#pragma once

/** C++ Inlcudes **/
#include <string>
#include <map>
#include "dataobjects.h"

/** Qt Includes **/
#include <QString>

/** ITK Includes **/

/** CDicom class is a abstract class for loading DICOM images **/

class CDicom
{
protected:
    CDicom() = default;
    CDicom(const CDicom& rhs);
    CDicom& operator =(const CDicom& rhs);

public:
    virtual ~CDicom() = default;
    virtual void OpenDicomDir(const std::string&) = 0;
    virtual InputImage4DType::Pointer GetImage4D() = 0;
    virtual bool isDicomLoaded() = 0;
    virtual const std::string& GetDicomDir() = 0;
    virtual const std::string& GetDescription() = 0;
    virtual unsigned int Get4thDimension() = 0;
    virtual void Clean() = 0;
    virtual InputImageType::Pointer GetImage3D(int) = 0;
    virtual bool GetImageParams(int, std::map<std::string, std::string>&) {
        return true;
    }
    virtual void SaveToDicom(const InputImageType::Pointer& Image, int volID, const QString& Dir, float, float, float)
    {
    }
    virtual void GetImageXYOrientation(double *dir) 
    {
        dir[0] = dir[1] = 1;
    }
};
