/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// Blueberry
#include <berryIWorkbenchWindow.h>

//Mitk
#include <mitkDataNode.h>
#include <mitkNodePredicateDataType.h>
#include <mitkImageGenerator.h>
#include <leastsquaresfit.h>

// Qmitk
#include "QmitkIGTFiducialRegistration.h"
#include <QmitkRenderWindow.h>

// Qt
#include <QSettings>


// ↓↓↓↓↓  Slots  ↓↓↓↓↓
void QmitkIGTFiducialRegistration::ConfirmProbe_roboRegis()
{
  int toolID = m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedToolID();

  m_probeNDpointer_roboRegis =
    m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedNavigationDataSource()->GetOutput(toolID);

  m_Controls.label_probe_roboRegis->setText(
    m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::ConfirmRoboDrf_roboRegis()
{
  int toolID = m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedToolID();

  m_roboDrfNDpointer_roboRegis =
    m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedNavigationDataSource()->GetOutput(toolID);

  m_Controls.label_roboDrf_roboRegis->setText(
    m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::CollectPointAroundAxis1_roboRegis()
{
  auto tmpNode = GetDataStorage()->GetNamedNode("Points around axis 1");
  if (tmpNode == nullptr)
  {
    auto axis1PointSet = mitk::PointSet::New();
    auto axis1PointSetNode = mitk::DataNode::New();
    axis1PointSetNode->SetName("Points around axis 1");
    axis1PointSetNode->SetData(axis1PointSet);
    GetDataStorage()->Add(axis1PointSetNode);
        
  }

  auto collectedPoints = dynamic_cast<mitk::PointSet *>(GetDataStorage()->GetNamedNode("Points around axis 1")->GetData());
  
  vtkNew<vtkMatrix4x4> matrixNdiToRoboDrf;
  matrixNdiToRoboDrf->DeepCopy(getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis));
  
  mitk::Point3D tmpPoint;
  tmpPoint[0] = matrixNdiToRoboDrf->GetElement(0, 3);
  tmpPoint[1] = matrixNdiToRoboDrf->GetElement(1, 3);
  tmpPoint[2] = matrixNdiToRoboDrf->GetElement(2, 3);

  int poinSetSize = collectedPoints->GetSize();

  if (poinSetSize == 0)
  {
    auto ndiToInitialPostureMatrix = getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis);

    for (int i{0}; i < 4; i++)
    {
      for (int j{0}; j < 4; j++)
      {
        m_matrix_ndiToInitialPosture[4 * i + j] = ndiToInitialPostureMatrix->GetElement(i, j);
      }
    }
    m_Controls.textBrowser_roboRegis->append("Initial Posture captured");
  }

  collectedPoints->InsertPoint(tmpPoint);

  m_Controls.lineEdit_axis1PointNum_roboRegis->setText(QString::number(collectedPoints->GetSize()));

}

void QmitkIGTFiducialRegistration::CollectPointAroundAxis2_roboRegis()
{
  auto tmpNode = GetDataStorage()->GetNamedNode("Points around axis 2");
  if (tmpNode == nullptr)
  {
    auto axis2PointSet = mitk::PointSet::New();
    auto axis2PointSetNode = mitk::DataNode::New();
    axis2PointSetNode->SetName("Points around axis 2");
    axis2PointSetNode->SetData(axis2PointSet);
    GetDataStorage()->Add(axis2PointSetNode);
  }

  auto collectedPoints =
    dynamic_cast<mitk::PointSet *>(GetDataStorage()->GetNamedNode("Points around axis 2")->GetData());

  vtkNew<vtkMatrix4x4> matrixNdiToRoboDrf;
  matrixNdiToRoboDrf->DeepCopy(getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis));

  mitk::Point3D tmpPoint;
  tmpPoint[0] = matrixNdiToRoboDrf->GetElement(0, 3);
  tmpPoint[1] = matrixNdiToRoboDrf->GetElement(1, 3);
  tmpPoint[2] = matrixNdiToRoboDrf->GetElement(2, 3);

  collectedPoints->InsertPoint(tmpPoint);

  m_Controls.lineEdit_axis2PointNum_roboRegis->setText(QString::number(collectedPoints->GetSize()));
}

void QmitkIGTFiducialRegistration::RemovePointAxis1_roboRegis()
{
  auto tmpNode = GetDataStorage()->GetNamedNode("Points around axis 1");
  if (tmpNode == nullptr)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet already empty ! ");
    return;
  }

  auto collectedPoints =
    dynamic_cast<mitk::PointSet *>(GetDataStorage()->GetNamedNode("Points around axis 1")->GetData());
  
  int pointNum = collectedPoints->GetSize();
  if (pointNum == 0)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet already empty ! ");
    return;
  }

  auto previousPoint = collectedPoints->GetPoint(pointNum - 1);

  collectedPoints->RemovePointAtEnd();

  m_Controls.lineEdit_axis1PointNum_roboRegis->setText(QString::number(collectedPoints->GetSize()));
}

