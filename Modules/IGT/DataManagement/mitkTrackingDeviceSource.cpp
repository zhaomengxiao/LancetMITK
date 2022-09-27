/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkTrackingDeviceSource.h"

#include "mitkTrackingDevice.h"
#include "mitkTrackingTool.h"

#include "mitkIGTTimeStamp.h"
#include "mitkIGTException.h"
#include "mitkIGTHardwareException.h"

mitk::TrackingDeviceSource::TrackingDeviceSource()
  : mitk::NavigationDataSource(), m_TrackingDevice(nullptr)
{
}

mitk::TrackingDeviceSource::~TrackingDeviceSource()
{
  if (m_TrackingDevice.IsNotNull())
  {
    if (m_TrackingDevice->GetState() == mitk::TrackingDevice::Tracking)
    {
      this->StopTracking();
    }
    if (m_TrackingDevice->GetState() == mitk::TrackingDevice::Ready)
    {
      this->Disconnect();
    }
    m_TrackingDevice = nullptr;
  }
}

void mitk::TrackingDeviceSource::GenerateData()
{
  if (m_IsFrozen) {return;} //no update at all if device is frozen
  else if (m_TrackingDevice.IsNull()) {return;}

  if (m_TrackingDevice->GetToolCount() < 1)
    return;

  if (this->GetNumberOfIndexedOutputs() != m_TrackingDevice->GetToolCount()) // mismatch between tools and outputs. What should we do? Were tools added to the tracking device after SetTrackingDevice() was called?
  {
    //check this: TODO:
    ////this might happen if a tool is plugged into an aurora during tracking.
    //this->CreateOutputs();

    std::stringstream ss;
    ss << "mitk::TrackingDeviceSource: not enough outputs available for all tools. "
      << this->GetNumberOfOutputs() << " outputs available, but "
      << m_TrackingDevice->GetToolCount() << " tools available in the tracking device.";
    throw std::out_of_range(ss.str());
  }
  /* update outputs with tracking data from tools */
  unsigned int toolCount = m_TrackingDevice->GetToolCount();
  for (unsigned int i = 0; i < toolCount; ++i)
  {
    mitk::NavigationData* nd = this->GetOutput(i);
    assert(nd);
    mitk::TrackingTool* t = m_TrackingDevice->GetTool(i);
    assert(t);

    if ((t->IsEnabled() == false) || (t->IsDataValid() == false))
    {
      nd->SetDataValid(false);
      continue;
    }
    nd->SetDataValid(true);
    mitk::NavigationData::PositionType p;
    t->GetPosition(p);
    nd->SetPosition(p);

    mitk::NavigationData::OrientationType o;
    t->GetOrientation(o);
    nd->SetOrientation(o);
    nd->SetOrientationAccuracy(t->GetTrackingError());
    nd->SetPositionAccuracy(t->GetTrackingError());
    nd->SetIGTTimeStamp(t->GetIGTTimeStamp());

    //for backward compatibility: check if the timestamp was set, if not create a default timestamp
    if (nd->GetIGTTimeStamp()==0) nd->SetIGTTimeStamp(mitk::IGTTimeStamp::GetInstance()->GetElapsed());
  }
}

void mitk::TrackingDeviceSource::SetTrackingDevice( mitk::TrackingDevice* td )
{
  MITK_DEBUG << "Setting TrackingDevice to " << td;
  if (this->m_TrackingDevice.GetPointer() != td)
  {
    this->m_TrackingDevice = td;
    this->CreateOutputs();
    std::stringstream name; // create a human readable name for the source
    name << td->GetData().Model << " Tracking Source";
    this->SetName(name.str());
  }
}

mitk::TrackingDevice::Pointer mitk::TrackingDeviceSource::GetTrackingDevice()
{
  return m_TrackingDevice;
}

void mitk::TrackingDeviceSource::CreateOutputs(){
  //if outputs are set then delete them
  if (this->GetNumberOfOutputs() > 0)
  {
    for (int numOP = this->GetNumberOfOutputs() -1; numOP >= 0; numOP--)
      this->RemoveOutput(numOP);
    this->Modified();
  }

  //fill the outputs if a valid tracking device is set
  if (m_TrackingDevice.IsNull())
    return;

  this->SetNumberOfIndexedOutputs(m_TrackingDevice->GetToolCount());  // create outputs for all tools
  unsigned int numberOfOutputs = this->GetNumberOfIndexedOutputs();
  MITK_DEBUG << "Number of tools at start of method CreateOutputs(): " << m_TrackingDevice->GetToolCount();
  MITK_DEBUG << "Number of outputs at start of method CreateOutputs(): " << numberOfOutputs;
  for (unsigned int idx = 0; idx < m_TrackingDevice->GetToolCount(); ++idx)
  {
    if (this->GetOutput(idx) == nullptr)
    {
      DataObjectPointer newOutput = this->MakeOutput(idx);
      static_cast<mitk::NavigationData*>(newOutput.GetPointer())->SetName(m_TrackingDevice->GetTool(idx)->GetToolName()); // set NavigationData name to ToolName
      this->SetNthOutput(idx, newOutput);
      this->Modified();
    }
  }
}

