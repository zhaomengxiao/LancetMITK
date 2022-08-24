/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QmitkIGTFiducialRegistration_h
#define QmitkIGTFiducialRegistration_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QmitkIGTFiducialRegistrationControls.h"


#include <ctkServiceEvent.h>

/*!
\brief QmitkIGTFiducialRegistration

\sa QmitkFunctionality
\ingroup ${plugin_target}_internal
*/
class QmitkIGTFiducialRegistration : public QmitkAbstractView
{
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

  public:

    void SetFocus() override;

    static const std::string VIEW_ID;

    void CreateQtPartControl(QWidget *parent) override;

    QmitkIGTFiducialRegistration();
    ~QmitkIGTFiducialRegistration() override;

  public slots:


  protected slots:

  void PointerSelectionChanged();
  void ImageSelectionChanged();


  // Add Spine CT preparation part
  void ReferenceSelect(); // Reference refers to the tracking DRF
  void ToolSelect(); // Tool refers to the CtDrf
  void PrepareCT(); // Display the patient CT relative to the tracking DRF coordinate system. Here, the tracking DRF is the reference, the CtDrf is the tool
  void CreateNdiDataCarrier();
  void CreateNdiDataCarrier2();

  // --------------------Dental slots start----------------------
  void testtDental();

  void ConfirmDentalProbePointer();
  void ConfirmDentalCalibratorDrfPointer();
  void ConfirmDentalRobotDrfPointer();

  void CollectCheckPoint1InCalibratorDrf();
  void CollectCheckPoint2InCalibratorDrf();
  void CollectCheckPoint3InCalibratorDrf();

  void GetDentalFlangeToDrillMatrix();

  // --------------------Dental slots End----------------------


  protected:

    void InitializeRegistration();

    Ui::IGTFiducialRegistrationControls m_Controls;

    mitk::NavigationData::Pointer m_TrackingPointer;



  // Add Spine CT preparation part

    mitk::NavigationData::Pointer m_ReferenceNDPointer; // Reference refers to the tracking DRF
    mitk::NavigationData::Pointer m_ToolNDPointer;
    vtkMatrix4x4 *m_toolMatrixInRef; // Tool refers to the CtDrf

  void getReferenceMatrix4x4(vtkMatrix4x4 *Mainmatrix, vtkMatrix4x4 *Refmatrix, vtkMatrix4x4 *Returnmatrix);

  vtkMatrix4x4 *getVtkMatrix4x4(mitk::NavigationData::Pointer nd);


  // Dental end effector calibration

  // double leftSensorAngle{0};
  // double rightSensorAngle{0};
  // double distanceToNdiOrigin{0};

  double m_standardDrillLength_dental{22.5};
  double m_currentDrillLength_dental{22.5};

  double m_designDrillTip_dental[3]{-3.02, -19.78, 326.10}; // in flange coordinate
  double m_designDrillAxis_dental[3]{0, 0.67, 0.74}; // in flange coordinate

  double m_oldDrillTip_dental[3]{-3.02, -19.78, 326.10}; // in flange coordinate
  double m_oldDrillAxis_dental[3]{0, -0.67, -0.74};        // in flange coordinate

  double m_mandibleDrillTip_dental[3]{18.0, -123.67, 222.69}; // in flange coordinate
  double m_mandibleDrillAxis_dental[3]{0.0, 0.24, -0.97};        // in flange coordinate

  double m_maxillaDrillTip_dental[3]{18.0, -62.53, 289.25};  // in flange coordinate
  double m_maxillaDrillAxis_dental[3]{0.0, 0.93, 0.38};        // in flange coordinate

  double m_point_standardCheckPoint_1_dental[3]{-17.47, -21.43,-0.25}; // in drill tip coordinate 
  double m_point_standardCheckPoint_2_dental[3]{-12.46, 27.59, -0.25}; // in drill tip coordinate 
  double m_point_standardCheckPoint_3_dental[3]{24.78, 16.27, -0.25}; // in drill tip coordinate 

  double m_point_standardCheckPoint_1_old[3]{-17.47, -21.43, -0.25}; // in drill tip coordinate
  double m_point_standardCheckPoint_2_old[3]{-12.46, 27.59, -0.25};   // in drill tip coordinate
  double m_point_standardCheckPoint_3_old[3]{24.78, 16.27, -0.25};   // in drill tip coordinate 

  double m_point_standardCheckPoint_1_mandible[3]{10.0, -10.0, 1.0}; // in drill tip coordinate 
  double m_point_standardCheckPoint_2_mandible[3]{0.0, 6.0, 1.0}; // in drill tip coordinate 
  double m_point_standardCheckPoint_3_mandible[3]{-10.0, -2.0, 1.0}; // in drill tip coordinate 

  double m_point_standardCheckPoint_1_maxilla[3]{8.0, 5.0, 1.0};    // in drill tip coordinate
  double m_point_standardCheckPoint_2_maxilla[3]{-10.0, 3.0, 1.0};  // in drill tip coordinate
  double m_point_standardCheckPoint_3_maxilla[3]{-8.0, -5.0, 1.0};  // in drill tip coordinate 

  double m_point_standardRobotDrfBall_1[3]{-59.36, 75.06, 100.28};
  double m_point_standardRobotDrfBall_2[3]{0, 62.40, 87.62};
  double m_point_standardRobotDrfBall_3[3]{74.89, 75.06, 100.28};

  double m_point_probeOnCheckPoint_1_dental[3]{0};  // in calibrator coordinate system
  double m_point_probeOnCheckPoint_2_dental[3]{0};  // in calibrator coordinate system
  double m_point_probeOnCheckPoint_3_dental[3]{0};  // in calibrator coordinate system

  mitk::NavigationData::Pointer m_dentalProbeNDPointer;
  mitk::NavigationData::Pointer m_dentalCalibratorDrfNDPointer;
  mitk::NavigationData::Pointer m_dentalRobotDrfNDPointer;

  // double m_matrix_ndiToProbe[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  double m_matrix_ndiToRobotDrf[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  double m_matrix_ndiToCalibratorDrf[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  double m_matrix_flangeToRobotDrf[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  double m_matrix_drillToCheckPoints[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  double m_matrix_calibratorDrfToCheckPoints[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  double m_matrix_robotDrfToCalibratorDrf[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  double m_matrix_robotDrfToDrill[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  double m_matrix_flangeToDrill[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};


  bool UpdateStandardCheckPoints(/*update m_point_standardCheckPoint_1/2/3_dental*/);

  bool GetMatrixFlangeToRobotDrf(/*update m_matrix_flangeToRobotDrf*/);
  bool GetMatrixDrillToCheckPoints(/*update m_matrix_drillToCheckPoints*/);

  bool GetMatrixCalibratorDrfToCheckPoints(/*update m_matrix_calibratorDrfToCheckPoints*/);
  bool GetMatrixRobotDrfToCalibratorDrf(/*update m_matrix_robotDrfToCalibratorDrf*/);

  bool ManipulateXYaxes();

  bool EvaluateResult();

};








#endif // IGTFiducialRegistration_h