void QmitkIGTFiducialRegistration::RemovePointAxis2_roboRegis()
{
  auto tmpNode = GetDataStorage()->GetNamedNode("Points around axis 2");
  if (tmpNode == nullptr)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet already empty ! ");
    return;
  }

  auto collectedPoints =
    dynamic_cast<mitk::PointSet *>(GetDataStorage()->GetNamedNode("Points around axis 2")->GetData());

  int pointNum = collectedPoints->GetSize();
  if (pointNum == 0)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet already empty ! ");
    return;
  }

  auto previousPoint = collectedPoints->GetPoint(pointNum - 1);

  collectedPoints->RemovePointAtEnd();

  m_Controls.lineEdit_axis2PointNum_roboRegis->setText(QString::number(collectedPoints->GetSize()));
}


void QmitkIGTFiducialRegistration::GetMatrixDrfToFlange_roboRegis()
{
  if (GetMatrixFlangeToRot() && GetMatrixNdiToRot())
  {
    vtkNew<vtkMatrix4x4> vtkMatrixFlangeToRot;
    vtkMatrixFlangeToRot->DeepCopy(m_matrix_flangeToRot_roboRegis);

    vtkNew<vtkMatrix4x4> vtkMatrixRotToNdi;
    vtkMatrixRotToNdi->DeepCopy(m_matrix_ndiToRot_roboRegis);
    vtkMatrixRotToNdi->Invert();

    vtkNew<vtkMatrix4x4> vtkMatrixNdiToInitialPosture;
    vtkMatrixNdiToInitialPosture->DeepCopy(m_matrix_ndiToInitialPosture);

    Eigen::Matrix4d infoMatrix{m_matrix_ndiToInitialPosture};
    infoMatrix.transposeInPlace();
    MITK_INFO << "matrix NDI to DRF:" << endl << infoMatrix;

    vtkNew<vtkTransform> transFlangeToDrf;
    transFlangeToDrf->Identity();
    transFlangeToDrf->PostMultiply();
    transFlangeToDrf->Concatenate(vtkMatrixNdiToInitialPosture);
    transFlangeToDrf->Concatenate(vtkMatrixRotToNdi);
    transFlangeToDrf->Concatenate(vtkMatrixFlangeToRot);
    transFlangeToDrf->Update();

    // auto matrixDrfToFlange = transFlangeToDrf->GetMatrix();
    // matrixDrfToFlange->Invert();
    //
    // auto tmpArray = matrixDrfToFlange->GetData();

    auto matrixFlangeToDrf = transFlangeToDrf->GetMatrix();
    
    auto tmpArray = matrixFlangeToDrf->GetData();

    Eigen::Matrix4d eigenMatrixFlangeToDrf{tmpArray};

    for (int i{0}; i < 16; i++)
    {
      m_matrix_flangeToDrf_roboRegis[i] = eigenMatrixFlangeToDrf(i);
    }

    eigenMatrixFlangeToDrf.transposeInPlace();

    MITK_INFO << "matrix TCP (flange) to DRF:" << endl << eigenMatrixFlangeToDrf;

    m_Controls.textBrowser_roboRegis->append("TCP (flange) to DRF matrix has been printed in console");

  }else
  {
    m_Controls.textBrowser_roboRegis->append("Points are not ready!");
  }
}


