/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

//Mitk
#include <mitkDataNode.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateDataType.h>
#include <mitkImageGenerator.h>

// Qmitk
#include "QmitkIGTFiducialRegistration.h"
#include <QmitkRenderWindow.h>

// Qt
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

// MicroServices
#include <usModuleContext.h>
#include <usGetModuleContext.h>
#include "usServiceReference.h"
#include <vtkQuaternion.h>

const std::string QmitkIGTFiducialRegistration::VIEW_ID = "org.mitk.views.IGTFiducialRegistration";

void QmitkIGTFiducialRegistration::SetFocus()
{
}

void QmitkIGTFiducialRegistration::CreateQtPartControl( QWidget *parent )
{

  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi( parent );

  //Connect signals and slots
  connect(m_Controls.m_ChooseSelectedPointer, SIGNAL(clicked()), this, SLOT(PointerSelectionChanged()));
  connect(m_Controls.m_ChooseSelectedImage, SIGNAL(clicked()), this, SLOT(ImageSelectionChanged()));

  // Add Spine CT preparation part
  connect(m_Controls.m_ChooseSelectedPointer_tool, SIGNAL(clicked()), this, SLOT(ToolSelect()));
  connect(m_Controls.m_ChooseSelectedPointer_reference, SIGNAL(clicked()), this, SLOT(ReferenceSelect()));
  connect(m_Controls.pushButton_prepareCT, SIGNAL(clicked()), this, SLOT(PrepareCT()));
  m_ToolNDPointer = mitk::NavigationData::New();
  m_ReferenceNDPointer = mitk::NavigationData::New();

  //Initialize Combobox
  m_Controls.m_DataStorageComboBox->SetDataStorage(this->GetDataStorage());
  m_Controls.m_DataStorageComboBox->SetAutoSelectNewItems(false);
  m_Controls.m_DataStorageComboBox->SetPredicate(mitk::NodePredicateOr::New(mitk::NodePredicateDataType::New("Surface"), mitk::NodePredicateDataType::New("Image")));

  //Initialize Fiducial Registration Widget
  m_Controls.m_FiducialRegistrationWidget->setDataStorage(this->GetDataStorage());
  m_Controls.m_FiducialRegistrationWidget->HideStaticRegistrationRadioButton(true);
  m_Controls.m_FiducialRegistrationWidget->HideContinousRegistrationRadioButton(true);
  m_Controls.m_FiducialRegistrationWidget->HideUseICPRegistrationCheckbox(true);

}

void QmitkIGTFiducialRegistration::InitializeRegistration()
{
  foreach(QmitkRenderWindow* renderWindow, this->GetRenderWindowPart(mitk::WorkbenchUtil::OPEN)->GetQmitkRenderWindows().values())
  {
    this->m_Controls.m_FiducialRegistrationWidget->AddSliceNavigationController(renderWindow->GetSliceNavigationController());
  }
}

