/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkOpenCVToMitkImageFilter.h"

#include <itkImportImageFilter.h>
#include <itkRGBPixel.h>
#include <mitkITKImageImport.txx>
#include <itkOpenCVImageBridge.h>
#include <itkImageFileWriter.h>

#include "mitkImageToOpenCVImageFilter.h"

namespace mitk{

  OpenCVToMitkImageFilter::OpenCVToMitkImageFilter()
  {
  }

  OpenCVToMitkImageFilter::~OpenCVToMitkImageFilter()
  {
  }

  void OpenCVToMitkImageFilter::SetOpenCVMat(const cv::Mat &image)
  {
    m_OpenCVMatMutex.lock();
    m_OpenCVMat = image;
    m_OpenCVMatMutex.unlock();
    this->Modified();
  }

  void OpenCVToMitkImageFilter::SetOpenCVImage(const IplImage* image)
  {
    const cv::Mat cvMat = cv::cvarrToMat(image, false);
    this->SetOpenCVMat(cvMat);
  }

  void OpenCVToMitkImageFilter::GenerateData()
  {
    if (m_OpenCVMat.cols != 0 && m_OpenCVMat.rows != 0 && m_OpenCVMat.data)
    {
      // copy current cvMat
      m_OpenCVMatMutex.lock();
      const cv::Mat input = m_OpenCVMat;
      m_OpenCVMatMutex.unlock();
      // convert cvMat to mitk::Image
      m_ImageMutex.lock();
      // now convert rgb image
      if ((input.depth() >= 0) && ((unsigned int)input.depth() == CV_8S) && (input.channels() == 1))
      {
        m_Image = ConvertCVMatToMitkImage< char, 2>(input);
      }
      else if (input.depth() == CV_8U && input.channels() == 1)
      {
        m_Image = ConvertCVMatToMitkImage< unsigned char, 2>(input);
      }
      else if (input.depth() == CV_8U && input.channels() == 3)
      {
        m_Image = ConvertCVMatToMitkImage< UCRGBPixelType, 2>(input);
      }
      else if (input.depth() == CV_16U && input.channels() == 1)
      {
        m_Image = ConvertCVMatToMitkImage< unsigned short, 2>(input);
      }
      else if (input.depth() == CV_16U && input.channels() == 3)
      {
        m_Image = ConvertCVMatToMitkImage< USRGBPixelType, 2>(input);
      }
      else if (input.depth() == CV_32F && input.channels() == 1)
      {
        m_Image = ConvertCVMatToMitkImage< float, 2>(input);
      }
      else if (input.depth() == CV_32F && input.channels() == 3)
      {
        m_Image = ConvertCVMatToMitkImage< FloatRGBPixelType, 2>(input);
      }
      else if (input.depth() == CV_64F && input.channels() == 1)
      {
        m_Image = ConvertCVMatToMitkImage< double, 2>(input);
      }
      else if (input.depth() == CV_64F && input.channels() == 3)
      {
        m_Image = ConvertCVMatToMitkImage< DoubleRGBPixelType, 2>(input);
      }
      else
      {
        MITK_WARN << "Unknown image depth and/or pixel type. Cannot convert OpenCV to MITK image.";
        return;
      }
      m_ImageMutex.unlock();
    }
    else
    {
      MITK_WARN << "Cannot start filter. OpenCV Image not set.";
      return;
    }
  }

  ImageSource::OutputImageType* OpenCVToMitkImageFilter::GetOutput()
  {
    return m_Image;
  }

  /********************************************
  * Converting from OpenCV image to ITK Image
  *********************************************/
  template <typename TPixel, unsigned int VImageDimension>
  Image::Pointer mitk::OpenCVToMitkImageFilter::ConvertCVMatToMitkImage(const cv::Mat input)
  {
    typedef itk::Image< TPixel, VImageDimension > ImageType;

    typename ImageType::Pointer output = itk::OpenCVImageBridge::CVMatToITKImage<ImageType>(input);
    Image::Pointer mitkImage = Image::New();
    mitkImage = GrabItkImageMemory(output);

    return mitkImage;
  }


  void OpenCVToMitkImageFilter::InsertOpenCVImageAsMitkTimeSlice(cv::Mat openCVImage, Image::Pointer mitkImage, int timeStep)
  {
    // convert it to an mitk::Image
    this->SetOpenCVMat(openCVImage);
    this->Modified();
    this->Update();

    //insert it as a timeSlice
    mitkImage->GetGeometry(timeStep)->SetSpacing(this->GetOutput()->GetGeometry()->GetSpacing());
    mitkImage->GetGeometry(timeStep)->SetOrigin(this->GetOutput()->GetGeometry()->GetOrigin());
    mitkImage->GetGeometry(timeStep)->SetIndexToWorldTransform(this->GetOutput()->GetGeometry()->GetIndexToWorldTransform());

    mitk::ImageReadAccessor readAccess(this->GetOutput());
    mitkImage->SetImportVolume(readAccess.GetData(), timeStep);

    mitkImage->Modified();
    mitkImage->Update();

    m_ImageMutex.lock();
    m_Image = mitkImage;
    m_ImageMutex.unlock();
  }

} // end namespace mitk
