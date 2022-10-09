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




// ↓↓↓↓↓  Slots  ↓↓↓↓↓
void QmitkIGTFiducialRegistration::ConfirmDentalSteelballProbePointer()
{
  int toolID = m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedToolID();

  m_dentalSteelballCalibrationProbeNDPointer =
    m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedNavigationDataSource()->GetOutput(toolID);
  
  m_Controls.label_probe_dentalSteelball->setText(
    m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::ConfirmDentalSteelballDrfPointer()
{
  int toolID = m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedToolID();

  m_dentalSteelballDrfNDPointer =
    m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedNavigationDataSource()->GetOutput(toolID);

  m_Controls.label_steelballDrf_dentalSteelball->setText(
    m_Controls.navipointerSelectionWidget_dentalSteelball->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::CollectCheckPoint1InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_checkpoint1_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_checkpoint1_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_checkpoint1_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "CheckPoint 1 collected:" + QString::number(m_checkpoint1_inSteelballDrf[0]) + "|" +
    QString::number(m_checkpoint1_inSteelballDrf[1]) + "|" + QString::number(m_checkpoint1_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectCheckPoint2InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_checkpoint2_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_checkpoint2_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_checkpoint2_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "CheckPoint 2 collected:" + QString::number(m_checkpoint2_inSteelballDrf[0]) + "|" +
    QString::number(m_checkpoint2_inSteelballDrf[1]) + "|" + QString::number(m_checkpoint2_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectCheckPoint3InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_checkpoint3_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_checkpoint3_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_checkpoint3_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "CheckPoint 3 collected:" + QString::number(m_checkpoint3_inSteelballDrf[0]) + "|" +
    QString::number(m_checkpoint3_inSteelballDrf[1]) + "|" + QString::number(m_checkpoint3_inSteelballDrf[2]));

  GetSteelballCentersInDrfCoordinate();
}

void QmitkIGTFiducialRegistration::CollectSteelballCenter1InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter1_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter1_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter1_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 1 collected:" + QString::number(m_steelballCenter1_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter1_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter1_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter2InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter2_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter2_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter2_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 2 collected:" + QString::number(m_steelballCenter2_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter2_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter2_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter3InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter3_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter3_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter3_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 3 collected:" + QString::number(m_steelballCenter3_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter3_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter3_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter4InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter4_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter4_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter4_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 4 collected:" + QString::number(m_steelballCenter4_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter4_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter4_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter5InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter5_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter5_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter5_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 5 collected:" + QString::number(m_steelballCenter5_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter5_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter5_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter6InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter6_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter6_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter6_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 6 collected:" + QString::number(m_steelballCenter6_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter6_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter6_inSteelballDrf[2]));
}
void QmitkIGTFiducialRegistration::CollectSteelballCenter7InSteelballDrf()
{
  vtkNew<vtkMatrix4x4> matrixNdiToProbe;
  matrixNdiToProbe->DeepCopy(getVtkMatrix4x4(m_dentalSteelballCalibrationProbeNDPointer));

  vtkNew<vtkMatrix4x4> matrixNdiToSteelballDrf;
  matrixNdiToSteelballDrf->DeepCopy(getVtkMatrix4x4(m_dentalSteelballDrfNDPointer));

  vtkNew<vtkMatrix4x4> matrixSteelballDrfToProbe;
  getReferenceMatrix4x4(matrixNdiToProbe, matrixNdiToSteelballDrf, matrixSteelballDrfToProbe);

  m_steelballCenter7_inSteelballDrf[0] = matrixSteelballDrfToProbe->GetElement(0, 3);
  m_steelballCenter7_inSteelballDrf[1] = matrixSteelballDrfToProbe->GetElement(1, 3);
  m_steelballCenter7_inSteelballDrf[2] = matrixSteelballDrfToProbe->GetElement(2, 3);

  m_Controls.textBrowser_dentalSteelball->append(
    "Steelball 7 collected:" + QString::number(m_steelballCenter7_inSteelballDrf[0]) + "|" +
    QString::number(m_steelballCenter7_inSteelballDrf[1]) + "|" +
    QString::number(m_steelballCenter7_inSteelballDrf[2]));
}

void QmitkIGTFiducialRegistration::CompareSteelballs()
{
  double tmpSteelballs[21]{
    m_steelballCenter1_inSteelballDrf[0], m_steelballCenter1_inSteelballDrf[1], m_steelballCenter1_inSteelballDrf[2],
    m_steelballCenter2_inSteelballDrf[0], m_steelballCenter2_inSteelballDrf[1], m_steelballCenter2_inSteelballDrf[2],
    m_steelballCenter3_inSteelballDrf[0], m_steelballCenter3_inSteelballDrf[1], m_steelballCenter3_inSteelballDrf[2],
    m_steelballCenter4_inSteelballDrf[0], m_steelballCenter4_inSteelballDrf[1], m_steelballCenter4_inSteelballDrf[2],
    m_steelballCenter5_inSteelballDrf[0], m_steelballCenter5_inSteelballDrf[1], m_steelballCenter5_inSteelballDrf[2],
    m_steelballCenter6_inSteelballDrf[0], m_steelballCenter6_inSteelballDrf[1], m_steelballCenter6_inSteelballDrf[2],
    m_steelballCenter7_inSteelballDrf[0], m_steelballCenter7_inSteelballDrf[1], m_steelballCenter7_inSteelballDrf[2],
  };

  for (int i{0}; i < 7; i++)
  {
    m_Controls.textBrowser_dentalSteelball->append("Steelball " + QString::number((i + 1)) + ":");
    m_Controls.textBrowser_dentalSteelball->append(
      "From checkPoints: " + QString::number(m_points_steelballCentersInDrfCoordinate[3 * i]) + " |" +
      QString::number(m_points_steelballCentersInDrfCoordinate[3 * i + 1]) + " |" +
      QString::number(m_points_steelballCentersInDrfCoordinate[3 * i + 2]) );

    m_Controls.textBrowser_dentalSteelball->append("From direct probing: " + QString::number(tmpSteelballs[3 * i]) +
                                                   " |" + QString::number(tmpSteelballs[3 * i + 1]) + " |" +
                                                   QString::number(tmpSteelballs[3 * i + 2]));

    double deviation{0};
    deviation = sqrt(pow(tmpSteelballs[3 * i] - m_points_steelballCentersInDrfCoordinate[3 * i], 2) +
                     pow(tmpSteelballs[3 * i + 1] - m_points_steelballCentersInDrfCoordinate[3 * i + 1], 2) +
                     pow(tmpSteelballs[3 * i + 2] - m_points_steelballCentersInDrfCoordinate[3 * i + 2], 2));
    m_Controls.textBrowser_dentalSteelball->append("Deviation: " + QString::number(deviation));
  }

}

  // ↑↑↑↑↑  Slots  ↑↑↑↑↑ 





// ↓↓↓↓↓  functions  ↓↓↓↓↓
bool QmitkIGTFiducialRegistration::GetMatrix_checkpointToGivenFrame()
{
  Eigen::Vector3d checkpoint1{
    m_points_designCheckpointLocations[0],
    m_points_designCheckpointLocations[1],
    m_points_designCheckpointLocations[2],
  };

  Eigen::Vector3d checkpoint2{
    m_points_designCheckpointLocations[3],
    m_points_designCheckpointLocations[4],
    m_points_designCheckpointLocations[5],
  };

  Eigen::Vector3d checkpoint3{
    m_points_designCheckpointLocations[6],
    m_points_designCheckpointLocations[7],
    m_points_designCheckpointLocations[8],
  };

  Eigen::Vector3d x = checkpoint2 - checkpoint1;
  x.normalize();

  Eigen::Vector3d y = x.cross(checkpoint3 - checkpoint1);
  y.normalize();

  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Vector3d t{
    m_points_designCheckpointLocations[0],
    m_points_designCheckpointLocations[1],
    m_points_designCheckpointLocations[2],
  };

  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;
  matrix.block<3, 1>(0, 3) = t;

  Eigen::Matrix4d matrix_checkpointsToGivenFrame =  matrix.inverse();

  MITK_INFO << "MatrixCheckPointsToGivenFrame:" << endl << matrix;

  matrix_checkpointsToGivenFrame.transposeInPlace();

  for (int i{0}; i < 16; i++)
  {
    m_matrix_checkpointToGivenFrame[i] = matrix_checkpointsToGivenFrame(i);
  }

  return true;
}


bool QmitkIGTFiducialRegistration::GetMatrix_steelballDrfToCheckpoints()
{
  Eigen::Vector3d checkPoint1{m_checkpoint1_inSteelballDrf};
  Eigen::Vector3d checkPoint2{m_checkpoint2_inSteelballDrf};
  Eigen::Vector3d checkPoint3{m_checkpoint3_inSteelballDrf};

  Eigen::Vector3d x = checkPoint2 - checkPoint1;
  x.normalize();

  Eigen::Vector3d y = x.cross(checkPoint3 - checkPoint1);
  y.normalize();

  Eigen::Vector3d z = x.cross(y);
  z.normalize();

  Eigen::Vector3d t{m_checkpoint1_inSteelballDrf};

  Eigen::Matrix4d matrix;
  matrix.setIdentity();

  matrix.block<3, 1>(0, 0) = x;
  matrix.block<3, 1>(0, 1) = y;
  matrix.block<3, 1>(0, 2) = z;
  matrix.block<3, 1>(0, 3) = t;

  matrix.transposeInPlace();
  for (int i{0}; i < 16; i++)
  {
    m_matrix_steelballDrfToCheckPoints[i] = matrix(i);
  }

  return true;
};

bool QmitkIGTFiducialRegistration::GetSteelballCentersInDrfCoordinate()
{
  GetMatrix_checkpointToGivenFrame();
  GetMatrix_steelballDrfToCheckpoints();

  Eigen::Matrix4d matrixCheckPointsToGivenFrame{m_matrix_checkpointToGivenFrame};
  matrixCheckPointsToGivenFrame.transposeInPlace();

  Eigen::Matrix4d matrixSteelballDrfToCheckPoints{m_matrix_steelballDrfToCheckPoints};
  matrixSteelballDrfToCheckPoints.transposeInPlace();


  m_Controls.textBrowser_dentalSteelball->append("Calibration results:");
  for (int i{0}; i < 7; i++)
  {
    Eigen::Vector4d currentSteelball{m_points_designSteelballCenters[3 * i],
                                     m_points_designSteelballCenters[3 * i + 1],
                                     m_points_designSteelballCenters[3 * i + 2],
                                     1};

    Eigen::Vector4d steelballInDrf = matrixSteelballDrfToCheckPoints * matrixCheckPointsToGivenFrame * currentSteelball;

    m_points_steelballCentersInDrfCoordinate[3 * i] = steelballInDrf(0);
    m_points_steelballCentersInDrfCoordinate[3 * i + 1] = steelballInDrf(1);
    m_points_steelballCentersInDrfCoordinate[3 * i + 2] = steelballInDrf(2);

    m_Controls.textBrowser_dentalSteelball->append(
      "steelball " + QString::number((i + 1)) + ": " +
      QString::number(m_points_steelballCentersInDrfCoordinate[3 * i]) + " / " +
      QString::number(m_points_steelballCentersInDrfCoordinate[3 * i + 1]) + " / " +
      QString::number(m_points_steelballCentersInDrfCoordinate[3 * i + 2]));

  }

  
  

  return true;
}

// ↑↑↑↑↑  functions  ↑↑↑↑↑ 
