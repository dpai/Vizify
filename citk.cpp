#include "citk.h"
#include "citkdicom.h"
#include "MrVizifyConfig.h"

#ifdef USE_ITKDCMTK
#include "cdcmtkdicom.h"
#endif

#ifdef USE_MERGE
#include "cmergedicom.h"
#endif

#include <iostream>

#ifdef WIN32
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

/** Static Declaration **/
CITK *CITK::m_CITKinstance = nullptr;

/** template Initializations **/
template int CITK::AddImageToList<OutputImageType>(QString, OutputImageType::Pointer);
template vtkImageData * CITK::GetVTKData<InputImageType>(InputImageType::Pointer);
template vtkImageData * CITK::GetVTKData<RGBImageType>(RGBImageType::Pointer);


CITK::CITK(void)
    :m_ImageListModel(nullptr),
      m_ImageName("Orig")
{

/** Define a DICOM instance **/
#ifdef USE_ITKDCMTK
    m_DICOM = CDCMTKDicom::CreateNew();
#elif defined(USE_MERGE)
	m_DICOM = CMergeDicom::CreateNew();
#else
    m_DICOM = std::unique_ptr<CITKDicom>(CITKDicom::CreateNew());
#endif

    std::cout << m_DICOM->GetDescription() << std::endl;

	/** Define a File Data Load Instance **/
	m_FileLoader = std::unique_ptr<CFileLoader>(CFileLoader::CreateNew());

	Init();

}

CITK::~CITK()
{
	m_ImageListMap.clear();
}

void CITK::Init()
{
	/** Return to Initial Pristine State **/
	m_ImageListMap.clear();

	if (m_ImageListModel)
		m_ImageListModel->clear();

	m_DataLoadState = NODATA;

	m_VolumeSequenceID = 0;

	m_4thDimension = 0;

	m_Input3DImage = nullptr;

	m_Input4DImage = nullptr;

	m_XYImageOrientation[0] = m_XYImageOrientation[1] = 1;

	m_Input3DImageParam.clear();
}

void CITK::LoadDicom()
{
    /** Ask for the folder **/
    QFileDialog                 DDialog;

    DDialog.setFileMode(QFileDialog::Directory);
    DDialog.setOption(QFileDialog::ShowDirsOnly);
	DDialog.setOption(QFileDialog::DontUseNativeDialog);

    int result = DDialog.exec();

    /** Choose Directory **/
    if (result)
    {
        m_ImagePath = DDialog.selectedFiles()[0];
    }
    else
    {
        return;
    }

	/** Init To Clean State **/
	Init();

    /** Load Dicom **/
    m_DICOM->OpenDicomDir(m_ImagePath.toStdString());

    m_Input4DImage = m_DICOM->GetImage4D();

	m_Input3DImage = m_DICOM->GetImage3D(m_VolumeSequenceID);

	m_4thDimension = m_DICOM->Get4thDimension();

	m_DICOM->GetImageParams(0, m_Input3DImageParam);

	m_DICOM->GetImageXYOrientation(m_XYImageOrientation);

	/** Was Data Loading successful **/
	m_DataLoadState = (m_DICOM->isDicomLoaded())?DICOMDATA:NODATA;

	/** Private Function - Cleanse all old data and reinitialize everything **/ 
	SetInitTraits();
	
}

void CITK::LoadData()
{
	/** Ask for the file **/
    QFileDialog                 FDialog;

    int result = FDialog.exec();

    /** Choose File **/
    if (result)
    {
        m_ImagePath = FDialog.selectedFiles()[0];
		 
    }
    else
    {
        return;
    }

	/** Init To Clean State **/
	Init();

	/** Open and Read File **/
	m_FileLoader->OpenFile(m_ImagePath.toStdString());

	/** Get the 4D Image **/
	m_Input4DImage = m_FileLoader->GetImage4D();

	/** If Image retrieved, extract the first 3D image **/
	if (m_Input4DImage.IsNotNull())
	{
		m_4thDimension = m_Input4DImage->GetLargestPossibleRegion().GetSize()[3];

		/** Extract a Sequence of 3D images from the 4D image. THe number of volumes can be one or more **/
		m_Input3DImage = m_FileLoader->GetImage3D(m_VolumeSequenceID);

		/** Get the Image Direction **/
		m_FileLoader->GetImageXYOrientation(m_XYImageOrientation);
	}

	/** Was Data Loading successful **/
	m_DataLoadState = (m_FileLoader->isFileLoaded())?VOLDATA:NODATA;

	/** Private Function - Cleanse all old data and reinitialize everything **/ 
	SetInitTraits();
}

void CITK::SetInitTraits()
{
	/** Making new Maps **/
    m_ImageListMap.insert(m_ImageName, m_Input3DImage );

    m_ImageListModel->appendRow(new QStandardItem(m_ImageName));
}

