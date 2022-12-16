#include "cfileloader.h"
#include "itkCastImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

CFileLoader::CFileLoader():
m_FileLoaded(false),
m_RescaleData(false)
{
}

void CFileLoader::OpenFile(const std::string& cFileName)
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

bool CFileLoader::CanITKRead(const std::string& cFileName)
{
    /** Create a ImageIOBase Object **/
    auto imageIO = itk::ImageIOFactory::CreateImageIO(cFileName.c_str(), itk::ImageIOFactory::ReadMode);

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
    using TImageType=itk::Image<T, 4>;
    using FileReaderType=itk::ImageFileReader<TImageType>;

    /** Initialize a ITK File Reader **/
    auto cReader = FileReaderType::New();
    cReader->SetFileName(cFileName);

    /** Read **/
    try
    {
        cReader->Update();

        m_FileLoaded = true;

        auto cImage = cReader->GetOutput();

        // The Rescaling currently is only for tested for ADC map off READY View
        if (m_RescaleData)
        {
            using RegionIteratorDouble3D = itk::ImageRegionIterator<TImageType>;

            RegionIteratorDouble3D cImage1Iterator(cImage, cImage->GetLargestPossibleRegion());

            for (cImage1Iterator.GoToBegin(); !cImage1Iterator.IsAtEnd() ; ++cImage1Iterator)
            {

                //cImage1Iterator.Set((cImage1Iterator.Get() + 0.0000009999999975 )*1000000.00250000000625); for ADC of READYVIEW
                if (cImage1Iterator.Get() != 0)
                    cImage1Iterator.Set(cImage1Iterator.Get()*4094.9997568532469372031832331552  + 1);  // FOR FA of medINRIA
                
            }
        }
        

        using CalculatorFilterType = itk::MinimumMaximumImageCalculator<TImageType>;
        auto imageCalculator = CalculatorFilterType::New();
        imageCalculator->SetImage(cImage);
        imageCalculator->Compute();
        std::cout << "Intensity Values are " << imageCalculator->GetMinimum() << " " << imageCalculator->GetMaximum() << std::endl;

        using RescaleType = itk::CastImageFilter<TImageType, InputImage4DType>;
        auto rescaler = RescaleType::New();
        rescaler->SetInput(cImage);
        rescaler->Update();

        auto pType = cImage->GetOrigin();

        /** Print Meta Information about Image -- To Delete **/
        std::cout << "Origin of the image is at " << pType[0] << " " << pType[1] << " "  << pType[2] << " " << pType[3] << std::endl;

        auto dType = cImage->GetDirection() ;

        std::cout << "Direction Cosines are " << dType[0][0] << " " << dType[1][0] << " " << dType[2][0] << " "  << 
                                                 dType[0][1] << " " << dType[1][1] << " " << dType[2][1] << " "  <<
                                                 dType[0][2] << " " << dType[1][2] << " " << dType[2][2] << std::endl;

        m_XYSliceOrientation[0] =  dType[0][0] + dType[1][0] + dType[2][0];
        m_XYSliceOrientation[1] =  dType[0][1] + dType[1][1] + dType[2][1];

        auto &fileDict = cImage->GetMetaDataDictionary();

        auto itr = fileDict.Begin();
        auto end = fileDict.End();

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
    m_FileImage3D = nullptr;
    m_FileImage4D = nullptr;
    m_FileDir.clear();
    m_FileName.clear();
}

unsigned int CFileLoader::Get4thDimension() const
{
    /** return 4th dimension if 4D data in memory .. else 0 **/
    if (isFileLoaded())
        return m_FileImage4D->GetLargestPossibleRegion().GetSize()[3];
    else
        return 0;
}

void CFileLoader::GetImageXYOrientation(double *dir) const
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
    using ExtractImageFilterType = itk::ExtractImageFilter<InputImage4DType, InputImageType>;
    InputImage4DType::RegionType extractRegion;
    auto extractfilter = ExtractImageFilterType::New();
    auto inputRegion = m_FileImage4D->GetLargestPossibleRegion();
    auto inputSize = inputRegion.GetSize();
    auto inputIndex    = inputRegion.GetIndex();

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
        m_FileImage3D = nullptr;
    }

    return m_FileImage3D;
}

