/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QmitkNDIVegaWidget_H
#define QmitkNDIVegaWidget_H

#include "ui_QmitkNDIVegaWidget.h"
#include "QmitkNDIAbstractDeviceWidget.h"


/** Documentation:
*   \brief Implementation of a configuration widget for NDI Polaris Devices.
*
*   \ingroup IGTUI
*/
class MITKIGTUI_EXPORT QmitkNDIVegaWidget : public QmitkNDIAbstractDeviceWidget
{
  Q_OBJECT // this is needed for all Qt objects that should have a MOC object (everything that derives from QObject)

public:
  static const std::string VIEW_ID;

  QmitkNDIVegaWidget(QWidget* parent = nullptr, Qt::WindowFlags f = nullptr);
  ~QmitkNDIVegaWidget() override;

  void Initialize() override;

signals:

  protected slots :

private:
  /// \brief Creation of the connections
  void CreateConnections();

  void CreateQtPartControl(QWidget *parent);
protected:

  /** @return Returns the frame rate set in the m_frameRatePolaris ComboBox
  */
	int GetVegaTrackingRate();


  Ui::QmitkNDIVegaWidget* m_Controls;

  void SetPortValueToGUI(int portValue) override;
  void SetPortTypeToGUI(int portType) override;

  QmitkNDIVegaWidget* Clone(QWidget* parent) const override;

public:
  void ResetOutput() override;
  void AddOutput(std::string s) override;
  mitk::TrackingDevice::Pointer GetTrackingDevice() override;

  void StoreUISettings() override;
  void LoadUISettings() override;
};
#endif
