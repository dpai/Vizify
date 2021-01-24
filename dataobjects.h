#pragma once

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkRGBPixel.h>
#include <itkImageIOBase.h>

/** Global Initializations **/
constexpr unsigned int InputDimension = 3;

constexpr unsigned int OutputDimension = 3;

constexpr unsigned int Input4DDimension = 4;

using RGBPixelType = itk::RGBPixel<unsigned char>;

using PixelDataType = signed short;

using InputImageType = itk::Image<PixelDataType, InputDimension>;

using OutputImageType = itk::Image<PixelDataType, OutputDimension>;

using RGBImageType =  itk::Image<RGBPixelType, InputDimension>;

using InputImage4DType = itk::Image<PixelDataType, Input4DDimension>;

using RegionIterator3D =  itk::ImageRegionIterator<InputImageType>;

using InternalPixelDataType = itk::ImageIOBase::IOComponentType;

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
