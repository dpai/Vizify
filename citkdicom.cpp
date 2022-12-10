#include "citkdicom.h"
#include <iostream>

// Test
#include <itkImageSeriesWriter.h>
#include <itkTileImageFilter.h>

CITKDicom::CITKDicom() :
m_DicomLoaded(false)
{
}

const std::string& CITKDicom::GetDescription()
{
    static const std::string desc{"ITK DICOM: GDCM"};
    return desc;
}

void CITKDicom::Clean()
{
    m_DicomLoaded = false;
    m_DicomImage4D = nullptr;
    m_DicomDir.clear();
    m_dicomReader = nullptr;
}

void CITKDicom::OpenDicomDir(const std::string& strdir)
{
    /** Clean the Old Stuff **/
    Clean();

    /** Save the Dicom dir **/
    m_DicomDir = strdir;

    /** Initialize an GDCM IO Object **/
    auto gdcmIO = GDCMImageIOType::New();

    /** Get the filenames in the series **/
    auto inputNames = InputNamesGeneratorType::New();
    inputNames->SetLoadPrivateTags(true);
    inputNames->SetInputDirectory(strdir);

    /** Initialize and Dicom Reader and read the Image Series **/
    m_dicomReader = SeriesReaderType::New();

    try
    {
        const auto& filenames = inputNames->GetInputFileNames();

        // Note: Code Hack To allow Private Tag Loading .
        gdcmIO->SetLoadPrivateTags(true);

        m_dicomReader->SetImageIO( gdcmIO );
        m_dicomReader->SetFileNames( filenames );
        m_dicomReader->Update();

        auto fitr = filenames.begin();

        while (fitr != filenames.end())
        {
            std::cout << "FileName is " << *fitr << std::endl;
            fitr++;
        }

        m_DicomLoaded = true;
        m_DicomImage4D = m_dicomReader->GetOutput();

        const auto fileDict = (*m_dicomReader->GetMetaDataDictionaryArray())[0];

        // Get some information on the header
        itk::MetaDataDictionary::ConstIterator itr = fileDict->Begin();
        itk::MetaDataDictionary::ConstIterator end = fileDict->End();

        std::string value;
        std::string label;

        // while (itr != end)
        // {
        // 	label = itr->first;
        // 	value = dynamic_cast<itk::MetaDataObject<std::string> *>(itr->second.GetPointer())->GetMetaDataObjectValue();

        // 	//std::cout << " Element " << label << " has value " << value << std::endl;

        // 	itr++;
        // }

        std::string entryId = "0020|0013";
        itr = fileDict->Find( entryId );
        if( itr == end )
        {
            std::cerr << "Tag " << entryId;
            std::cerr << " not found in the DICOM header" << std::endl;
            return ;
        }
        else
        {
            label = itr->first;
            value = dynamic_cast<itk::MetaDataObject<std::string> *>(itr->second.GetPointer())->GetMetaDataObjectValue();

            std::cout << " Found Element " << label << " has value " << value << std::endl;
        }

        // Other Relevant Information
        InputImage4DType::PointType pType = m_DicomImage4D->GetOrigin();
        std::cout << "Origin is " << pType[0] << " " << pType[1] << " " << pType[2] << " " << pType[3] << std::endl;
        InputImage4DType::IndexType iType;
        m_DicomImage4D->TransformPhysicalPointToIndex(pType, iType);
        std::cout << "Index of First Point is " << iType[0] <<  " " << iType[1] << "  " << iType[2] << std::endl;
        InputImage4DType::OffsetType oType;
        InputImage4DType::DirectionType dType = m_DicomImage4D->GetDirection();
        InputImage4DType::OffsetType::OffsetValueType ovType;
        InputImage4DType::SpacingType sType = m_DicomImage4D->GetSpacing();

        ovType = m_DicomImage4D->ComputeOffset(iType);
        std::cout << "Offset Value is : " << ovType  << std::endl;
        std::cout << "Direction " << dType[0][0] << " " << dType[1][0] << " " << dType[2][0] << " "  << dType[0][1] << " " << dType[1][1] << " " << dType[2][1] << std::endl;
        std::cout << "Spacing " << sType[0] << " " << sType[1] << " " << sType[2] << std::endl;

        /*ExtractVolumefrom4D(0);
        ExtractVolumefrom3D(0);

        //Tile it back
        typedef itk::TileImageFilter< Input2DImageType, OutputImageType > TilerType;
        TilerType::Pointer tiler = TilerType::New();
 
          itk::FixedArray< unsigned int, OutputDimension > layout;
 
          layout[0] = 1;
          layout[1] = 1;
          layout[2] = 0;
 
          tiler->SetLayout( layout );
 
          unsigned int inputImageNumber = 0;

          for (int ii = 0; ii < 30; ii++)
          {
              ExtractVolumefrom3D(ii);
              m_DicomImage2D->DisconnectPipeline();
              tiler->SetInput(inputImageNumber++, m_DicomImage2D);
          }

          PixelDataType filler = 128;
 
          tiler->SetDefaultPixelValue( filler );
 
          tiler->Update();

          InputImageType::PointType pType1 = tiler->GetOutput()->GetOrigin();
        std::cout << "Origin is " << pType1[0] << " " << pType1[1] << " " << pType1[2] << std::endl;

        // Test DICOM Save from GDCM 
        typedef itk::Image< PixelDataType, 2 > TestOutputImageType;

        typedef itk::ImageSeriesWriter< InputImageType, TestOutputImageType > SeriesWriterType;

        SeriesWriterType::Pointer writer = SeriesWriterType::New();

        writer->SetInput(tiler->GetOutput());

        writer->SetImageIO(gdcmIO);

        inputNames->SetOutputDirectory("C:\\Workspace\\TestGDCMData");

        writer->SetFileNames(inputNames->GetOutputFileNames());

        writer->SetMetaDataDictionaryArray(m_dicomReader->GetMetaDataDictionaryArray());


        writer->Update();*/
    }
    catch (itk::ExceptionObject &excp)
    {
        /** Exception: Dir does not contain DICOM series **/
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "Exception in series " << excp << std::endl;
        m_DicomLoaded = false;
        return;
    }
}

