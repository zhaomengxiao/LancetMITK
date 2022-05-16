/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkPointSetVtkMapper3D.h"
#include "mitkColorProperty.h"
#include "mitkDataNode.h"
#include "mitkPointSet.h"
#include "mitkProperties.h"
#include "mitkVtkPropRenderer.h"

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkVectorText.h>

#include <cstdlib>

#include <mitkPropertyObserver.h>
#include <vtk_glew.h>

// constructor LocalStorage
mitk::PointSetVtkMapper3D::LocalStorage::LocalStorage()
  : m_NumberOfSelectedAdded(0),
    m_NumberOfUnselectedAdded(0),
    m_NumberOfMarkedAdded(0),
    m_NumberOfHidedAdded(0)
{
  /// All point positions, already in world coordinates
  m_WorldPositions = vtkSmartPointer<vtkPoints>::New();
  /// All connections between two points (used for contour drawing)
  m_PointConnections = vtkSmartPointer<vtkCellArray>::New(); // m_PointConnections between points

  m_vtkSelectedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  m_vtkUnselectedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  m_vtkHidedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  m_vtkMarkedPointList = vtkSmartPointer<vtkAppendPolyData>::New();

  m_VtkSelectedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  m_VtkUnselectedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  m_VtkHidedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  m_VtkMarkedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

  // creating actors to be able to set transform
  m_SelectedActor = vtkSmartPointer<vtkActor>::New();
  m_UnselectedActor = vtkSmartPointer<vtkActor>::New();
  m_HidedActor = vtkSmartPointer<vtkActor>::New();
  m_MarkedActor = vtkSmartPointer<vtkActor>::New();
  m_ContourActor = vtkSmartPointer<vtkActor>::New();

  // propassembly
  m_PointsAssembly = vtkSmartPointer<vtkPropAssembly>::New();
}

// destructor LocalStorage
mitk::PointSetVtkMapper3D::LocalStorage::~LocalStorage()
{
}


const mitk::PointSet *mitk::PointSetVtkMapper3D::GetInput()
{
  return static_cast<const mitk::PointSet *>(GetDataNode()->GetData());
}

mitk::PointSetVtkMapper3D::PointSetVtkMapper3D()
    : 
    m_vtkTextList(nullptr),
    m_PointSize(1.0),
    m_ContourRadius(0.5)
{
}

mitk::PointSetVtkMapper3D::~PointSetVtkMapper3D()
{
}

//void mitk::PointSetVtkMapper3D::ReleaseGraphicsResources(vtkWindow *renWin)
//{
//  ls->m_PointsAssembly->ReleaseGraphicsResources(renWin);
//
//  ls->m_SelectedActor->ReleaseGraphicsResources(renWin);
//  ls->m_UnselectedActor->ReleaseGraphicsResources(renWin);
//  ls->m_ContourActor->ReleaseGraphicsResources(renWin);
//  ls->m_HidedActor->ReleaseGraphicsResources(renWin);
//  ls->m_MarkedActor->ReleaseGraphicsResources(renWin);
//}

void mitk::PointSetVtkMapper3D::ReleaseGraphicsResources(mitk::BaseRenderer *renderer)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
    ls->m_PointsAssembly->ReleaseGraphicsResources(renderer->GetRenderWindow());

    ls->m_SelectedActor->ReleaseGraphicsResources(renderer->GetRenderWindow());
    ls->m_UnselectedActor->ReleaseGraphicsResources(renderer->GetRenderWindow());
    ls->m_MarkedActor->ReleaseGraphicsResources(renderer->GetRenderWindow());
    ls->m_HidedActor->ReleaseGraphicsResources(renderer->GetRenderWindow());
    ls->m_ContourActor->ReleaseGraphicsResources(renderer->GetRenderWindow());
}

