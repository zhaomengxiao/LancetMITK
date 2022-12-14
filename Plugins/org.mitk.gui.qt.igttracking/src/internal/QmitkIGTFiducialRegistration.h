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

  // --------------------Dental endEffector slots start----------------------

  void ConfirmDentalProbePointer();
  void ConfirmDentalCalibratorDrfPointer();
  void ConfirmDentalRobotDrfPointer();

  void CollectCheckPoint1InCalibratorDrf();
  void CollectCheckPoint2InCalibratorDrf();
  void CollectCheckPoint3InCalibratorDrf();

  void GetDentalFlangeToDrillMatrix();

  // --------------------Dental endEffector slots End----------------------

// --------------------start of Dental steelball calibration slots ----------------------

  void ConfirmDentalSteelballProbePointer();
  void ConfirmDentalSteelballDrfPointer();
  
  void CollectCheckPoint1InSteelballDrf();
  void CollectCheckPoint2InSteelballDrf();
  void CollectCheckPoint3InSteelballDrf();

  void CollectSteelballCenter1InSteelballDrf();
  void CollectSteelballCenter2InSteelballDrf();
  void CollectSteelballCenter3InSteelballDrf();
  void CollectSteelballCenter4InSteelballDrf();
  void CollectSteelballCenter5InSteelballDrf();
  void CollectSteelballCenter6InSteelballDrf();
  void CollectSteelballCenter7InSteelballDrf();

  void CompareSteelballs();
  // --------------------end of Dental steelball calibration slots ----------------------


  // -------------------- Start of Dental robotic arm registration slots ------------------- 
  void ConfirmTrolley_roboRegis();
  void ConfirmRoboDrf_roboRegis();

  void CollectPointAroundAxis1_roboRegis();
  void CollectPointAroundAxis2_roboRegis();
  
  void RemovePointAxis1_roboRegis();
  void RemovePointAxis2_roboRegis();
 
  void GetMatrixDrfToFlange_roboRegis();

  void GetTrolleyDrfToBase_roboRegis();
  void GetTrolleyDrfToBaseAvg_roboRegis();

  // -------------------- End of Dental robotic arm registration slots ------------------- 


  // Calibrate bended probe
  void CalibrateBendedProbe();



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



  //-----------start of Dental steelball calibration ---------------------

  mitk::NavigationData::Pointer m_dentalSteelballCalibrationProbeNDPointer;
  mitk::NavigationData::Pointer m_dentalSteelballDrfNDPointer;

  double m_points_designSteelballCenters[21]
  {25.36, -152.54, -21.55,
    21.88,  -155.34, -12.45,
    21.98, -151.21, -6.14,
    26.85,   -140.02, -4.47,
    20.11, -133.68, -25.36,
    17.35, -145.66, -32.41,
    18.93, -150.78, -29.20
  }; // the design locations of the steelballs in given coordinate system Fr

  double m_points_designCheckpointLocations[9]
  {
    15.26, -136.02, -28.42,
    18.83, -149.88, -19.68,
    23.39, -142.49, -5.33
  }; // the design locations of the checkpoints in given coordinate system Fr


  double m_points_steelballCentersInDrfCoordinate[21]{0}; // output for subsequent dental registration

  double m_steelballCenter1_inSteelballDrf[3]{0};
  double m_steelballCenter2_inSteelballDrf[3]{0};
  double m_steelballCenter3_inSteelballDrf[3]{0};
  double m_steelballCenter4_inSteelballDrf[3]{0};
  double m_steelballCenter5_inSteelballDrf[3]{0};
  double m_steelballCenter6_inSteelballDrf[3]{0};
  double m_steelballCenter7_inSteelballDrf[3]{0};

  double m_checkpoint1_inSteelballDrf[3]{0};
  double m_checkpoint2_inSteelballDrf[3]{0};
  double m_checkpoint3_inSteelballDrf[3]{0};

  double m_matrix_checkpointToGivenFrame[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  double m_matrix_steelballDrfToCheckPoints[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  bool GetMatrix_checkpointToGivenFrame();

  bool GetMatrix_steelballDrfToCheckpoints();

  bool GetSteelballCentersInDrfCoordinate();

  //-----------End of Dental steelball calibration ---------------------


  //---------------Start of dental robotic arm registration ------------------------
  mitk::NavigationData::Pointer m_trolleyNDpointer_roboRegis;
  mitk::NavigationData::Pointer m_roboDrfNDpointer_roboRegis;

  double m_matrix_ndiToInitialPosture[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // from NDI

  double m_matrix_flangeToRot_roboRegis[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // intermediate variable
  double m_matrix_ndiToRot_roboRegis[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // intermediate variable

  double m_matrix_flangeToDrf_roboRegis[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // final output

  double m_axis1Direction[3]{0};
  double m_axis2Direction[3]{0};

  bool GetMatrixFlangeToRot();
  bool GetMatrixNdiToRot();
  bool GetAxis_roboRegis(mitk::PointSet::Pointer pointsAroundAxis, double axis[3]);
  bool GetFlageOriginInNdi_roboRegis(mitk::PointSet::Pointer points, double center[3]);


  bool AverageNavigationData(mitk::NavigationData::Pointer ndPtr, 
	        int timeInterval /*milisecond*/, int intervalNum, double matrixArray[16]);

  //---------------End of dental robotic arm registration ------------------------


};








#endif // IGTFiducialRegistration_h