void QmitkIGTFiducialRegistration::PointerSelectionChanged()
{
  InitializeRegistration();
  int toolID = m_Controls.m_TrackingDeviceSelectionWidget->GetSelectedToolID();
  m_TrackingPointer = m_Controls.m_TrackingDeviceSelectionWidget->GetSelectedNavigationDataSource()->GetOutput(toolID);
  m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.m_PointerLabel->setText(m_Controls.m_TrackingDeviceSelectionWidget->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::ImageSelectionChanged()
{
  InitializeRegistration();
  m_Controls.m_ImageLabel->setText(m_Controls.m_DataStorageComboBox->GetSelectedNode()->GetName().c_str());
  m_Controls.m_FiducialRegistrationWidget->setImageNode(m_Controls.m_DataStorageComboBox->GetSelectedNode());
}

QmitkIGTFiducialRegistration::QmitkIGTFiducialRegistration()
{

}

QmitkIGTFiducialRegistration::~QmitkIGTFiducialRegistration()
{

}


// Add Spine CT preparation part
void QmitkIGTFiducialRegistration::ReferenceSelect()
{
  int toolID = m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedToolID();
  m_ReferenceNDPointer =
    m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedNavigationDataSource()->GetOutput(toolID);
  // m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.m_PointerLabel_reference->setText(
    m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedNavigationTool()->GetToolName().c_str());
}

void QmitkIGTFiducialRegistration::ToolSelect(){
  int toolID = m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedToolID();
  m_ToolNDPointer =
    m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedNavigationDataSource()->GetOutput(toolID);
  // m_Controls.m_FiducialRegistrationWidget->setTrackerNavigationData(m_TrackingPointer);
  m_Controls.m_PointerLabel_tool->setText(
    m_Controls.m_TrackingDeviceSelectionWidget_sawcalib->GetSelectedNavigationTool()->GetToolName().c_str());
};

vtkMatrix4x4 *QmitkIGTFiducialRegistration::getVtkMatrix4x4(mitk::NavigationData::Pointer nd)
{
  auto o = nd->GetOrientation();
  double R[3][3];
  double *V = {nd->GetPosition().GetDataPointer()};
  vtkQuaterniond quaterniond{o.r(), o.x(), o.y(), o.z()};
  quaterniond.ToMatrix3x3(R);

  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  matrix->SetElement(0, 0, R[0][0]);
  matrix->SetElement(0, 1, R[0][1]);
  matrix->SetElement(0, 2, R[0][2]);
  matrix->SetElement(1, 0, R[1][0]);
  matrix->SetElement(1, 1, R[1][1]);
  matrix->SetElement(1, 2, R[1][2]);
  matrix->SetElement(2, 0, R[2][0]);
  matrix->SetElement(2, 1, R[2][1]);
  matrix->SetElement(2, 2, R[2][2]);

  matrix->SetElement(0, 3, V[0]);
  matrix->SetElement(1, 3, V[1]);
  matrix->SetElement(2, 3, V[2]);

  matrix->Print(std::cout);
  return matrix;
}

void QmitkIGTFiducialRegistration::getReferenceMatrix4x4(vtkMatrix4x4 *Mainmatrix,
                                                       vtkMatrix4x4 *Refmatrix,
                                                       vtkMatrix4x4 *Returnmatrix)
{
  Refmatrix->Invert();
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->PostMultiply();
  transform->SetMatrix(Mainmatrix);
  transform->Concatenate(Refmatrix);
  transform->Update();
  transform->GetMatrix(Returnmatrix);
}

void QmitkIGTFiducialRegistration::PrepareCT()
{
  if (m_ToolNDPointer->IsDataValid() && m_ReferenceNDPointer->IsDataValid())
  {
    auto toolMatrix = getVtkMatrix4x4(m_ToolNDPointer);
    auto ReferenceMatrix = getVtkMatrix4x4(m_ReferenceNDPointer);

    m_toolMatrixInRef = vtkMatrix4x4::New();
    getReferenceMatrix4x4(toolMatrix, ReferenceMatrix, m_toolMatrixInRef);

    m_toolMatrixInRef->Print(std::cout);

    if (GetDataStorage()->GetNamedNode("PatientCT") == nullptr)
    {
      m_Controls.textBrowser_prepareCt->setText("Please rename the wanted image as PatientCT");
    }
    else
    {
      auto ctDrf_patientCt_matrix = vtkMatrix4x4::New();
      auto tmpNode = GetDataStorage()->GetNamedNode("PatientCT");
      ctDrf_patientCt_matrix = tmpNode->GetData()->GetGeometry()->GetVtkMatrix();
      vtkTransform *trans = vtkTransform::New();
      trans->PostMultiply();
      trans->SetMatrix(ctDrf_patientCt_matrix);
      trans->Concatenate(m_toolMatrixInRef);

      tmpNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(trans->GetMatrix());
    }

  }
  else
  {
    m_Controls.textBrowser_prepareCt->setText("CT DRF is not valid");
  }
}
