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
#include <QThread>


// ↓↓↓↓↓  Slots  ↓↓↓↓↓
void QmitkIGTFiducialRegistration::ConfirmTrolley_roboRegis()
{
  int toolID = m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedToolID();

  m_trolleyNDpointer_roboRegis =
    m_Controls.navipointerSelectionWidget_roboRegis->GetSelectedNavigationDataSource()->GetOutput(toolID);

  m_Controls.label_trolley_roboRegis->setText(
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
  // matrixNdiToRoboDrf->DeepCopy(getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis));
  double array[16];
  AverageNavigationData(m_roboDrfNDpointer_roboRegis, 30, 20, array);
  matrixNdiToRoboDrf->DeepCopy(array);

  mitk::Point3D tmpPoint;
  tmpPoint[0] = matrixNdiToRoboDrf->GetElement(0, 3);
  tmpPoint[1] = matrixNdiToRoboDrf->GetElement(1, 3);
  tmpPoint[2] = matrixNdiToRoboDrf->GetElement(2, 3);

  int poinSetSize = collectedPoints->GetSize();

  if (poinSetSize == 0)
  {
    // auto ndiToInitialPostureMatrix = getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis);

    for (int i{0}; i < 4; i++)
    {
      for (int j{0}; j < 4; j++)
      {
        m_matrix_ndiToInitialPosture[4 * i + j] = matrixNdiToRoboDrf->GetElement(i, j);
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
  // matrixNdiToRoboDrf->DeepCopy(getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis));
  double tmpArray[16];
  AverageNavigationData(m_roboDrfNDpointer_roboRegis, 30, 20, tmpArray);
  matrixNdiToRoboDrf->DeepCopy(tmpArray);


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

	for (int i{ 0 }; i < 4; i++)
	{
		  m_Controls.textBrowser_roboRegis->append(
			QString::number(matrixFlangeToDrf->GetElement(i, 0)) + "  " +
			QString::number(matrixFlangeToDrf->GetElement(i, 1)) + "  " +
			QString::number(matrixFlangeToDrf->GetElement(i, 2)) + " " +
			QString::number(matrixFlangeToDrf->GetElement(i, 3)));
	}

  }else
  {
    m_Controls.textBrowser_roboRegis->append("Points are not ready!");
  }
}


void QmitkIGTFiducialRegistration::GetTrolleyDrfToBase_roboRegis()
{
  if(GetDataStorage()->GetNamedNode("RoboBaseToFlange matrix") == nullptr)
  {
	  m_Controls.textBrowser_roboRegis->append("The base to flange matrix hasn't been recorded yet");
	  return;
  }

  vtkNew<vtkMatrix4x4> flangeToBaseMatrix;
  auto tmpMatrix = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("RoboBaseToFlange matrix")->GetData())->GetGeometry()->GetVtkMatrix();
  flangeToBaseMatrix->DeepCopy(tmpMatrix);
  flangeToBaseMatrix->Invert();

  vtkNew<vtkMatrix4x4> endDrfToFlangeMatrix;
  endDrfToFlangeMatrix->DeepCopy(m_matrix_flangeToDrf_roboRegis);
  endDrfToFlangeMatrix->Invert();

  auto trolleyDrfToNdiMatrix = getVtkMatrix4x4(m_trolleyNDpointer_roboRegis);
  trolleyDrfToNdiMatrix->Invert();

  auto ndiToEndDrfMatrix = getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis);

  vtkNew<vtkTransform> tmpTransform; // baseRf to base
  tmpTransform->Identity();
  tmpTransform->PostMultiply();
  tmpTransform->Concatenate(flangeToBaseMatrix);
  tmpTransform->Concatenate(endDrfToFlangeMatrix);
  tmpTransform->Concatenate(ndiToEndDrfMatrix);
  tmpTransform->Concatenate(trolleyDrfToNdiMatrix);
  tmpTransform->Update();

  auto trolleyDrfToBase = tmpTransform->GetMatrix();

  m_Controls.textBrowser_roboRegis->append("BaseRf to base matrix:");

  for (int i{0}; i < 4; i++)
  {
	  m_Controls.textBrowser_roboRegis->append(
		  QString::number(trolleyDrfToBase->GetElement(i,0))+"  "+
		  QString::number(trolleyDrfToBase->GetElement(i, 1)) + "  " + 
		  QString::number(trolleyDrfToBase->GetElement(i, 2)) + " " + 
		  QString::number(trolleyDrfToBase->GetElement(i, 3)));
  }

}

void QmitkIGTFiducialRegistration::GetTrolleyDrfToBaseAvg_roboRegis()
{
	if (GetDataStorage()->GetNamedNode("RoboBaseToFlange matrix") == nullptr)
	{
		m_Controls.textBrowser_roboRegis->append("The base to flange matrix hasn't been recorded yet");
		return;
	}
  
	vtkNew<vtkMatrix4x4> flangeToBaseMatrix;
	auto tmpMatrix = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("RoboBaseToFlange matrix")->GetData())->GetGeometry()->GetVtkMatrix();
	flangeToBaseMatrix->DeepCopy(tmpMatrix);
	flangeToBaseMatrix->Invert();

	vtkNew<vtkMatrix4x4> endDrfToFlangeMatrix;
	endDrfToFlangeMatrix->DeepCopy(m_matrix_flangeToDrf_roboRegis);
	endDrfToFlangeMatrix->Invert();

	
	double tmp_x[3]{ 0,0,0 };
	double tmp_y[3]{ 0,0,0 };
	double tmp_translation[3]{ 0,0,0 };

  for(int i{0}; i < 100; i++)
  {
	  m_trolleyNDpointer_roboRegis->Update();
	  m_roboDrfNDpointer_roboRegis->Update();

	  auto trolleyDrfToNdiMatrix_tmp = getVtkMatrix4x4(m_trolleyNDpointer_roboRegis);
	  trolleyDrfToNdiMatrix_tmp->Invert();
	  // auto trolleyDrfToNdiArray_tmp = trolleyDrfToNdiMatrix_tmp->GetData();

	  auto ndiToEndDrfMatrix_tmp = getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis);
	  // auto ndiToEndDrfArray_tmp = ndiToEndDrfMatrix_tmp->GetData();

	  vtkNew<vtkTransform> transformBaseRFtoEndRF;
	  transformBaseRFtoEndRF->Identity();
	  transformBaseRFtoEndRF->PostMultiply();
	  transformBaseRFtoEndRF->SetMatrix(ndiToEndDrfMatrix_tmp);
	  transformBaseRFtoEndRF->Concatenate(trolleyDrfToNdiMatrix_tmp);
	  transformBaseRFtoEndRF->Update();

	  auto tmpMatrix = transformBaseRFtoEndRF->GetMatrix();
	  m_Controls.textBrowser_roboRegis->append("Tmp baseRFtoEndRF matrix: " + QString::number(tmpMatrix->GetElement(0, 3)) +
		  "/" + QString::number(tmpMatrix->GetElement(1, 3)) + "/" + QString::number(tmpMatrix->GetElement(2, 3)));

	  tmp_x[0] += tmpMatrix->GetElement(0, 0);
	  tmp_x[1] += tmpMatrix->GetElement(1, 0);
	  tmp_x[2] += tmpMatrix->GetElement(2, 0);

	  tmp_y[0] += tmpMatrix->GetElement(0, 1);
	  tmp_y[1] += tmpMatrix->GetElement(1, 1);
	  tmp_y[2] += tmpMatrix->GetElement(2, 1);

	  tmp_translation[0] += tmpMatrix->GetElement(0, 3);
	  tmp_translation[1] += tmpMatrix->GetElement(1, 3);
	  tmp_translation[2] += tmpMatrix->GetElement(2, 3);

	  QThread::msleep(70);
  }

  // Assemble baseRF to EndRF matrix
  Eigen::Vector3d x;
  x[0] = tmp_x[0];
  x[1] = tmp_x[1];
  x[2] = tmp_x[2];
  x.normalize();

  Eigen::Vector3d h;
  h[0] = tmp_y[0];
  h[1] = tmp_y[1];
  h[2] = tmp_y[2];
  h.normalize();

  Eigen::Vector3d z;
  z = x.cross(h);
  z.normalize();

  Eigen::Vector3d y;
  y = z.cross(x);
  y.normalize();

  tmp_translation[0] = tmp_translation[0] / 100;
  tmp_translation[1] = tmp_translation[1] / 100;
  tmp_translation[2] = tmp_translation[2] / 100;

  vtkNew<vtkMatrix4x4> baseRFtoEndRFmatrix;
  double tmpArray[16]
  {
    x[0], y[0], z[0], tmp_translation[0],
	x[1], y[1], z[1], tmp_translation[1],
	x[2], y[2], z[2], tmp_translation[2],
    0,0,0,1
  };
  baseRFtoEndRFmatrix->DeepCopy(tmpArray);


	vtkNew<vtkTransform> tmpTransform; // baseRf to base
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(flangeToBaseMatrix);
	tmpTransform->Concatenate(endDrfToFlangeMatrix);
	tmpTransform->Concatenate(baseRFtoEndRFmatrix);
	tmpTransform->Update();

	auto trolleyDrfToBase = tmpTransform->GetMatrix();

	m_Controls.textBrowser_roboRegis->append("BaseRf to base matrix:");

	for (int i{ 0 }; i < 4; i++)
	{
		m_Controls.textBrowser_roboRegis->append(
			QString::number(trolleyDrfToBase->GetElement(i, 0)) + "  " +
			QString::number(trolleyDrfToBase->GetElement(i, 1)) + "  " +
			QString::number(trolleyDrfToBase->GetElement(i, 2)) + " " +
			QString::number(trolleyDrfToBase->GetElement(i, 3)));
	}
}


