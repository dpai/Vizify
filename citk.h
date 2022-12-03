#pragma once
#include <cdicom.h>
#include <cfileloader.h>

/** C++ includes **/
#include <map>
#include <memory>

/** ITK Includes **/
#include "itkImageToVTKImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkExtractImageFilter.h"

/** Qt Includes **/
#include <QString>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHash>
#include <QMessageBox>
#include <QVariant>

/** VTK Includes **/
#include <vtkImageData.h>

class CITK
{

/** Data Members **/
private:
    enum class DataClass{ 
        NODATA = 2, 
        DICOMDATA = 4, 
        VOLDATA = 8 
    };

    std::unique_ptr<CDicom>                   m_DICOM;
    std::unique_ptr<CFileLoader>              m_FileLoader;
    QString                                   m_ImagePath;
    InputImageType::Pointer                   m_Input3DImage;
    InputImage4DType::Pointer                 m_Input4DImage;
    QStandardItemModel                       *m_ImageListModel;
    static CITK                              *m_CITKinstance;
    QHash<QString, InputImageType::Pointer>   m_ImageListMap;
    QString                                   m_ImageName;
    std::map<std::string, std::string>        m_Input3DImageParam;
    double									  m_XYImageOrientation[2];
    DataClass								  m_DataLoadState;
    unsigned int                              m_4thDimension;
    unsigned int						      m_VolumeSequenceID;

/** Constructors and Destructors **/
private:
    CITK();

 public:
    ~CITK();
    CITK(const CITK& rhs) = delete;
    CITK& operator =(const CITK& rhs) = delete;

/** Function Members **/
 public:
    static CITK* CreateNew() {
        if (m_CITKinstance == nullptr)
            m_CITKinstance = new CITK();
        return m_CITKinstance;
    }

    void LoadDicom();

    void LoadData();

    bool isDataLoaded();

    QString GetImageDirectory() {
        return m_ImagePath;
    }
    
    InputImageType::Pointer GetImage(const QString& = "Orig") const ;

    void SetImageListModel(QAbstractItemModel *tModel);

    template<typename T>
    int AddImageToList(QString, typename T::Pointer);

    template<typename T>
    vtkImageData * GetVTKData(typename T::Pointer);

    unsigned int Get4thDimension() {
        return m_4thDimension;
    }
    
    unsigned int GetCurrentVolumeID() {
        return m_VolumeSequenceID;
    }

    bool Update3DVolume(unsigned int);

    bool GetImageParamsAsModel(QAbstractItemModel *);

    void SaveToDicom(InputImageType::Pointer, QString, float, float, float);

    double *GetXYImageOrientation(void);

private:
    
    void SetInitTraits();

    void Init();

    bool Convert3Dto4D();
};
