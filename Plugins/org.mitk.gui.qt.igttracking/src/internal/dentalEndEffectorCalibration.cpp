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

// Qmitk
#include "QmitkIGTFiducialRegistration.h"
#include <QmitkRenderWindow.h>

// Qt
#include <QSettings>




//--------------------slots---------------------------
void QmitkIGTFiducialRegistration::ConfirmDentalProbePointer()
{
  int toolID = m_Controls.navipointerSelectionWidget_dental->GetSelectedToolID();
  m_dentalProbeNDPointer =
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationDataSource()->GetOutput(toolID);
  // m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.label_probe_dental->setText(
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationTool()->GetToolName().c_str());
}
void QmitkIGTFiducialRegistration::ConfirmDentalCalibratorDrfPointer()
{
  int toolID = m_Controls.navipointerSelectionWidget_dental->GetSelectedToolID();
  m_dentalCalibratorDrfNDPointer =
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationDataSource()->GetOutput(toolID);
  // m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.label_calibratorDrf_dental->setText(
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationTool()->GetToolName().c_str());
}
void QmitkIGTFiducialRegistration::ConfirmDentalRobotDrfPointer()
{
  int toolID = m_Controls.navipointerSelectionWidget_dental->GetSelectedToolID();
  m_dentalRobotDrfNDPointer =
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationDataSource()->GetOutput(toolID);
  // m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.label_robotDrf_dental->setText(
    m_Controls.navipointerSelectionWidget_dental->GetSelectedNavigationTool()->GetToolName().c_str());
}


void QmitkIGTFiducialRegistration::CollectCheckPoint1InCalibratorDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToCalibrator;
  matrixNdiToCalibrator->DeepCopy(getVtkMatrix4x4(m_dentalCalibratorDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixCalibratorToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToCalibrator, matrixCalibratorToProbe);

  m_point_probeOnCheckPoint_1_dental[0] = matrixCalibratorToProbe->GetElement(0, 3);
  m_point_probeOnCheckPoint_1_dental[1] = matrixCalibratorToProbe->GetElement(1, 3);
  m_point_probeOnCheckPoint_1_dental[2] = matrixCalibratorToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dental->append("CheckPoint 1 collected:" +
                                        QString::number(m_point_probeOnCheckPoint_1_dental[0]) + "|" +
    QString::number(m_point_probeOnCheckPoint_1_dental[1]) + "|" +
    QString::number(m_point_probeOnCheckPoint_1_dental[2]));

}

void QmitkIGTFiducialRegistration::CollectCheckPoint2InCalibratorDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToCalibrator;
  matrixNdiToCalibrator->DeepCopy(getVtkMatrix4x4(m_dentalCalibratorDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixCalibratorToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToCalibrator, matrixCalibratorToProbe);

  m_point_probeOnCheckPoint_2_dental[0] = matrixCalibratorToProbe->GetElement(0, 3);
  m_point_probeOnCheckPoint_2_dental[1] = matrixCalibratorToProbe->GetElement(1, 3);
  m_point_probeOnCheckPoint_2_dental[2] = matrixCalibratorToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dental->append(
    "CheckPoint 2 collected:" + QString::number(m_point_probeOnCheckPoint_2_dental[0]) + "|" +
    QString::number(m_point_probeOnCheckPoint_2_dental[1]) + "|" +
    QString::number(m_point_probeOnCheckPoint_2_dental[2]));
}

void QmitkIGTFiducialRegistration::CollectCheckPoint3InCalibratorDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToCalibrator;
  matrixNdiToCalibrator->DeepCopy(getVtkMatrix4x4(m_dentalCalibratorDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixCalibratorToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToCalibrator, matrixCalibratorToProbe);

  m_point_probeOnCheckPoint_3_dental[0] = matrixCalibratorToProbe->GetElement(0, 3);
  m_point_probeOnCheckPoint_3_dental[1] = matrixCalibratorToProbe->GetElement(1, 3);
  m_point_probeOnCheckPoint_3_dental[2] = matrixCalibratorToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dental->append(
    "CheckPoint 3 collected:" + QString::number(m_point_probeOnCheckPoint_3_dental[0]) + "|" +
    QString::number(m_point_probeOnCheckPoint_3_dental[1]) + "|" +
    QString::number(m_point_probeOnCheckPoint_3_dental[2]));
}