void mitk::PointSetVtkMapper3D::CreateVTKRenderObjects(mitk::BaseRenderer* renderer)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
    ls->m_PointsAssembly->VisibilityOn();

  ls->m_PointsAssembly->VisibilityOn();

  if (ls->m_PointsAssembly->GetParts()->IsItemPresent(ls->m_SelectedActor))
    ls->m_PointsAssembly->RemovePart(ls->m_SelectedActor);
  if (ls->m_PointsAssembly->GetParts()->IsItemPresent(ls->m_UnselectedActor))
    ls->m_PointsAssembly->RemovePart(ls->m_UnselectedActor);
  if (ls->m_PointsAssembly->GetParts()->IsItemPresent(ls->m_ContourActor))
    ls->m_PointsAssembly->RemovePart(ls->m_ContourActor);
  if (ls->m_PointsAssembly->GetParts()->IsItemPresent(ls->m_HidedActor))
      ls->m_PointsAssembly->RemovePart(ls->m_HidedActor);
  if (ls->m_PointsAssembly->GetParts()->IsItemPresent(ls->m_MarkedActor))
      ls->m_PointsAssembly->RemovePart(ls->m_MarkedActor);

  // exceptional displaying for PositionTracker -> MouseOrientationTool
  int mapperID;
  bool isInputDevice = false;
  if (this->GetDataNode()->GetBoolProperty("inputdevice", isInputDevice) && isInputDevice)
  {
    if (this->GetDataNode()->GetIntProperty("BaseRendererMapperID", mapperID) && mapperID == BaseRenderer::Standard3D)
      return; // The event for the PositionTracker came from the 3d widget and  not needs to be displayed
  }

  ls->m_vtkSelectedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  ls->m_vtkUnselectedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  ls->m_vtkMarkedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  ls->m_vtkHidedPointList = vtkSmartPointer<vtkAppendPolyData>::New();
  // get and update the PointSet
  mitk::PointSet::Pointer input = const_cast<mitk::PointSet *>(this->GetInput());

  /* only update the input data, if the property tells us to */
  bool update = true;
  this->GetDataNode()->GetBoolProperty("updateDataOnRender", update);
  if (update == true)
    input->Update();

  int timestep = this->GetTimestep();

  mitk::PointSet::DataType::Pointer itkPointSet = input->GetPointSet(timestep);

  if (itkPointSet.GetPointer() == nullptr)
  {
    ls->m_PointsAssembly->VisibilityOff();
    return;
  }

  // now fill selected and unselected pointList
  // get size of Points in Property
  m_PointSize = 2;
  mitk::FloatProperty::Pointer pointSizeProp =
    dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetProperty("pointsize"));
  if (pointSizeProp.IsNotNull())
    m_PointSize = pointSizeProp->GetValue();

  // get the property for creating a label onto every point only once
  bool showLabel = true;
  this->GetDataNode()->GetBoolProperty("show label", showLabel);
  const char *pointLabel = nullptr;
  if (showLabel)
  {
    if (dynamic_cast<mitk::StringProperty *>(this->GetDataNode()->GetPropertyList()->GetProperty("label")) != nullptr)
      pointLabel =
        dynamic_cast<mitk::StringProperty *>(this->GetDataNode()->GetPropertyList()->GetProperty("label"))->GetValue();
    else
      showLabel = false;
  }

  // whether or not to creat a "contour" - connecting lines between all the points
  int nbPoints = itkPointSet->GetPointData()->Size();
  bool makeContour = false;
  this->GetDataNode()->GetBoolProperty("show contour", makeContour);

  bool closeContour = false;
  this->GetDataNode()->GetBoolProperty("close contour", closeContour);
  int contourPointLimit = 0; // NO contour
  if (makeContour)
  {
    if (closeContour)
      contourPointLimit = nbPoints;
    else
      contourPointLimit = nbPoints - 1;
  }

  // build list of all positions for later transform in one go
  mitk::PointSet::PointsContainer::Iterator pointsIter;
  int ptIdx;

  ls->m_NumberOfSelectedAdded = 0;
  ls->m_NumberOfUnselectedAdded = 0;
  ls->m_NumberOfHidedAdded = 0;
  ls->m_NumberOfMarkedAdded = 0;
  vtkSmartPointer<vtkPoints> localPoints = vtkSmartPointer<vtkPoints>::New();
  ls->m_WorldPositions = vtkSmartPointer<vtkPoints>::New();
  ls->m_PointConnections = vtkSmartPointer<vtkCellArray>::New(); // m_PointConnections between points
  for (ptIdx = 0, pointsIter = itkPointSet->GetPoints()->Begin(); pointsIter != itkPointSet->GetPoints()->End();
       pointsIter++, ptIdx++)
  {
    itk::Point<float> currentPoint = pointsIter->Value();
    localPoints->InsertPoint(ptIdx, currentPoint[0], currentPoint[1], currentPoint[2]);

    if (makeContour && ptIdx < contourPointLimit)
    {
      vtkIdType cell[2] = {(ptIdx + 1) % nbPoints, ptIdx};
      ls->m_PointConnections->InsertNextCell(2, cell);
    }
  }

  vtkSmartPointer<vtkLinearTransform> vtktransform = this->GetDataNode()->GetVtkTransform(this->GetTimestep());
  vtktransform->TransformPoints(localPoints, ls->m_WorldPositions);

  // create contour
  if (makeContour)
  {
      this->CreateContour(ls->m_WorldPositions, ls->m_PointConnections, renderer);
  }

  // check if the list for the PointDataContainer is the same size as the PointsContainer. Is not, then the points were
  // inserted manually and can not be visualized according to the PointData (selected/unselected)
  bool pointDataBroken = (itkPointSet->GetPointData()->Size() != itkPointSet->GetPoints()->Size());

  // now add an object for each point in data
  mitk::PointSet::PointDataContainer::Iterator pointDataIter = itkPointSet->GetPointData()->Begin();
  for (ptIdx = 0; ptIdx < nbPoints; ++ptIdx) // pointDataIter moved at end of loop
  {
    double currentPoint[3];
    ls->m_WorldPositions->GetPoint(ptIdx, currentPoint);
    vtkSmartPointer<vtkPolyDataAlgorithm> source;

    // check for the pointtype in data and decide which geom-object to take and then add to the selected or unselected
    // list
    int pointType;
    if (itkPointSet->GetPointData()->size() == 0 || pointDataBroken)
      pointType = mitk::PTUNDEFINED;
    else
      pointType = pointDataIter.Value().pointSpec;

    switch (pointType)
    {
      case mitk::PTUNDEFINED:
      {
        vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetRadius(m_PointSize / 2.0f);
        sphere->SetCenter(currentPoint);
        // sphere->SetCenter(pointsIter.Value()[0],pointsIter.Value()[1],pointsIter.Value()[2]);

        // MouseOrientation Tool (PositionTracker)
        if (isInputDevice)
        {
          sphere->SetThetaResolution(10);
          sphere->SetPhiResolution(10);
        }
        else
        {
          sphere->SetThetaResolution(20);
          sphere->SetPhiResolution(20);
        }
        source = sphere;
      }
      break;
      case mitk::PTSTART:
      {
        vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
        cube->SetXLength(m_PointSize / 2);
        cube->SetYLength(m_PointSize / 2);
        cube->SetZLength(m_PointSize / 2);
        cube->SetCenter(currentPoint);
        source = cube;
      }
      break;
      case mitk::PTCORNER:
      {
        vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
        cone->SetRadius(m_PointSize / 2.0f);
        cone->SetCenter(currentPoint);
        cone->SetResolution(20);
        source = cone;
      }
      break;
      case mitk::PTEDGE:
      {
        vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
        cylinder->SetRadius(m_PointSize / 2.0f);
        cylinder->SetCenter(currentPoint);
        cylinder->SetResolution(20);
        source = cylinder;
      }
      break;
      case mitk::PTEND:
      {
        vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetRadius(m_PointSize / 2.0f);
        // no SetCenter?? this functionality should be explained!
        // otherwise: join with default block!
        sphere->SetThetaResolution(20);
        sphere->SetPhiResolution(20);
        source = sphere;
      }
      break;

      // case PBIG:
      // {
      //     vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
      //     sphere->SetRadius((m_PointSize+1) / 2.0f);
      //     sphere->SetCenter(currentPoint);
      //     sphere->SetThetaResolution(20);
      //     sphere->SetPhiResolution(20);
      //     source = sphere;
      // }
      // break;
      // case PSMALL:
      // {
      //     vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
      //     ScalarType pointsize = 1;
      //     if (m_PointSize>1)
      //     {
      //         pointsize = m_PointSize - 1;
      //     }
      //     else
      //     {
      //         pointsize = m_PointSize;
      //     }
      //     sphere->SetRadius(pointsize / 2.0f);
      //     sphere->SetCenter(currentPoint);
      //     sphere->SetThetaResolution(20);
      //     sphere->SetPhiResolution(20);
      //     source = sphere;
      // }
      //break;
      default:
      {
        vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetRadius(m_PointSize / 2.0f);
        sphere->SetCenter(currentPoint);
        sphere->SetThetaResolution(20);
        sphere->SetPhiResolution(20);
        source = sphere;
      }
      break;
    }
   
    if (pointDataIter.Value().selected && !pointDataBroken)
    {
        ls->m_vtkSelectedPointList->AddInputConnection(source->GetOutputPort());
      ++ls->m_NumberOfSelectedAdded;
    }
    // else if (pointDataIter.Value().hided && !pointDataBroken)
    // {
    //     ls->m_vtkHidedPointList->AddInputConnection(source->GetOutputPort());
    //     ++ls->m_NumberOfHidedAdded;
    // }
    // else if (pointDataIter.Value().marked && !pointDataBroken)
    // {
    //     ls->m_vtkMarkedPointList->AddInputConnection(source->GetOutputPort());
    //     ++ls->m_NumberOfMarkedAdded;
    // }
    else
    {
        ls->m_vtkUnselectedPointList->AddInputConnection(source->GetOutputPort());
      ++ls->m_NumberOfUnselectedAdded;
    }
    if (showLabel)
    {
      char buffer[20];
      std::string l = pointLabel;
      if (input->GetSize() > 1)
      {
        sprintf(buffer, "%d", ptIdx + 1);
        l.append(buffer);
      }
      // Define the text for the label
      vtkSmartPointer<vtkVectorText> label = vtkSmartPointer<vtkVectorText>::New();
      label->SetText(l.c_str());

      //# Set up a transform to move the label to a new position.
      vtkSmartPointer<vtkTransform> aLabelTransform = vtkSmartPointer<vtkTransform>::New();
      aLabelTransform->Identity();
      aLabelTransform->Translate(currentPoint[0] + 2, currentPoint[1] + 2, currentPoint[2]);
      aLabelTransform->Scale(5.7, 5.7, 5.7);

      //# Move the label to a new position.
      vtkSmartPointer<vtkTransformPolyDataFilter> labelTransform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      labelTransform->SetTransform(aLabelTransform);
      labelTransform->SetInputConnection(label->GetOutputPort());

      // add it to the wright PointList
      if (pointType)
      {
          ls->m_vtkSelectedPointList->AddInputConnection(labelTransform->GetOutputPort());
        ++ls->m_NumberOfSelectedAdded;
      }
      else
      {
          ls->m_vtkUnselectedPointList->AddInputConnection(labelTransform->GetOutputPort());
        ++ls->m_NumberOfUnselectedAdded;
      }
    }

    if (pointDataIter != itkPointSet->GetPointData()->End())
      pointDataIter++;
  } // end FOR

  // now according to number of elements added to selected or unselected, build up the rendering pipeline
  if (ls->m_NumberOfSelectedAdded > 0)
  {
      ls->m_VtkSelectedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      ls->m_VtkSelectedPolyDataMapper->SetInputConnection(ls->m_vtkSelectedPointList->GetOutputPort());

    // create a new instance of the actor
    ls->m_SelectedActor = vtkSmartPointer<vtkActor>::New();

    ls->m_SelectedActor->SetMapper(ls->m_VtkSelectedPolyDataMapper);
    ls->m_PointsAssembly->AddPart(ls->m_SelectedActor);
  }

  if (ls->m_NumberOfUnselectedAdded > 0)
  {
      ls->m_VtkUnselectedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      ls->m_VtkUnselectedPolyDataMapper->SetInputConnection(ls->m_vtkUnselectedPointList->GetOutputPort());

    // create a new instance of the actor
    ls->m_UnselectedActor = vtkSmartPointer<vtkActor>::New();

    ls->m_UnselectedActor->SetMapper(ls->m_VtkUnselectedPolyDataMapper);
    ls->m_PointsAssembly->AddPart(ls->m_UnselectedActor);
  }

  if (ls->m_NumberOfHidedAdded > 0)
  {
      ls->m_VtkHidedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      ls->m_VtkHidedPolyDataMapper->SetInputConnection(ls->m_vtkHidedPointList->GetOutputPort());

      // create a new instance of the actor
      ls->m_HidedActor = vtkSmartPointer<vtkActor>::New();

      ls->m_HidedActor->SetMapper(ls->m_VtkHidedPolyDataMapper);
      ls->m_PointsAssembly->AddPart(ls->m_HidedActor);
  }

  if (ls->m_NumberOfMarkedAdded > 0)
  {
      ls->m_VtkMarkedPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      ls->m_VtkMarkedPolyDataMapper->SetInputConnection(ls->m_vtkMarkedPointList->GetOutputPort());

      // create a new instance of the actor
      ls->m_MarkedActor = vtkSmartPointer<vtkActor>::New();

      ls->m_MarkedActor->SetMapper(ls->m_VtkMarkedPolyDataMapper);
      ls->m_PointsAssembly->AddPart(ls->m_MarkedActor);
  }
}

