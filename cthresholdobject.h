#pragma once

#include "dataobjects.h"

/** Qt Includes **/
#include <QString>

/** ITK Includes **/
#include <itkLabelToRGBImageFilter.h>
#include <itkCastImageFilter.h>

/** Global Initialization **/
enum class Algorithms 
{
    Otsu = 0, 
    Triangle = 1
};

/** typedefs **/
using ThresholdPixelType = unsigned char;
using ThresholdImageType = itk::Image<ThresholdPixelType, InputDimension>;

/** CThresholdObject: An abstract class for threshold objects **/

class CThresholdObject
{
/** Constructers and Desctructors **/
protected:
    CThresholdObject();
    CThresholdObject(const CThresholdObject& rhs);
    CThresholdObject& operator =(const CThresholdObject& rhs);

public:
    virtual ~CThresholdObject();

/** Data Members **/
private:
    InputImageType::Pointer m_InputImage;
    OutputImageType::Pointer m_OutputImage;
    RGBImageType::Pointer m_RGBOutputImage;
    int  nLevels;

/** Function Members (Public) **/
public:
    void SetInput(const InputImageType::Pointer& cImage) 
    {
        m_InputImage = cImage;
    }

    const InputImageType::Pointer& GetInput() const noexcept 
    {
        return m_InputImage;
    }

    void SetThresholdLevels(int levels) noexcept 
    {
        nLevels = levels;
    }

    virtual int Process() = 0;

    virtual const QString& GetDescription() = 0;

    const OutputImageType::Pointer& GetOutput() const noexcept 
    {
        return m_OutputImage;
    }

    const RGBImageType::Pointer& GetRGBOutput() const noexcept 
    {
        return m_RGBOutputImage;
    }

    int GetLevels() const noexcept 
    {
        return nLevels;
    }

/** Function Members (Protected) **/
protected:
    void SetOutput(ThresholdImageType::Pointer cImage);

    void SetRGBOutput(ThresholdImageType::Pointer cImage);
};