void mitk::TrackingDeviceSource::Connect()
{
  if (m_TrackingDevice.IsNull())
    throw std::invalid_argument("mitk::TrackingDeviceSource: No tracking device set");
  if (this->IsConnected())
    return;
  try
  {
    //Try to open the connection. If it didn't work (fals is returned from OpenConnection by the tracking device), throw an exception.
    if (!m_TrackingDevice->OpenConnection())
    {
      mitkThrowException(mitk::IGTHardwareException) << "Could not open connection.";
    }
  }
  catch (mitk::IGTException &e)
  {
    throw std::runtime_error(std::string("mitk::TrackingDeviceSource: Could not open connection to tracking device. Error: ") + e.GetDescription());
  }
}

void mitk::TrackingDeviceSource::StartTracking()
{
  if (m_TrackingDevice.IsNull())
    throw std::invalid_argument("mitk::TrackingDeviceSource: No tracking device set");
  if (m_TrackingDevice->GetState() == mitk::TrackingDevice::Tracking)
    return;
  if (m_TrackingDevice->StartTracking() == false)
    throw std::runtime_error("mitk::TrackingDeviceSource: Could not start tracking");
}

void mitk::TrackingDeviceSource::Disconnect()
{
  if (m_TrackingDevice.IsNull())
    throw std::invalid_argument("mitk::TrackingDeviceSource: No tracking device set");
  if (m_TrackingDevice->CloseConnection() == false)
    throw std::runtime_error("mitk::TrackingDeviceSource: Could not close connection to tracking device");
}

void mitk::TrackingDeviceSource::StopTracking()
{
  if (m_TrackingDevice.IsNull())
    throw std::invalid_argument("mitk::TrackingDeviceSource: No tracking device set");
  if (m_TrackingDevice->StopTracking() == false)
    throw std::runtime_error("mitk::TrackingDeviceSource: Could not stop tracking");
}

void mitk::TrackingDeviceSource::UpdateOutputInformation()
{
  if(this->GetTrackingDevice()->GetToolCount() != this->GetNumberOfIndexedOutputs())
    this->CreateOutputs();

  this->Modified();  // make sure that we need to be updated
  Superclass::UpdateOutputInformation();
}

void mitk::TrackingDeviceSource::TransferCoordsFromTrackedObjectToTrackingDevice(std::string referenceToolName,
  AffineTransform3D::Pointer coordsInTrackedObject,
  AffineTransform3D::Pointer coordsInTrackingDevice)
{
  AffineTransform3D::Pointer toolRegistrationMatrix = GetToolMetaData(referenceToolName)->GetToolRegistrationMatrix();

  if (this->GetOutput(referenceToolName)==nullptr)
  {
    throw std::invalid_argument("can not get named output");
    return;
  }
  mitk::AffineTransform3D::Pointer T_trackingdevice_2_rf = this->GetOutput(referenceToolName)->GetAffineTransform3D();

  //if there is no named tool,this will return a identity matrix
  mitk::AffineTransform3D::Pointer T_rf_2_trackedobj = GetToolMetaData(referenceToolName)->GetToolRegistrationMatrix();

  // T_trackingDevice = T_trackingdevice_2_rf * T_rf_2_trackedobj * T_trackedobj
  coordsInTrackingDevice = coordsInTrackedObject->Clone();
  coordsInTrackingDevice->Compose(T_rf_2_trackedobj);
  coordsInTrackingDevice->Compose(T_trackingdevice_2_rf);
}

void mitk::TrackingDeviceSource::TransferCoordsFromTrackingDeviceToTrackedObject(std::string referenceToolName,
  AffineTransform3D::Pointer coordsInTrackingDevice,
  AffineTransform3D::Pointer coordsInTrackedObject)
{
  AffineTransform3D::Pointer toolRegistrationMatrix = GetToolMetaData(referenceToolName)->GetToolRegistrationMatrix();

  if (this->GetOutput(referenceToolName) == nullptr)
  {
    throw std::invalid_argument("can not get named output");
    return;
  }
  mitk::AffineTransform3D::Pointer T_trackingdevice_2_rf = this->GetOutput(referenceToolName)->GetAffineTransform3D();
  T_trackingdevice_2_rf->GetInverse(T_trackingdevice_2_rf);
  // if there is no named tool,this will return a identity matrix
  mitk::AffineTransform3D::Pointer T_rf_2_trackedobj = GetToolMetaData(referenceToolName)->GetToolRegistrationMatrix()->Clone();

  T_rf_2_trackedobj->GetInverse(T_rf_2_trackedobj);
  // T_trackedobj = T_trackedobj_2_rf * T_rf_2_trackingdevice * T_trackingDevice
  coordsInTrackedObject = coordsInTrackingDevice->Clone();
  coordsInTrackedObject->Compose(T_trackingdevice_2_rf);
  coordsInTrackedObject->Compose(T_rf_2_trackedobj);
}

//unsigned int mitk::TrackingDeviceSource::GetToolCount()
//{
//  if (m_TrackingDevice)
//    return m_TrackingDevice->GetToolCount();
//  return 0;
//}

bool mitk::TrackingDeviceSource::IsConnected()
{
  if (m_TrackingDevice.IsNull())
    return false;

  return (m_TrackingDevice->GetState() == mitk::TrackingDevice::Ready) || (m_TrackingDevice->GetState() == mitk::TrackingDevice::Tracking);
}

bool mitk::TrackingDeviceSource::IsTracking()
{
  if (m_TrackingDevice.IsNull())
    return false;

  return m_TrackingDevice->GetState() == mitk::TrackingDevice::Tracking;
}