void mitk::PointSetVtkMapper3D::GenerateDataForRenderer(mitk::BaseRenderer *renderer)
{
  LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  if (!visible)
  {
    ls->m_UnselectedActor->VisibilityOff();
    ls->m_SelectedActor->VisibilityOff();
    ls->m_ContourActor->VisibilityOff();
    ls->m_MarkedActor->VisibilityOff();
    ls->m_HidedActor->VisibilityOff();
    return;
  }

  // create new vtk render objects (e.g. sphere for a point)
  //BaseLocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  bool needGenerateData = ls->IsGenerateDataRequired(renderer, this, GetDataNode());

  if (!needGenerateData)
  {
    if (this->GetDataNode()->GetPropertyList()->GetMTime() > ls->GetLastGenerateDataTime() ||
        this->GetDataNode()->GetPropertyList(renderer)->GetMTime() > ls->GetLastGenerateDataTime())
    {
      needGenerateData = true;
    }
  }

  if (needGenerateData)
  {
    this->CreateVTKRenderObjects(renderer);
    ls->UpdateGenerateDataTime();
  }

  this->ApplyAllProperties(renderer, ls->m_ContourActor);

  bool showPoints = true;
  this->GetDataNode()->GetBoolProperty("show points", showPoints);

  ls->m_UnselectedActor->SetVisibility(showPoints);
  ls->m_SelectedActor->SetVisibility(showPoints);
  ls->m_MarkedActor->SetVisibility(showPoints);
  ls->m_HidedActor->SetVisibility(false);

  if (false && dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetProperty("opacity")) != nullptr)
  {
    mitk::FloatProperty::Pointer pointOpacity =
      dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetProperty("opacity"));
    float opacity = pointOpacity->GetValue();
    ls->m_ContourActor->GetProperty()->SetOpacity(opacity);
    ls->m_UnselectedActor->GetProperty()->SetOpacity(opacity);
    ls->m_SelectedActor->GetProperty()->SetOpacity(opacity);
  }

  bool showContour = false;
  this->GetDataNode()->GetBoolProperty("show contour", showContour);
  ls->m_ContourActor->SetVisibility(showContour);
}