void QmitkIGTFiducialRegistration::CalibrateBendedProbe()
{
	auto pointsAroundAxis = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Points around axis 1")->GetData());

	int pointNum = pointsAroundAxis->GetSize();

	if (pointNum < 3)
	{
		m_Controls.textBrowser_dental->append("Not enough probe points!");
	}

	// Get the axis using cross product
	auto point0 = pointsAroundAxis->GetPoint(0);
	auto point1 = pointsAroundAxis->GetPoint(1);
	auto point2 = pointsAroundAxis->GetPoint(3);

	Eigen::Vector3d p0;
	Eigen::Vector3d p1;
	Eigen::Vector3d p2;

	for (int i{ 0 }; i < 3; i++)
	{
		p0[i] = point0[i];
		p1[i] = point1[i];
		p2[i] = point2[i];
	}

	Eigen::Vector3d v0 = p1 - p0;
	Eigen::Vector3d v1 = p2 - p0;

	Eigen::Vector3d axisVector = v0.cross(v1);
	axisVector.normalize();

	double axis[3]{ 0 };

	if (pointNum == 3)
	{
		axis[0] = axisVector[0];
		axis[1] = axisVector[1];
		axis[2] = axisVector[2];

	}

	// Get the axis by fitting a circle
	std::vector<double> inp_pointset(3 * (pointsAroundAxis->GetSize()));
	std::array<double, 3> outp_center;
	double outp_radius;
	std::array<double, 3> outp_normal;
	for (int i{ 0 }; i < (pointsAroundAxis->GetSize()); i++)
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
		axis[0] = -outp_normal[0];
		axis[1] = -outp_normal[1];
		axis[2] = -outp_normal[2];
	}

	double onePointOnPositiveAxis[3]{ 0 };
	onePointOnPositiveAxis[0] = outp_center[0] + axis[0];
	onePointOnPositiveAxis[1] = outp_center[1] + axis[1];
	onePointOnPositiveAxis[2] = outp_center[2] + axis[2];

	vtkNew<vtkMatrix4x4> centerMatrix;
	centerMatrix->Identity();
	centerMatrix->SetElement(0, 3, outp_center[0]);
	centerMatrix->SetElement(1, 3, outp_center[1]);
	centerMatrix->SetElement(2, 3, outp_center[2]);

	vtkNew<vtkMatrix4x4> positiveAxisPointMatrix;
	positiveAxisPointMatrix->Identity();
	positiveAxisPointMatrix->SetElement(0, 3, onePointOnPositiveAxis[0]);
	positiveAxisPointMatrix->SetElement(1, 3, onePointOnPositiveAxis[1]);
	positiveAxisPointMatrix->SetElement(2, 3, onePointOnPositiveAxis[2]);


	auto probeToNdiMatrix = getVtkMatrix4x4(m_roboDrfNDpointer_roboRegis);
	probeToNdiMatrix->Invert();

	vtkNew<vtkTransform> tmpTrans_0;
	tmpTrans_0->PostMultiply();
	tmpTrans_0->SetMatrix(centerMatrix);
	tmpTrans_0->Concatenate(probeToNdiMatrix);
	tmpTrans_0->Update();

	vtkNew<vtkTransform> tmpTrans_1;
	tmpTrans_1->PostMultiply();
	tmpTrans_1->SetMatrix(positiveAxisPointMatrix);
	tmpTrans_1->Concatenate(probeToNdiMatrix);
	tmpTrans_1->Update();

	m_Controls.textBrowser_dental->append("Circle center under probe:" + 
	QString::number(tmpTrans_0->GetMatrix()->GetElement(0,3))+ "/"+
		QString::number(tmpTrans_0->GetMatrix()->GetElement(1, 3)) + "/" + 
		QString::number(tmpTrans_0->GetMatrix()->GetElement(2, 3)) + "/");

	m_Controls.textBrowser_dental->append("Positive axis point under probe:" +
		QString::number(tmpTrans_1->GetMatrix()->GetElement(0, 3)) + "/" +
		QString::number(tmpTrans_1->GetMatrix()->GetElement(1, 3)) + "/" +
		QString::number(tmpTrans_1->GetMatrix()->GetElement(2, 3)) + "/");

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


