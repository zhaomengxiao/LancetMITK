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
  void PrepareCT(); // Display the patient CT relative to the tracking DRF coordinate system 

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

};

#endif // IGTFiducialRegistration_h