void mitk::PointSetVtkMapper3D::ResetMapper(BaseRenderer * renderer)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
  ls->m_PointsAssembly->VisibilityOff();
}

vtkProp *mitk::PointSetVtkMapper3D::GetVtkProp(mitk::BaseRenderer * renderer)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
  return ls->m_PointsAssembly;
}

void mitk::PointSetVtkMapper3D::UpdateVtkTransform(mitk::BaseRenderer * /*renderer*/)
{
}

void mitk::PointSetVtkMapper3D::ApplyAllProperties(mitk::BaseRenderer *renderer, vtkActor *actor)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
  Superclass::ApplyColorAndOpacityProperties(renderer, actor);
  // check for color props and use it for rendering of selected/unselected points and contour
  // due to different params in VTK (double/float) we have to convert!

  // vars to convert to
  double unselectedColor[4] = {1.0f, 1.0f, 0.0f, 1.0f}; // yellow
  double selectedColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};   // red
  double contourColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};    // red
  double markedColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };    // green

  // different types for color!!!
  mitk::Color tmpColor;
  double opacity = 1.0;

  // check if there is an unselected property
  if (dynamic_cast<mitk::ColorProperty *>(
        this->GetDataNode()->GetPropertyList(renderer)->GetProperty("unselectedcolor")) != nullptr)
  {
    tmpColor = dynamic_cast<mitk::ColorProperty *>(
                 this->GetDataNode()->GetPropertyList(renderer)->GetProperty("unselectedcolor"))
                 ->GetValue();
    unselectedColor[0] = tmpColor[0];
    unselectedColor[1] = tmpColor[1];
    unselectedColor[2] = tmpColor[2];
    unselectedColor[3] = 1.0f; //!!define a new ColorProp to be able to pass alpha value
  }
  else if (dynamic_cast<mitk::ColorProperty *>(
             this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("unselectedcolor")) != nullptr)
  {
    tmpColor =
      dynamic_cast<mitk::ColorProperty *>(this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("unselectedcolor"))
        ->GetValue();
    unselectedColor[0] = tmpColor[0];
    unselectedColor[1] = tmpColor[1];
    unselectedColor[2] = tmpColor[2];
    unselectedColor[3] = 1.0f; //!!define a new ColorProp to be able to pass alpha value
  }
  else
  {
    // check if the node has a color
    float unselectedColorTMP[4] = {1.0f, 1.0f, 0.0f, 1.0f}; // yellow
    m_DataNode->GetColor(unselectedColorTMP, nullptr);
    unselectedColor[0] = unselectedColorTMP[0];
    unselectedColor[1] = unselectedColorTMP[1];
    unselectedColor[2] = unselectedColorTMP[2];
    // unselectedColor[3] stays 1.0f
  }

  // get selected property
  if (dynamic_cast<mitk::ColorProperty *>(
        this->GetDataNode()->GetPropertyList(renderer)->GetProperty("selectedcolor")) != nullptr)
  {
    tmpColor =
      dynamic_cast<mitk::ColorProperty *>(this->GetDataNode()->GetPropertyList(renderer)->GetProperty("selectedcolor"))
        ->GetValue();
    selectedColor[0] = tmpColor[0];
    selectedColor[1] = tmpColor[1];
    selectedColor[2] = tmpColor[2];
    selectedColor[3] = 1.0f;
  }
  else if (dynamic_cast<mitk::ColorProperty *>(
             this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("selectedcolor")) != nullptr)
  {
    tmpColor =
      dynamic_cast<mitk::ColorProperty *>(this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("selectedcolor"))
        ->GetValue();
    selectedColor[0] = tmpColor[0];
    selectedColor[1] = tmpColor[1];
    selectedColor[2] = tmpColor[2];
    selectedColor[3] = 1.0f;
  }

  // get contour property
  if (dynamic_cast<mitk::ColorProperty *>(
        this->GetDataNode()->GetPropertyList(renderer)->GetProperty("contourcolor")) != nullptr)
  {
    tmpColor =
      dynamic_cast<mitk::ColorProperty *>(this->GetDataNode()->GetPropertyList(renderer)->GetProperty("contourcolor"))
        ->GetValue();
    contourColor[0] = tmpColor[0];
    contourColor[1] = tmpColor[1];
    contourColor[2] = tmpColor[2];
    contourColor[3] = 1.0f;
  }
  else if (dynamic_cast<mitk::ColorProperty *>(
             this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("contourcolor")) != nullptr)
  {
    tmpColor =
      dynamic_cast<mitk::ColorProperty *>(this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("contourcolor"))
        ->GetValue();
    contourColor[0] = tmpColor[0];
    contourColor[1] = tmpColor[1];
    contourColor[2] = tmpColor[2];
    contourColor[3] = 1.0f;
  }

  if (dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetPropertyList(renderer)->GetProperty("opacity")) !=
      nullptr)
  {
    mitk::FloatProperty::Pointer pointOpacity =
      dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetPropertyList(renderer)->GetProperty("opacity"));
    opacity = pointOpacity->GetValue();
  }
  else if (dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("opacity")) !=
           nullptr)
  {
    mitk::FloatProperty::Pointer pointOpacity =
      dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetPropertyList(nullptr)->GetProperty("opacity"));
    opacity = pointOpacity->GetValue();
  }
  // finished color / opacity fishing!

  // check if a contour shall be drawn
  bool showContour = false;
  this->GetDataNode()->GetBoolProperty("show contour", showContour, renderer);
  if (showContour && (ls->m_ContourActor != nullptr))
  {
    this->CreateContour(ls->m_WorldPositions, ls->m_PointConnections,renderer);
    ls->m_ContourActor->GetProperty()->SetColor(contourColor);
    ls->m_ContourActor->GetProperty()->SetOpacity(opacity);
  }

  ls->m_SelectedActor->GetProperty()->SetColor(selectedColor);
  ls->m_SelectedActor->GetProperty()->SetOpacity(opacity);

  ls->m_UnselectedActor->GetProperty()->SetColor(unselectedColor);
  ls->m_UnselectedActor->GetProperty()->SetOpacity(opacity);

  ls->m_MarkedActor->GetProperty()->SetColor(markedColor);
  ls->m_MarkedActor->GetProperty()->SetOpacity(opacity);
}