// ↑↑↑↑↑  Slots  ↑↑↑↑↑ 





// ↓↓↓↓↓  functions  ↓↓↓↓↓
bool QmitkIGTFiducialRegistration::GetMatrixFlangeToRot()
{
  m_axis1Direction[0] = m_Controls.lineEdit_axis1_x_roboRegis->text().toDouble();
  m_axis1Direction[1] = m_Controls.lineEdit_axis1_y_roboRegis->text().toDouble();
  m_axis1Direction[2] = m_Controls.lineEdit_axis1_z_roboRegis->text().toDouble();

  m_axis2Direction[0] = m_Controls.lineEdit_axis2_x_roboRegis->text().toDouble();
  m_axis2Direction[1] = m_Controls.lineEdit_axis2_y_roboRegis->text().toDouble();
  m_axis2Direction[2] = m_Controls.lineEdit_axis2_z_roboRegis->text().toDouble();

  // In TCP (flange) space
  Eigen::Vector3d axis1_tcp;
  axis1_tcp[0] = m_axis1Direction[0];
  axis1_tcp[1] = m_axis1Direction[1];
  axis1_tcp[2] = m_axis1Direction[2];
  axis1_tcp.normalize();

  Eigen::Vector3d axis2_tcp;
  axis2_tcp[0] = m_axis2Direction[0];
  axis2_tcp[1] = m_axis2Direction[1];
  axis2_tcp[2] = m_axis2Direction[2];
  axis2_tcp.normalize();

  Eigen::Vector3d x = axis1_tcp;

  Eigen::Vector3d y = axis1_tcp.cross(axis2_tcp);
  y.normalize();
  
  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;

  MITK_INFO << "matrix TCP to ROT:" << endl << matrix;

  matrix.transposeInPlace();

  for (int i{0}; i < 16; i++)
  {
    m_matrix_flangeToRot_roboRegis[i] = matrix(i);
  }

  return true;
}

bool QmitkIGTFiducialRegistration::GetMatrixNdiToRot()
{

  // check if the nodes exit and if the pointsets are empty
  auto tmpNode = GetDataStorage()->GetNamedNode("Points around axis 1");
  if (tmpNode == nullptr)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet around axis 1 empty ! ");
    return false;
  }

  auto tmpNode2 = GetDataStorage()->GetNamedNode("Points around axis 2");
  if (tmpNode == nullptr)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet around axis 2 empty ! ");
    return false;
  }

  auto axis1PointSet = dynamic_cast<mitk::PointSet *>(tmpNode->GetData());
  auto axis2PointSet = dynamic_cast<mitk::PointSet *>(tmpNode2->GetData());

  int axis1PointNum = axis1PointSet->GetSize();
  int axis2PointNum = axis2PointSet->GetSize();

  if (axis1PointNum < 3)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet around axis 1 not sufficient ! ");
    return false;
  }

  if (axis2PointNum < 3)
  {
    m_Controls.textBrowser_roboRegis->append("PointSet around axis 2 not sufficient ! ");
    return false;
  }

  // Get axis 1 and axis 2 in NDI space
  double axis1[3];
  double axis2[3];

  GetAxis_roboRegis(axis1PointSet, axis1);
  GetAxis_roboRegis(axis2PointSet, axis2);

  // Merge the 2 pointSets to fit a sphere
  auto spherePoints = mitk::PointSet::New();
  double sphereCenter[3];

  for (int i{0}; i < (axis1PointNum + axis2PointNum); i++)
  {
    if ( i < axis1PointNum)
    {
      spherePoints->InsertPoint(axis1PointSet->GetPoint(i));
    }else
    {
      spherePoints->InsertPoint(axis2PointSet->GetPoint(i - axis1PointNum));
    }
  }

  GetFlageOriginInNdi_roboRegis(spherePoints, sphereCenter);

  Eigen::Vector3d translation;
  translation[0] = sphereCenter[0];
  translation[1] = sphereCenter[1];
  translation[2] = sphereCenter[2];


  Eigen::Vector3d x{axis1[0], axis1[1], axis1[2]};
  Eigen::Vector3d axis2Vector{axis2[0], axis2[1], axis2[2]};

  Eigen::Vector3d y = x.cross(axis2Vector);
  y.normalize();

  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;
  matrix.block<3, 1>(0, 3) = translation;

  MITK_INFO << "matrix NDI to ROT:" << endl << matrix;

  matrix.transposeInPlace();

  for (int i{0}; i < 16; i++)
  {
    m_matrix_ndiToRot_roboRegis[i] = matrix(i);
  }

  return true;
}


