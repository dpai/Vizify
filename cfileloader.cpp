#include "cfileloader.h"
#include "itkCastImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

/** Template Initializations **/
template InputImage4DType::Pointer CFileLoader::InternalImageReader<double>(const char *);
template InputImage4DType::Pointer CFileLoader::InternalImageReader<short>(const char *);

CFileLoader::CFileLoader()
{
	m_FileLoaded = false;
	m_RescaleData = false;
}

CFileLoader::~CFileLoader()
{
}

void CFileLoader::OpenFile(std::string cFileName)
{
	/** Clean the Old Stuff **/
	Clean();

	/** Check whether the file can be read by ITK **/
	if (CanITKRead(cFileName))
	{
		/** USe ITK to read the selected file **/
		std::cout << "Can Read " << cFileName << std::endl ;

		/** Read the Image - Convert to signed short if Pixel Data Type is other than short **/
		m_FileImage4D = ReadImage(cFileName.c_str(), m_InternalPixelType);
	}
	else
	{
		/** If ITK cannot read the selected file .. handle it here **/
		std::cout << "Cannot Read " << cFileName << std::endl;
		std::cout << "Not Implemented Yet " << std::endl;
	}
}

bool CFileLoader::CanITKRead(std::string cFileName)
{
	/** Create a ImageIOBase Object **/
	itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(cFileName.c_str(), itk::ImageIOFactory::ReadMode);

	/** File Cannot be Read **/
	if (imageIO.IsNull())
		return false;
	
	/** Check the Pixel Type within the file **/
	imageIO->SetFileName(cFileName);
	imageIO->ReadImageInformation();
	m_InternalPixelType = imageIO->GetComponentType();
	std::cout << "Pixel Type is : " << imageIO->GetComponentTypeAsString(m_InternalPixelType) << std::endl;

	/** ALL OK - File can be read **/
	return true;
}


InputImage4DType::Pointer CFileLoader::ReadImage(const char *cFileName, InternalPixelDataType pixelType)
{
	switch(pixelType)
	{
	case itk::ImageIOBase::DOUBLE: // If the Internal Type of image is double
		{
			std::cout << "Image Type has been rescaled from Double to Short. " << std::endl;
			return InternalImageReader<double>(cFileName);
		}
	case itk::ImageIOBase::FLOAT:
		{
			std::cout << "Image Type will rescale from float to short " << std::endl;
			return InternalImageReader<float>(cFileName);
		}
	default: // Default to signed short
			return InternalImageReader<short>(cFileName);
	}
}