void mitk::PointSetVtkMapper3D::CreateContour(vtkPoints* points, vtkCellArray* PointConnections, mitk::BaseRenderer* renderer)
{
    LocalStorage* ls = m_LSH.GetLocalStorage(renderer);
  vtkSmartPointer<vtkAppendPolyData> vtkContourPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
  vtkSmartPointer<vtkPolyDataMapper> vtkContourPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

  vtkSmartPointer<vtkPolyData> contour = vtkSmartPointer<vtkPolyData>::New();
  contour->SetPoints(points);
  contour->SetLines(PointConnections);

  vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
  tubeFilter->SetNumberOfSides(12);
  tubeFilter->SetInputData(contour);

  // check for property contoursize.
  m_ContourRadius = 0.5;
  mitk::FloatProperty::Pointer contourSizeProp =
    dynamic_cast<mitk::FloatProperty *>(this->GetDataNode()->GetProperty("contoursize"));

  if (contourSizeProp.IsNotNull())
    m_ContourRadius = contourSizeProp->GetValue();

  tubeFilter->SetRadius(m_ContourRadius);
  tubeFilter->Update();

  // add to pipeline
  vtkContourPolyData->AddInputConnection(tubeFilter->GetOutputPort());
  vtkContourPolyDataMapper->SetInputConnection(vtkContourPolyData->GetOutputPort());

  ls->m_ContourActor->SetMapper(vtkContourPolyDataMapper);
  ls->m_PointsAssembly->AddPart(ls->m_ContourActor);
}