bool CITK::isDataLoaded()
{
	/** Return the Data State Loaded **/
	switch (m_DataLoadState)
	{
	case VOLDATA:
		return m_FileLoader->isFileLoaded();
	case DICOMDATA:
		return m_DICOM->isDicomLoaded();
	case NODATA:
	default:
		return false;
	}
}

void CITK::SetImageListModel(QAbstractItemModel *tModel)
{
	/** Fix the List Model to the Qt Gui used to visualize image names**/
    if (m_ImageListModel == nullptr)
        m_ImageListModel = static_cast<QStandardItemModel*>(tModel);
}

InputImageType::Pointer CITK::GetImage(const QString &key) const
{
	/** Test whether the List Model is Set **/
	if (m_ImageListModel == nullptr)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "ListModel Not Set: Please use SetImageListModel() " << std::endl;
        return nullptr;
    }

	return m_ImageListMap[key];
}

bool CITK::Update3DVolume(unsigned int value)
{
	/** Check if the Data is Loaded **/
	if (!isDataLoaded())
	{
		std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "No Data Loaded" << std::endl;
        return false;
	}

	/** Check if the 4D data is present **/
	if (m_4thDimension == 0)
	{
		std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "No 4D Volume Found" << std::endl;
        return false;
	}

	/** Check if the volume requested is Valid **/
	if (value >= m_4thDimension)
	{
		std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "Requested index exceeds 4th Dimension (iIndex = " << value << " ; 4th Dimension Index = " << m_4thDimension << ")" << std::endl;
        return false;
	}

	/** Update current 3D vloume loaded index **/
	m_VolumeSequenceID = value;

	/** Extract from File Loader Object or Dicom Loader Object **/
	if (m_DataLoadState == VOLDATA)
		m_Input3DImage = m_FileLoader->GetImage3D(m_VolumeSequenceID);
	else //DICOM
	{
		m_Input3DImage = m_DICOM->GetImage3D(m_VolumeSequenceID);
		m_DICOM->GetImageParams(value, m_Input3DImageParam);
	}
	
	// Insert into current 3D visualization list for the GUI 
	m_ImageListMap.insert(m_ImageName, m_Input3DImage );

	return true;
}

bool CITK::GetImageParamsAsModel(QAbstractItemModel *model)
{
	int iCount = model->rowCount();
	int iRow = 0;
	std::map<std::string, std::string>::const_iterator itr = m_Input3DImageParam.begin();

	model->removeRows(0, iCount);

	while (itr != m_Input3DImageParam.end())
	{
		model->insertRow(iRow);

		model->setData(model->index(iRow,0), QString::fromUtf8(itr->first.c_str()));

		model->setData(model->index(iRow,1), QString::fromUtf8(itr->second.c_str()));

		itr++;
	}


	return true;
}

double *CITK::GetXYImageOrientation()
{
	return m_XYImageOrientation;
}

void CITK::SaveToDicom(InputImageType::Pointer tImage, QString Dir, float Dx, float Dy, float Dz)
{
	//Teast
	m_DICOM->SaveToDicom(tImage, m_VolumeSequenceID, Dir, Dx, Dy, Dz);
}

template<typename T>
int CITK::AddImageToList(QString key,  typename T::Pointer value)
{
	/** Error - The Input combo boxes are not connected **/
    if (m_ImageListModel == nullptr)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "ListModel Not Set: Please use SetImageListModel() " << std::endl;
        return -1;
    }

	/** Add Image Data to the map - Maintain only single copies**/
    m_ImageListMap.insert(key, value);

	if (m_ImageListModel->findItems(key).count() == 0)
		m_ImageListModel->appendRow(new QStandardItem(key));

    return 1;
}

template<typename T>
vtkImageData * CITK::GetVTKData(typename T::Pointer cImage)
{
    if (!isDataLoaded())
        return 0;

    bool flipAxes[3] = { false, false, false };

    /** Flip Image to Right Orientation for Visualization in VTK **/
    typedef itk::FlipImageFilter< T > FlipImageFilterType;
    typename FlipImageFilterType::Pointer flipImage = FlipImageFilterType::New();
    flipImage->SetFlipAxes( flipAxes );
    flipImage->SetInput(cImage);

    /** Note: I can control different itk::Image<> pointers here **/
    /** Convert ITK Image to VTK Image **/
    typedef itk::ImageToVTKImageFilter< T > itkVTKConverter3D;
    typename itkVTKConverter3D::Pointer convert = itkVTKConverter3D::New();
    convert->SetInput(flipImage->GetOutput());
    convert->Update();

    vtkImageData *cVTKData = vtkImageData::New();
    cVTKData->DeepCopy(convert->GetOutput());

    return cVTKData;
}

