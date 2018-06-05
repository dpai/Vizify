#ifndef DATAOBJECTS_H
#define DATAOBJECTS_H

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkRGBPixel.h>
#include <itkImageIOBase.h>

/** Global Initializations **/
const unsigned int InputDimension = 3;

const unsigned int OutputDimension = 3;

const unsigned int Input4DDimension = 4;

typedef itk::RGBPixel<unsigned char>                             RGBPixelType;

typedef signed short                                             PixelDataType;

typedef itk::Image< PixelDataType, InputDimension >              InputImageType;

typedef itk::Image< PixelDataType, OutputDimension >             OutputImageType;

typedef itk::Image< RGBPixelType, InputDimension >               RGBImageType;

typedef itk::Image< PixelDataType, Input4DDimension >            InputImage4DType;

typedef itk::ImageRegionIterator<InputImageType>				 RegionIterator3D;

typedef itk::ImageIOBase::IOComponentType						 InternalPixelDataType;

enum {
	FINAL_OUTPUT_IMAGE = 8, FINAL_OUTPUT_WITH_OVERLAY, INTERMEDIATE_OUTPUT_0, INTERMEDIATE_OUTPUT_1, INTERMEDIATE_OUTPUT_2, INTERMEDIATE_OUTPUT_3, INTERMEDIATE_OUTPUT_4, INTERMEDIATE_OUTPUT_5 
};

enum {
	CORR_COEFF = 100, DICE_COEFF
};

extern bool MatchImageSize(InputImageType::Pointer cImage1, InputImageType::Pointer cImage2);

extern void MakeSquareImage(InputImageType::Pointer cImage1, unsigned int iDims, short pixelValue);

extern void MakeGaussImage(InputImageType::Pointer cImage2, double variance);

extern void MakeLineImage(InputImageType::Pointer cImage1, unsigned int iDims);

#endif // DATAOBJECTS_H
