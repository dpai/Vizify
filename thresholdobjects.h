#ifndef THRESHOLDOBJECTS_H
#define THRESHOLDOBJECTS_H

#include "cthresholdobject.h"
#include "dataobjects.h"

/** ITK Includes **/
#include <itkOtsuMultipleThresholdsImageFilter.h>
#include <itkTriangleThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

/** Otsu Threshold **/
class COtsuThresholdObject : public CThresholdObject
{
private:
    COtsuThresholdObject() {
    }
    COtsuThresholdObject(const COtsuThresholdObject& rhs);
    COtsuThresholdObject& operator =(const COtsuThresholdObject& rhs);

public:
    ~COtsuThresholdObject(){}

    static COtsuThresholdObject* CreateNew() {
        return new COtsuThresholdObject();
    }

	QString GetDescription() {
		return QString("_Otsu");
	}

    int Process() {

		/** Initializations **/
        InputImageType::Pointer cImage = GetInput();
		int levels = GetLevels();

		/** Sanity Chech - Input Image is Present **/
        if (cImage.IsNotNull())
        {
            /** Initialize the Otsu Thresholder **/
            typedef itk::OtsuMultipleThresholdsImageFilter< ThresholdImageType, ThresholdImageType > OtsuFilterType;
            typedef OtsuFilterType::ThresholdVectorType ThresholdVectorType;

            typedef itk::RescaleIntensityImageFilter< InputImageType, ThresholdImageType > RescaleFilterType;

			/** Rescale the Image **/
            RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
            rescaler->SetOutputMinimum( 0 );
            rescaler->SetOutputMaximum( 255 );
            rescaler->SetInput(cImage);

			/** Setup the Otsu Filter Input **/
            OtsuFilterType::Pointer otsuFilter = OtsuFilterType::New();

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
			const ThresholdVectorType &thresholdVector = otsuFilter->GetThresholds();
			ThresholdVectorType::const_iterator itNum = thresholdVector.begin();

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
class CTriangleThresholdObject : public CThresholdObject
{
private:
    CTriangleThresholdObject() {
    }
    CTriangleThresholdObject(const CTriangleThresholdObject& rhs);
    CTriangleThresholdObject& operator =(const CTriangleThresholdObject& rhs);

public:
    ~CTriangleThresholdObject(){}

    static CTriangleThresholdObject* CreateNew() {
        return new CTriangleThresholdObject();
    }

	QString GetDescription() {
		return QString("_Triangle");
	}

    int Process() {

		/** Initializations **/
        InputImageType::Pointer cImage = GetInput();
		int levels = GetLevels();

		/** Sanity Chech - Input Image is Present **/
        if (cImage.IsNotNull())
        {
            /** Initialize the Otsu Thresholder **/
            typedef itk::TriangleThresholdImageFilter< ThresholdImageType, ThresholdImageType > TriangleFilterType;

            typedef itk::RescaleIntensityImageFilter< InputImageType, ThresholdImageType > RescaleFilterType;

			/** Rescale the Image **/
            RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
            rescaler->SetOutputMinimum( 0 );
            rescaler->SetOutputMaximum( 255 );
            rescaler->SetInput(cImage);

			/** Setup the Otsu Filter Input **/
            TriangleFilterType::Pointer triangleFilter = TriangleFilterType::New();

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
#endif // THRESHOLDOBJECTS_H