void QmitkIGTFiducialRegistration::GetDentalFlangeToDrillMatrix()
{
  UpdateStandardCheckPoints();
  GetMatrixDrillToCheckPoints(); // needs to be inverted
  vtkNew<vtkMatrix4x4> matrixCheckPointsToDrill;
  matrixCheckPointsToDrill->DeepCopy(m_matrix_drillToCheckPoints);
  matrixCheckPointsToDrill->Invert();

  GetMatrixCalibratorDrfToCheckPoints();
  vtkNew<vtkMatrix4x4> matrixCalibratorDrfToCheckPoints;
  matrixCalibratorDrfToCheckPoints->DeepCopy(m_matrix_calibratorDrfToCheckPoints);

  // MatrixRobotDrfToCalibrator is to be calculated
  GetMatrixFlangeToRobotDrf();
  vtkNew<vtkMatrix4x4> matrixFlangeToRobotDrf;
  matrixFlangeToRobotDrf->DeepCopy(m_matrix_flangeToRobotDrf);

  // Calculate MatrixRobotDrfToCalibrator ; the calibrator should be attached to the robot arm end at this moment
  vtkNew<vtkMatrix4x4> ndiToRobotDrf;
  ndiToRobotDrf->DeepCopy(getVtkMatrix4x4(m_dentalRobotDrfNDPointer));

  vtkNew<vtkMatrix4x4> ndiToCalibratorDrf;
  ndiToCalibratorDrf->DeepCopy(getVtkMatrix4x4(m_dentalCalibratorDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixRobotDrfToCalibratorDrf;
  getReferenceMatrix4x4(ndiToCalibratorDrf, ndiToRobotDrf, matrixRobotDrfToCalibratorDrf);

  // Assemble the above matrices
  vtkNew<vtkTransform> tmpTransform; // flange to drill
  tmpTransform->PostMultiply();
  tmpTransform->Identity();
  tmpTransform->Concatenate(matrixCheckPointsToDrill);
  tmpTransform->Concatenate(matrixCalibratorDrfToCheckPoints);
  tmpTransform->Concatenate(matrixRobotDrfToCalibratorDrf);
  tmpTransform->Concatenate(matrixFlangeToRobotDrf);
  tmpTransform->Update();

  auto vtkMatrixFlangeToDrill = tmpTransform->GetMatrix();
  auto tmpArray = vtkMatrixFlangeToDrill->GetData();

  Eigen::Matrix4d eigenMatrixFlangeToDrill{tmpArray};

  for (int i{0}; i< 16; i++)
  {
    m_matrix_flangeToDrill[i] = eigenMatrixFlangeToDrill(i);
  }

  eigenMatrixFlangeToDrill.transposeInPlace();


  // Fine tune the x and y axis of the drill coordinate
  Eigen::Vector3d z{eigenMatrixFlangeToDrill(8), eigenMatrixFlangeToDrill(9), eigenMatrixFlangeToDrill(10)};
  Eigen::Vector3d tmpY{0, 1, 0};
  Eigen::Vector3d x = tmpY.cross(z);
  x.normalize();
  Eigen::Vector3d y = z.cross(x);
  y.normalize();

  eigenMatrixFlangeToDrill.block<3, 1>(0, 0) = x;
  eigenMatrixFlangeToDrill.block<3, 1>(0, 1) = y;
  eigenMatrixFlangeToDrill.block<3, 1>(0, 2) = z;

  // Calculate the drill tip deviation
  double drillTipError = sqrt(pow(eigenMatrixFlangeToDrill(12) + 3.02, 2) + pow(eigenMatrixFlangeToDrill(13) + 19.78, 2) +
         pow(eigenMatrixFlangeToDrill(14) - 326.10, 2));

  Eigen::Vector3d standardDrillAxis{0, 0.67, 0.74};

  double cosineVlaue = z.dot(standardDrillAxis) / (z.norm() * standardDrillAxis.norm());
  double drillAxisAngleError = acos(cosineVlaue) * 180 / 3.14159; 

  MITK_INFO << "m_matrix_flangeToDrill" << endl << eigenMatrixFlangeToDrill <<endl;

  MITK_INFO << "Drill tip in flange coordinate:" << endl;
  MITK_INFO << "x:" << eigenMatrixFlangeToDrill(12) << " (-3.02)" << endl;
  MITK_INFO << "y:" << eigenMatrixFlangeToDrill(13) << " (-19.78)" << endl;
  MITK_INFO << "z:" << eigenMatrixFlangeToDrill(14) << " (326.10)" << endl;
  MITK_INFO << "Deviation: " << drillTipError << endl;
  MITK_INFO << "Drill Axis Deviation: " << drillAxisAngleError << endl;

  // MITK_INFO << "Drill tip +z 1 mm in flange coordinate:" << endl;
  // MITK_INFO << "x:" << eigenMatrixFlangeToDrill(12) + eigenMatrixFlangeToDrill(8) << " (-3.02)" << endl;
  // MITK_INFO << "y:" << eigenMatrixFlangeToDrill(13) + eigenMatrixFlangeToDrill(9) << " (-19.11)" << endl;
  // MITK_INFO << "z:" << eigenMatrixFlangeToDrill(14) + eigenMatrixFlangeToDrill(10) << " (326.75)" << endl;

}


//--------------------fucntions-----------------------
void QmitkIGTFiducialRegistration::testtDental()
{
  GetMatrixFlangeToRobotDrf();
  GetMatrixDrillToCheckPoints();
  GetMatrixCalibratorDrfToCheckPoints();
}

bool QmitkIGTFiducialRegistration::GetMatrixFlangeToRobotDrf()
{
  // Ball 1 as the origin of the robot DRF coordinate system
  // 1-->2 : x axis
  // 1-->2 X 1-->3 : y axis
  // this configuration should be conformed when preparing the .rom file !!!

  // Eigen::Vector3d ballCenter_1{m_point_standardRobotDrfBall_1};
  // Eigen::Vector3d ballCenter_2{m_point_standardRobotDrfBall_2};
  // Eigen::Vector3d ballCenter_3{m_point_standardRobotDrfBall_3};
  //
  // Eigen::Vector3d x = ballCenter_2 - ballCenter_1;
  // x.normalize();
  //
  // Eigen::Vector3d y = x.cross(ballCenter_3 - ballCenter_1);
  // y.normalize();
  //
  // Eigen::Vector3d z = x.cross(y);
  // z.normalize();
  //
  // Eigen::Vector3d t{ballCenter_1[0], ballCenter_1[1], ballCenter_1[2]};
  //
  // Eigen::Matrix4d matrix;
  // matrix.setIdentity();
  //
  // matrix.block<3, 1>(0, 0) = x;
  // matrix.block<3, 1>(0, 1) = y;
  // matrix.block<3, 1>(0, 2) = z;
  // matrix.block<3, 1>(0, 3) = t;
  //
  // // MITK_INFO << "x" << endl<<x<<endl;
  // // MITK_INFO << "y" << endl<<y<<endl;
  // // MITK_INFO << "z" << endl << z << endl;
  //
  // MITK_INFO << "MatrixFlangeToRobotDrf: " << endl<<matrix;
  //
  // matrix.transposeInPlace();
  //
  // //MITK_INFO << "matrix" << endl << matrix;
  //
  // for (int i{0}; i < 16; i++)
  // {
  //   m_matrix_flangeToRobotDrf[i] = matrix(i);
  // }

  m_matrix_flangeToRobotDrf[3] = 0;
  m_matrix_flangeToRobotDrf[7] = 0;
  m_matrix_flangeToRobotDrf[11] = 0;


  return true;
}

bool QmitkIGTFiducialRegistration::GetMatrixDrillToCheckPoints()
{
  m_currentDrillLength_dental = m_Controls.lineEdit_drillLength_dental->text().toDouble();
  double drillLengthOffset{m_currentDrillLength_dental - m_standardDrillLength_dental};
  m_point_standardCheckPoint_1_dental[2] -= drillLengthOffset;
  m_point_standardCheckPoint_2_dental[2] -= drillLengthOffset;
  m_point_standardCheckPoint_3_dental[2] -= drillLengthOffset;

  Eigen::Vector3d checkPoint_1{m_point_standardCheckPoint_1_dental};
  Eigen::Vector3d checkPoint_2{m_point_standardCheckPoint_2_dental};
  Eigen::Vector3d checkPoint_3{m_point_standardCheckPoint_3_dental};

  Eigen::Vector3d x = checkPoint_2 - checkPoint_1;
  x.normalize();

  Eigen::Vector3d y = x.cross(checkPoint_3 - checkPoint_1);
  y.normalize();

  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Vector3d t{m_point_standardCheckPoint_1_dental};

  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;
  matrix.block<3, 1>(0, 3) = t;

  MITK_INFO << "MatrixDrillToCheckPoints:" << endl << matrix;

  matrix.transposeInPlace();

  // MITK_INFO << "matrix:" << endl << matrix;

  for (int i{0}; i < 16; i++)
  {
    m_matrix_drillToCheckPoints[i] = matrix(i);
  }

  return true;
}

bool QmitkIGTFiducialRegistration::GetMatrixCalibratorDrfToCheckPoints()
{
  Eigen::Vector3d checkPoint_1{m_point_probeOnCheckPoint_1_dental};
  Eigen::Vector3d checkPoint_2{m_point_probeOnCheckPoint_2_dental};
  Eigen::Vector3d checkPoint_3{m_point_probeOnCheckPoint_3_dental};

  Eigen::Vector3d x = checkPoint_2 - checkPoint_1;
  x.normalize();

  Eigen::Vector3d y = x.cross(checkPoint_3 - checkPoint_1);
  y.normalize();

  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Vector3d t{m_point_probeOnCheckPoint_1_dental};


  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;
  matrix.block<3, 1>(0, 3) = t;

  MITK_INFO << "MatrixCalibratorDrfToCheckPoints:" << endl << matrix;

  matrix.transposeInPlace();

  //MITK_INFO << "matrix:" << endl << matrix;

  for (int i{0}; i < 16; i++)
  {
    m_matrix_calibratorDrfToCheckPoints[i] = matrix(i);
  }

  return true;
}


bool QmitkIGTFiducialRegistration::UpdateStandardCheckPoints()
{
  if (m_Controls.radioButton_oldDental->isChecked())
  {
    m_point_standardCheckPoint_1_dental[0] = m_point_standardCheckPoint_1_old[0];
    m_point_standardCheckPoint_1_dental[1] = m_point_standardCheckPoint_1_old[1];
    m_point_standardCheckPoint_1_dental[2] = m_point_standardCheckPoint_1_old[2];
  }

  if (m_Controls.radioButton_maxilla->isChecked())
  {
    m_point_standardCheckPoint_1_dental[0] = m_point_standardCheckPoint_1_maxilla[0];
    m_point_standardCheckPoint_1_dental[1] = m_point_standardCheckPoint_1_maxilla[1];
    m_point_standardCheckPoint_1_dental[2] = m_point_standardCheckPoint_1_maxilla[2];
  }

  if (m_Controls.radioButton_mandible->isChecked())
  {
    m_point_standardCheckPoint_1_dental[0] = m_point_standardCheckPoint_1_mandible[0];
    m_point_standardCheckPoint_1_dental[1] = m_point_standardCheckPoint_1_mandible[1];
    m_point_standardCheckPoint_1_dental[2] = m_point_standardCheckPoint_1_mandible[2];
  }

  return true;
}




 