void mitk::PointSetVtkMapper3D::SetDefaultProperties(mitk::DataNode *node, mitk::BaseRenderer *renderer, bool overwrite)
{
  node->AddProperty("line width", mitk::IntProperty::New(2), renderer, overwrite);
  node->AddProperty("pointsize", mitk::FloatProperty::New(1.0), renderer, overwrite);
  node->AddProperty("selectedcolor", mitk::ColorProperty::New(1.0f, 0.0f, 0.0f), renderer, overwrite); // red
  node->AddProperty("color", mitk::ColorProperty::New(1.0f, 1.0f, 0.0f), renderer, overwrite);         // yellow
  node->AddProperty("opacity", mitk::FloatProperty::New(1.0f), renderer, overwrite);
  node->AddProperty("show contour", mitk::BoolProperty::New(false), renderer, overwrite);
  node->AddProperty("close contour", mitk::BoolProperty::New(false), renderer, overwrite);
  node->AddProperty("contourcolor", mitk::ColorProperty::New(1.0f, 0.0f, 0.0f), renderer, overwrite);
  node->AddProperty("contoursize", mitk::FloatProperty::New(0.5), renderer, overwrite);
  node->AddProperty("show points", mitk::BoolProperty::New(true), renderer, overwrite);
  node->AddProperty("updateDataOnRender", mitk::BoolProperty::New(true), renderer, overwrite);
  Superclass::SetDefaultProperties(node, renderer, overwrite);
}