template<typename T>
InputImage4DType::Pointer CFileLoader::InternalImageReader(const char * cFileName)
{
	/** TypeDefs **/
	typedef itk::Image<T, 4> TImageType;
	typedef itk::ImageFileReader<TImageType> FileReaderType;

	/** Initialize a ITK File Reader **/
	typename FileReaderType::Pointer cReader = FileReaderType::New();
	cReader->SetFileName(cFileName);

	/** Read **/
	try
	{
		cReader->Update();

		m_FileLoaded = true;

		typename TImageType::Pointer cImage = cReader->GetOutput();

		// The Rescaling currently is only for tested for ADC map off READY View
		if (m_RescaleData)
		{
			typedef itk::ImageRegionIterator<TImageType>				 RegionIteratorDouble3D;

			RegionIteratorDouble3D cImage1Iterator(cImage, cImage->GetLargestPossibleRegion());

			for (cImage1Iterator.GoToBegin(); !cImage1Iterator.IsAtEnd() ; ++cImage1Iterator)
			{

				//cImage1Iterator.Set((cImage1Iterator.Get() + 0.0000009999999975 )*1000000.00250000000625); for ADC of READYVIEW
				if (cImage1Iterator.Get() != 0)
					cImage1Iterator.Set(cImage1Iterator.Get()*4094.9997568532469372031832331552  + 1);  // FOR FA of medINRIA
				
			}
		}
		

		typedef itk::MinimumMaximumImageCalculator<TImageType> CalculatorFilterType;
		typename CalculatorFilterType::Pointer imageCalculator = CalculatorFilterType::New();
		imageCalculator->SetImage(cImage);
		imageCalculator->Compute();
		std::cout << "Intensity Values are " << imageCalculator->GetMinimum() << " " << imageCalculator->GetMaximum() << std::endl;

		typedef itk::CastImageFilter<TImageType, InputImage4DType> RescaleType;
		typename RescaleType::Pointer rescaler = RescaleType::New();
		rescaler->SetInput(cImage);
		rescaler->Update();

		typename TImageType::PointType pType = cImage->GetOrigin();

		/** Print Meta Information about Image -- To Delete **/
		std::cout << "Origin of the image is at " << pType[0] << " " << pType[1] << " "  << pType[2] << " " << pType[3] << std::endl;

		InputImage4DType::DirectionType dType = cImage->GetDirection() ;

		std::cout << "Direction Cosines are " << dType[0][0] << " " << dType[1][0] << " " << dType[2][0] << " "  << 
				                                    dType[0][1] << " " << dType[1][1] << " " << dType[2][1] << " "  <<
				                                    dType[0][2] << " " << dType[1][2] << " " << dType[2][2] << std::endl;

		m_XYSliceOrientation[0] =  dType[0][0] + dType[1][0] + dType[2][0];
		m_XYSliceOrientation[1] =  dType[0][1] + dType[1][1] + dType[2][1];

		itk::MetaDataDictionary &fileDict = cImage->GetMetaDataDictionary();

		itk::MetaDataDictionary::ConstIterator itr = fileDict.Begin();
		itk::MetaDataDictionary::ConstIterator end = fileDict.End();

		std::string value;
		std::string label;

		while (itr != end)
		{
			label = itr->first;
			value = dynamic_cast<itk::MetaDataObject<std::string> *>(itr->second.GetPointer())->GetMetaDataObjectValue();

			std::cout << " Element " << label << " has value " << value << std::endl;

			itr++;
		}

		return rescaler->GetOutput();

	}
	catch (itk::ExceptionObject &excp)
	{
		/** Exception: Could Not Load File **/
		std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		std::cerr << "Error reading file " << excp << std::endl;
		m_FileLoaded = false;
		return nullptr;
	}
}

void CFileLoader::Clean()
{
	m_FileLoaded = false;
	m_FileImage3D = NULL;
	m_FileImage4D = NULL;
	m_FileDir.clear();
	m_FileName.clear();
}

unsigned int CFileLoader::Get4thDimension()
{
	/** return 4th dimension if 4D data in memory .. else 0 **/
	if (isFileLoaded())
		return m_FileImage4D->GetLargestPossibleRegion().GetSize()[3];
	else
		return 0;
}

void CFileLoader::GetImageXYOrientation(double *dir)
{
	dir[0] = m_XYSliceOrientation[0];
	dir[1] = m_XYSliceOrientation[1];
}

InputImageType::Pointer CFileLoader::GetImage3D(unsigned int iID)
{
	/** Check if File Loaded **/
	if (!m_FileLoaded)
		return nullptr;

	/** Check if requested 3D data is valid**/
	if (iID > Get4thDimension())
		return nullptr;

	/** Extract the 3D data **/
	typedef itk::ExtractImageFilter< InputImage4DType, InputImageType > ExtractImageFilterType;
	InputImage4DType::RegionType				extractRegion;

	ExtractImageFilterType::Pointer		        extractfilter = ExtractImageFilterType::New();
	InputImage4DType::RegionType				inputRegion   = m_FileImage4D->GetLargestPossibleRegion();
		
	InputImage4DType::SizeType					inputSize	  = inputRegion.GetSize();

    InputImage4DType::IndexType					inputIndex    = inputRegion.GetIndex();

	extractfilter->InPlaceOn();
	extractfilter->SetDirectionCollapseToSubmatrix();

	/** Set Slice to Extract **/
	inputSize[3]  = 0;

	inputIndex[3] = iID;

	extractRegion.SetSize(inputSize);
	extractRegion.SetIndex(inputIndex);

	extractfilter->SetExtractionRegion(extractRegion);
	extractfilter->SetInput(m_FileImage4D);

	try 
	{
		extractfilter->Update();

		m_FileImage3D = extractfilter->GetOutput();
	}
	catch (itk::ExceptionObject &excp)
	{
		/** if 4D or 3D data is not valid **/
		std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
		std::cerr << "No 4D Volume Found or 3D data not valid " << excp << std::endl;
		m_FileImage3D = NULL;
	}

	return m_FileImage3D;
}

