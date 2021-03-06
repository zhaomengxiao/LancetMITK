/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkSurfaceVtkIO.h"

#include "mitkSurface.h"
#include <mitkUtf8Util.h>

#include <vtkLinearTransform.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>

#include <itksys/SystemTools.hxx>

namespace mitk
{
  SurfaceVtkIO::SurfaceVtkIO(const std::string &baseDataType,
                             const CustomMimeType &mimeType,
                             const std::string &description)
    : AbstractFileIO(baseDataType, mimeType, description)
  {
  }

  vtkSmartPointer<vtkPolyData> SurfaceVtkIO::GetPolyData(unsigned int t, std::string &fileName)
  {
    const auto *input = dynamic_cast<const Surface *>(this->GetInput());

    vtkSmartPointer<vtkTransformPolyDataFilter> transformPolyData = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

    // surfaces do not have to exist in all timesteps; therefor, only write valid surfaces
    if (input->GetVtkPolyData(t) == nullptr)
      return vtkSmartPointer<vtkPolyData>();

    std::string baseName = this->GetOutputLocation();
    std::string extension = Utf8Util::Utf8ToLocal8Bit(itksys::SystemTools::GetFilenameExtension(Utf8Util::Local8BitToUtf8(baseName)));
    if (!extension.empty())
    {
      baseName = baseName.substr(0, baseName.size() - extension.size());
    }

    std::ostringstream ss;
    ss.imbue(::std::locale::classic());

    BaseGeometry *geometry = input->GetGeometry(t);
    if (input->GetTimeGeometry()->IsValidTimeStep(t))
    {
      if (input->GetTimeGeometry()->CountTimeSteps() > 1)
      {
        const TimeBounds &timebounds = input->GetTimeGeometry()->GetTimeBounds(t);
        ss << baseName << "_S" << std::setprecision(0) << timebounds[0] << "_E" << std::setprecision(0) << timebounds[1]
           << "_T" << t << extension;
      }
      else
      {
        // use the original file name
        ss << this->GetOutputLocation();
      }
    }
    else
    {
      MITK_WARN << "Error on write: TimeGeometry invalid of surface " << fileName << ".";
      return vtkSmartPointer<vtkPolyData>();
    }

    fileName = ss.str();

    transformPolyData->SetInputData(input->GetVtkPolyData(t));
    transformPolyData->SetTransform(geometry->GetVtkTransform());
    transformPolyData->UpdateWholeExtent();

    vtkSmartPointer<vtkPolyData> polyData = transformPolyData->GetOutput();
    return polyData;
  }

  IFileIO::ConfidenceLevel SurfaceVtkIO::GetWriterConfidenceLevel() const
  {
    if (AbstractFileIO::GetWriterConfidenceLevel() == Unsupported)
      return Unsupported;
    if (this->GetInput()->GetTimeGeometry()->CountTimeSteps() > 1)
    {
      // The VTK formats don't support multiple time points.
      // During writing, we write each time step into a separate file.
      // For output streams, we only write the first time-step and print a warning.
      return PartiallySupported;
    }
    return Supported;
  }
}