InputImage4DType::Pointer CITKDicom::GetImage4D()
{
    return (m_DicomLoaded) ? m_DicomImage4D : nullptr;
}

InputImageType::Pointer CITKDicom::GetImage3D(int iIndex)
{
    /** CCheck if Data is loaded **/
    if (!m_DicomLoaded)
        return nullptr;

    /** Check if 3D data requested is valid **/
    if (iIndex > Get4thDimension())
        return nullptr;

    /** Extract 3D data **/
    if (ExtractVolumefrom4D(iIndex))
        return m_DicomImage3D;
    else
        return nullptr;
}

bool CITKDicom::ExtractVolumefrom4D(unsigned int iIndex)
{
    /** Check if 4D Data is in memory **/
    if (m_DicomImage4D.IsNull())
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "No 4D Image was found in memory " << std::endl;
        return false;
    }

    /** Extract the 3D data **/
    using ExtractImageFilterType = itk::ExtractImageFilter<InputImage4DType, InputImageType>;
    auto extractfilter = ExtractImageFilterType::New();
    auto inputRegion = m_DicomImage4D->GetLargestPossibleRegion();
    auto inputSize = inputRegion.GetSize();
    auto inputIndex = inputRegion.GetIndex();

    extractfilter->InPlaceOn();
    extractfilter->SetDirectionCollapseToSubmatrix();

    /** Set Slice to Extract **/
    inputSize[3]  = 0;
    inputIndex[3] = iIndex;
    InputImage4DType::RegionType extractRegion;
    extractRegion.SetSize(inputSize);
    extractRegion.SetIndex(inputIndex);

    extractfilter->SetExtractionRegion(extractRegion);
    extractfilter->SetInput(m_DicomImage4D);
    extractfilter->Update();

    m_DicomImage3D = extractfilter->GetOutput();

    return true;
}


bool CITKDicom::ExtractVolumefrom3D(unsigned int iIndex)
{
    /** Check if 4D Data is in memory **/
    if (m_DicomImage3D.IsNull())
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "No 4D Image was found in memory " << std::endl;
        return false;
    }

    /** Extract the 3D data **/
    using ExtractImageFilterType = itk::ExtractImageFilter<InputImageType, Input2DImageType>;
    auto extractfilter = ExtractImageFilterType::New();
    auto inputRegion = m_DicomImage3D->GetLargestPossibleRegion();
    auto inputSize = inputRegion.GetSize();
    auto inputIndex = inputRegion.GetIndex();

    extractfilter->InPlaceOn();
    extractfilter->SetDirectionCollapseToSubmatrix();

    /** Set Slice to Extract **/
    inputSize[2]  = 0;
    inputIndex[2] = iIndex;

    InputImageType::RegionType extractRegion;
    extractRegion.SetSize(inputSize);
    extractRegion.SetIndex(inputIndex);

    extractfilter->SetExtractionRegion(extractRegion);
    extractfilter->SetInput(m_DicomImage3D);
    extractfilter->Update();

    m_DicomImage2D = extractfilter->GetOutput();

    return true;
}

bool CITKDicom::isDicomLoaded() noexcept
{
    return m_DicomLoaded;
}

const std::string& CITKDicom::GetDicomDir()
{
    return m_DicomDir;
}

unsigned int CITKDicom::Get4thDimension()
{
    /** get the 4th dimension of 4D data -- if not present return 0 **/
    if (isDicomLoaded())
        return m_DicomImage4D->GetLargestPossibleRegion().GetSize()[3];
    else
        return 0;
}