bool QmitkIGTFiducialRegistration::AverageNavigationData(mitk::NavigationData::Pointer ndPtr, int timeInterval /*milisecond*/, int intervalNum, double matrixArray[16])
{
  // The frame rate of Vega ST is 60 Hz, so the timeInterval should be larger than 16.7 ms

	double tmp_x[3]{ 0,0,0 };
	double tmp_y[3]{ 0,0,0 };
	double tmp_translation[3]{ 0,0,0 };

	for (int i{ 0 }; i < intervalNum; i++)
	{
		ndPtr->Update();

		auto tmpMatrix = getVtkMatrix4x4(ndPtr);

		tmp_x[0] += tmpMatrix->GetElement(0, 0);
		tmp_x[1] += tmpMatrix->GetElement(1, 0);
		tmp_x[2] += tmpMatrix->GetElement(2, 0);

		tmp_y[0] += tmpMatrix->GetElement(0, 1);
		tmp_y[1] += tmpMatrix->GetElement(1, 1);
		tmp_y[2] += tmpMatrix->GetElement(2, 1);

		tmp_translation[0] += tmpMatrix->GetElement(0, 3);
		tmp_translation[1] += tmpMatrix->GetElement(1, 3);
		tmp_translation[2] += tmpMatrix->GetElement(2, 3);

		QThread::msleep(timeInterval);
	}

	// Assemble baseRF to EndRF matrix
	Eigen::Vector3d x;
	x[0] = tmp_x[0];
	x[1] = tmp_x[1];
	x[2] = tmp_x[2];
	x.normalize();

	Eigen::Vector3d h;
	h[0] = tmp_y[0];
	h[1] = tmp_y[1];
	h[2] = tmp_y[2];
	h.normalize();

	Eigen::Vector3d z;
	z = x.cross(h);
	z.normalize();

	Eigen::Vector3d y;
	y = z.cross(x);
	y.normalize();

	tmp_translation[0] = tmp_translation[0] / intervalNum;
	tmp_translation[1] = tmp_translation[1] / intervalNum;
	tmp_translation[2] = tmp_translation[2] / intervalNum;

	double tmpArray[16]
	{
	  x[0], y[0], z[0], tmp_translation[0],
	  x[1], y[1], z[1], tmp_translation[1],
	  x[2], y[2], z[2], tmp_translation[2],
	  0,0,0,1
	};

  for (int i{0}; i<16; i++)
  {
	  matrixArray[i] = tmpArray[i];
  }

	return true;
}


  // ↑↑↑↑↑  functions  ↑↑↑↑↑ 
