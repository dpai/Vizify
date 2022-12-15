#pragma once

#include "cthresholdobject.h"
#include "dataobjects.h"

/** ITK Includes **/
#include <itkOtsuMultipleThresholdsImageFilter.h>
#include <itkTriangleThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

/** Otsu Threshold **/
class COtsuThresholdObject final : public CThresholdObject
{
public:
    COtsuThresholdObject() = default;
    COtsuThresholdObject(const COtsuThresholdObject& rhs) = delete;
    COtsuThresholdObject& operator =(const COtsuThresholdObject& rhs) = delete;

    static COtsuThresholdObject* CreateNew() 
    {
        return new COtsuThresholdObject();
    }

    const QString& GetDescription() override
    {
        static const QString desc{"_Otsu"};
        return desc;
    }

    int Process() override
    {
        /** Initializations **/
        const auto cImage = GetInput();
        const auto levels = GetLevels();

        /** Sanity Chech - Input Image is Present **/
        if (cImage.IsNotNull())
        {
            /** Initialize the Otsu Thresholder **/
            using OtsuFilterType = itk::OtsuMultipleThresholdsImageFilter<ThresholdImageType, ThresholdImageType>;
            using ThresholdVectorType = OtsuFilterType::ThresholdVectorType;
            using RescaleFilterType = itk::RescaleIntensityImageFilter<InputImageType, ThresholdImageType> ;

            /** Rescale the Image **/
            auto rescaler = RescaleFilterType::New();
            rescaler->SetOutputMinimum( 0 );
            rescaler->SetOutputMaximum( 255 );
            rescaler->SetInput(cImage);

            /** Setup the Otsu Filter Input **/
            auto otsuFilter = OtsuFilterType::New();
            otsuFilter->SetInput(rescaler->GetOutput());
            otsuFilter->SetNumberOfThresholds(levels);
            otsuFilter->SetNumberOfHistogramBins(255);

            /** Threshold THe Image **/
            try
            {
                std::cout << "Performing the Threshold " << std::endl;
                otsuFilter->Update();
            }
            catch (itk::ExceptionObject &excp)
            {
                std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
                std::cerr << "Otsu Threshold Operation Failed: " << excp << std::endl;
                return -1;
            }

            /** Print The Thresholds on the Console - Will add a Status bar later **/
            const auto& thresholdVector = otsuFilter->GetThresholds();
            auto itNum = thresholdVector.begin();

            for (; itNum < thresholdVector.end() ; itNum++)
            {
                std::cout << "OtsuThreshold[" << (int)(itNum - thresholdVector.begin())  << "] = " << (*itNum) << std::endl ;
            }

            /** Final Output Image **/
            SetOutput(otsuFilter->GetOutput());

            /** Final THresholded Color Label Map **/
            //SetRGBOutput(otsuFilter->GetOutput());

            return 1;
        }

        /** No Image **/
        return -1;
    }  // End of Process()
};


/** Triangle Threshold **/
class CTriangleThresholdObject final : public CThresholdObject
{
public:
    CTriangleThresholdObject() = default;
    CTriangleThresholdObject(const CTriangleThresholdObject& rhs) = delete;
    CTriangleThresholdObject& operator =(const CTriangleThresholdObject& rhs) = delete;

    static CTriangleThresholdObject* CreateNew() 
    {
        return new CTriangleThresholdObject();
    }

    const QString& GetDescription() override
    {
        static const QString desc{"_Triangle"};
        return desc;
    }

    int Process() override
    {

        /** Initializations **/
        const auto cImage = GetInput();
        const auto levels = GetLevels();

        /** Sanity Chech - Input Image is Present **/
        if (cImage.IsNotNull())
        {
            /** Initialize the Otsu Thresholder **/
            using TriangleFilterType = itk::TriangleThresholdImageFilter<ThresholdImageType, ThresholdImageType>;
            using RescaleFilterType = itk::RescaleIntensityImageFilter<InputImageType, ThresholdImageType>;

            /** Rescale the Image **/
            auto rescaler = RescaleFilterType::New();
            rescaler->SetOutputMinimum( 0 );
            rescaler->SetOutputMaximum( 255 );
            rescaler->SetInput(cImage);

            /** Setup the Otsu Filter Input **/
            auto triangleFilter = TriangleFilterType::New();
            triangleFilter->SetInput(rescaler->GetOutput());
            triangleFilter->SetNumberOfHistogramBins(255);
            triangleFilter->SetInsideValue(0);
            triangleFilter->SetOutsideValue(255);

            /** Threshold THe Image **/
            try
            {
                std::cout << "Performing the Threshold " << std::endl;
                triangleFilter->Update();
            }
            catch (itk::ExceptionObject &excp)
            {
                std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
                std::cerr << "Triangle Threshold Operation Failed: " << excp << std::endl;
                return -1;
            }

            /** Print The Thresholds on the Console - Will add a Status bar later **/
            std::cout << "TriangleThreshold: " << (int)triangleFilter->GetThreshold() << std::endl ;

            /** Final Output Image **/
            SetOutput(triangleFilter->GetOutput());

            /** Final THresholded Color Label Map **/
            //SetRGBOutput(triangleFilter->GetOutput());

            return 1;
        }

        /** No Image **/
        return -1;
    }  // End of Process()
};