bool QmitkIGTFiducialRegistration::GetAxis_roboRegis(mitk::PointSet::Pointer pointsAroundAxis, double axis[3])
{
  int pointNum = pointsAroundAxis->GetSize();

  if (pointNum < 3)
  {
    return false;
  }

  // Get the axis using cross product
  auto point0 = pointsAroundAxis->GetPoint(0);
  auto point1 = pointsAroundAxis->GetPoint(1);
  auto point2 = pointsAroundAxis->GetPoint(3);

  Eigen::Vector3d p0;
  Eigen::Vector3d p1;
  Eigen::Vector3d p2;

  for (int i{0}; i < 3; i++)
  {
    p0[i] = point0[i];
    p1[i] = point1[i];
    p2[i] = point2[i];
  }

  Eigen::Vector3d v0 = p1 - p0;
  Eigen::Vector3d v1 = p2 - p0;

  Eigen::Vector3d axisVector = v0.cross(v1);
  axisVector.normalize();

  if (pointNum == 3)
  {
    axis[0] = axisVector[0];
    axis[1] = axisVector[1];
    axis[2] = axisVector[2];

    return true;
  }

  // Get the axis by fitting a circle
  std::vector<double> inp_pointset(3 * (pointsAroundAxis->GetSize()));
  std::array<double, 3> outp_center;
  double outp_radius;
  std::array<double, 3> outp_normal;
  for (int i{0}; i < (pointsAroundAxis->GetSize()); i++)
  {
    inp_pointset[3 * i] = pointsAroundAxis->GetPoint(i)[0];
    inp_pointset[3 * i + 1] = pointsAroundAxis->GetPoint(i)[1];
    inp_pointset[3 * i + 2] = pointsAroundAxis->GetPoint(i)[2];
  }

  lancetAlgorithm::fit_circle_3d(inp_pointset, outp_center, outp_radius, outp_normal);

  if ((outp_normal[0] * axisVector[0] + outp_normal[1] * axisVector[1] + outp_normal[2] * axisVector[2]) > 0)
  {
    axis[0] = outp_normal[0];
    axis[1] = outp_normal[1];
    axis[2] = outp_normal[2];
  }
  else
  {
    axis[0] = - outp_normal[0];
    axis[1] = - outp_normal[1];
    axis[2] = - outp_normal[2];
  }


 
  return true;
}


bool QmitkIGTFiducialRegistration::GetFlageOriginInNdi_roboRegis(mitk::PointSet::Pointer points, double center[3])
{
  if (points->GetSize() < 4)
  {
    return false;
  }

  std::vector<double> inp_pointset(3 * (points->GetSize()));
  std::array<double, 3> outp_center;
  double outp_radius;
  std::array<double, 3> outp_normal;
  for (int i{0}; i < (points->GetSize()); i++)
  {
    inp_pointset[3 * i] = points->GetPoint(i)[0];
    inp_pointset[3 * i + 1] = points->GetPoint(i)[1];
    inp_pointset[3 * i + 2] = points->GetPoint(i)[2];
  }

  lancetAlgorithm::fit_sphere(inp_pointset, outp_center, outp_radius);

  center[0] = outp_center[0];
  center[1] = outp_center[1];
  center[2] = outp_center[2];

  return true;
}


  // ↑↑↑↑↑  functions  ↑↑↑↑↑ 
