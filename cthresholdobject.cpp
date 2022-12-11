#include "cthresholdobject.h"

CThresholdObject::CThresholdObject() :
nLevels(1)
{
}

CThresholdObject::~CThresholdObject()
{
}

void CThresholdObject::SetOutput( ThresholdImageType::Pointer cImage)
{
	m_OutputImage = nullptr;

	/** Convert Threshold Image Type to Output Image Type **/
	using CastFilterType = itk::CastImageFilter<ThresholdImageType, OutputImageType>;

    auto caster = CastFilterType::New();
    caster->SetInput(cImage);

	try
	{
		caster->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "Rescale Image to Output Type Failed: " << excp << std::endl;
        return;
    }

	m_OutputImage = caster->GetOutput();
}

void CThresholdObject::SetRGBOutput (ThresholdImageType::Pointer cImage)
{
	m_RGBOutputImage = nullptr;
	/** Map ThresholdLabels to distinct RGB Colors **/
    using LabelRGBFilterType = itk::LabelToRGBImageFilter<ThresholdImageType, RGBImageType>;
    auto colormapImageFilter = LabelRGBFilterType::New();
    colormapImageFilter->SetInput(cImage);

    try
	{
		colormapImageFilter->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
        std::cerr << "Error--> (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
        std::cerr << "RGB Conversion Failed: " << excp << std::endl;
        return;
    }

	m_RGBOutputImage = colormapImageFilter->GetOutput();
}
