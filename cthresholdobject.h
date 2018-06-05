#ifndef CTHRESHOLDOBJECT_H
#define CTHRESHOLDOBJECT_H

#include "dataobjects.h"

/** Qt Includes **/
#include <QString>

/** ITK Includes **/
#include <itkLabelToRGBImageFilter.h>
#include <itkCastImageFilter.h>

/** Global Initialization **/
enum {
    Otsu = 0, Triangle = 1
};

/** typedefs **/
typedef unsigned char                                       ThresholdPixelType;

typedef itk::Image<ThresholdPixelType, InputDimension>      ThresholdImageType;

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
   int  nLevels;
   InputImageType::Pointer         m_InputImage;
   OutputImageType::Pointer        m_OutputImage;
   RGBImageType::Pointer           m_RGBOutputImage;

/** Function Members (Public) **/
public:
   void SetInput(InputImageType::Pointer cImage) {
	   m_InputImage = cImage;
   }

   InputImageType::Pointer GetInput() {
       return m_InputImage;
   }

   void SetThresholdLevels(int levels) {
       nLevels = levels;
   }

   virtual int Process() = 0;

   virtual QString GetDescription() = 0;

   OutputImageType::Pointer GetOutput() {
       return m_OutputImage;
   }

   RGBImageType * GetRGBOutput() {
	   return m_RGBOutputImage;
   }

   int GetLevels() {
       return nLevels;
   }

/** Function Members (Protected) **/
protected:
   void SetOutput(ThresholdImageType::Pointer cImage);

   void SetRGBOutput(ThresholdImageType::Pointer cImage);
};

#endif // CTHRESHOLDOBJECT_H
