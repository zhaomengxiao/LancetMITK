#include "LancetVegaTrackingDevice.h"
#include "mitkIGTHardwareException.h"
#include "mitkIGTTimeStamp.h"
#include "LancetVegaTrackingDeviceTypeInformation.h"
#include "mitkNDIPassiveTool.h"
//#include "IGTController.h"
// vtk


namespace mitk
{
  LancetVegaTrackingDevice::LancetVegaTrackingDevice()
    : TrackingDevice(),
      m_DeviceName("LancetVega"),
      m_hostName("192.168.1.10"),
      m_OperationMode(ToolTracking6D)
  {
    m_Data = LancetVegaTrackingDeviceTypeInformation::GetDeviceDataLancetVegaTrackingDevice();
    m_6DTools.clear();
    m_MarkerPoints.reserve(50); // a maximum of 50 marker positions can be reported by the tracking device
  }

  LancetVegaTrackingDevice::~LancetVegaTrackingDevice()
  {
    if (this->GetState() == Tracking)
    {
      this->StopTracking();
    }
    if (this->GetState() == Ready)
    {
      this->CloseConnection();
    }
  }

  /*bool LancetVegaTrackingDevice::SetVolume(mitk::TrackingDeviceData volume)
  {
      if (m_DeviceProtocol->VSEL(volume) != mitk::NDIOKAY)
      {
          mitkThrowException(mitk::IGTHardwareException) << "Could not set volume!";
      }
      return true;
  }*/

  bool LancetVegaTrackingDevice::InternalAddTool(NDIPassiveTool *tool)
  {
    MITK_INFO << "InternalAddTool Called!!!!";
    if (tool == nullptr)
      return false;
    NDIPassiveTool::Pointer p = tool;
    //p->Enable();
    int returnValue;
    /* if the connection to the tracking device is already established, add the new tool to the device now */
    if (this->GetState() == Ready)
    {
      MITK_INFO << "State Ready";
      // Request a port handle to load a passive tool into
      int portHandle = m_capi.portHandleRequest();

      p->SetPortHandle(intToString(portHandle, 2));
      // Load the .rom file using the previously obtained port handle
      m_capi.loadSromToPort(p->GetFile(), portHandle);

      /* initialize the port handle */
      returnValue = m_capi.portHandleInitialize(intToString(portHandle, 2));
      if (warningOrError(returnValue, "capi.portHandleInitialize()"))
      {
        return false;
      }
      /* enable the port handle */
      if (p->IsEnabled() == true)
      {
        MITK_INFO << "enable portHandle";
        returnValue = m_capi.portHandleEnable(p->GetPortHandle());
        if (warningOrError(returnValue, "capi.portHandleInitialize()"))
        {
          return false;
        }
      }
      /* now that the tool is added to the device, add it to list too */
      m_ToolsMutex.lock();
      this->m_6DTools.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else if (this->GetState() == Setup)
    {
      MITK_INFO << "State Setup";
      /* In Setup mode, we only add it to the list, so that OpenConnection() can add it later */
      m_ToolsMutex.lock();
      this->m_6DTools.push_back(p);
      m_ToolsMutex.unlock();
      this->Modified();
      return true;
    }
    else // in Tracking mode, no tools can be added
      return false;
  }

  std::string LancetVegaTrackingDevice::intToString(int input, int width) const
  {
    std::stringstream convert;
    convert << std::dec << std::setfill('0');
    convert << std::setw(width) << input;
    return convert.str();
  }

  bool LancetVegaTrackingDevice::StartTracking()
  {
    if (this->GetState() != Ready)
      return false;

    this->SetState(Tracking);          // go to mode Tracking
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    this->m_StopTracking = false;
    this->m_StopTrackingMutex.unlock();

    m_Thread = std::thread(&LancetVegaTrackingDevice::ThreadStartTracking, this);
    // start a new thread that executes the TrackTools() method
    mitk::IGTTimeStamp::GetInstance()->Start(this);
    MITK_INFO << "lancet vega start tracking";
    return true;
  }

  bool LancetVegaTrackingDevice::OpenConnection()
  {
    if (this->GetState() != Setup)
    {
      mitkThrowException(mitk::IGTException) << "Can only try to open the connection if in setup mode";
    }
    int errorCode;
    errorCode = this->m_capi.connect(m_hostName);
    if (this->warningOrError(errorCode, "capi.connect()"))
    {
      mitkThrowException(mitk::IGTHardwareException) << "Can not connect to " << m_hostName;
      return false;
    }
    // Print the firmware version for debugging purposes
    MITK_INFO<< m_capi.getUserParameter("Features.Firmware.Version");

    // Determine if the connected device supports the BX2 command
    determineApiSupportForBX2();

    // Initialize the system. This clears all previously loaded tools, unsaved settings etc...
    errorCode = this->m_capi.initialize();
    if (this->warningOrError(errorCode, "capi.initialize()"))
    {
      mitkThrowException(mitk::IGTHardwareException) << "initialize failed " << m_hostName;
      return false;
    }
    // Print the frame  for debugging purposes
    MITK_INFO << "NDI PARAMETER:" << m_capi.getUserParameter("Param.Tracking.Frame Frequency");
    
    errorCode = m_capi.setUserParameter("Param.Tracking.Track Frequency", std::to_string(m_TrackingFrequencyPara));
    if (this->warningOrError(errorCode, "capi.setUserParameter()"))
    {
      mitkThrowException(mitk::IGTHardwareException) << "setUserParameter failed " << m_hostName;
        return false;
    }
    // Print the frame  for debugging purposes
    MITK_INFO << "NDI PARAMETER:" << m_capi.getUserParameter("Param.Tracking.Track Frequency");
    /**
* POLARIS: initialize the tools that were added manually
**/
    {
      std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
      int returnValue;
      auto endIt = m_6DTools.end();
      for (auto it = m_6DTools.begin(); it != endIt; ++it)
      {
        /* get a port handle for the tool */
        int portHandle = m_capi.portHandleRequest();
        if (!warningOrError(portHandle, "capi.portHandleRequest()"))
        {
          (*it)->SetPortHandle(intToString(portHandle, 2));
          /* now write the SROM file of the tool to the tracking system using PVWR */
          if (this->m_Data.Line == mitk::LancetVegaTrackingDeviceTypeInformation::GetTrackingDeviceName())
          {
            m_capi.loadSromToPort((*it)->GetFile(), portHandle);

            returnValue = m_capi.portHandleInitialize(intToString(portHandle, 2));
            if (warningOrError(returnValue, "capi.portHandleInitialize()"))
            {
              mitkThrowException(mitk::IGTHardwareException) << "portHandleInitialize failed " << m_hostName;
              return false;
            }

            if ((*it)->IsEnabled() == true)
            {
              MITK_INFO << "enable portHandle";
              returnValue = m_capi.portHandleEnable((*it)->GetPortHandle());
              if (warningOrError(returnValue, "capi.portHandleEnable()"))
              {
                mitkThrowException(mitk::IGTHardwareException) << "portHandleEnable failed " << m_hostName;
                return false;
              }
            }
          }
        }
      }
    } // end of toolsmutexlockholder scope

    this->SetState(Ready);

    //SetVolume(this->m_Data);

    return true;
  }

  bool LancetVegaTrackingDevice::CloseConnection()
  {
    if (this->GetState() == Setup)
    {
      return true;
    }

    this->SetState(Setup);
    return true;
  }

  bool LancetVegaTrackingDevice::SetTrackingFrequency(unsigned n)
  {
      if (this->GetState() == Tracking)
          return false;

      itkDebugMacro("setting tracking Frequency to " << n);
         
      
      if(m_TrackingFrequencyPara!=n)
      {
          this->Modified();
          m_TrackingFrequencyPara = n;
          if (this->GetState() == Ready)// if the connection to the tracking system is established, send the new rate to the tracking device too
          {
              int returnValue;
              if (n > 2)
              {
                  returnValue = m_capi.setUserParameter("Param.Tracking.Track Frequency", std::to_string(2));
              }
              else returnValue = m_capi.setUserParameter("Param.Tracking.Track Frequency", std::to_string(n));

              if (warningOrError(returnValue, "capi.setUserParameter(Param.Tracking.Track Frequency)"))
              {
                  return false;
              }
          }
          
      }
      return true;
  }

  unsigned int LancetVegaTrackingDevice::GetToolCount() const
  {
    return static_cast<unsigned int>(this->m_6DTools.size());
  }

  TrackingTool *LancetVegaTrackingDevice::GetTool(unsigned int toolNumber) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    if (toolNumber < m_6DTools.size())
      return m_6DTools.at(toolNumber);
    return nullptr;
  }

  TrackingTool *LancetVegaTrackingDevice::GetToolByName(std::string name) const
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    auto end = m_6DTools.end();
    for (auto iterator = m_6DTools.begin(); iterator != end; ++iterator)
      if (name.compare((*iterator)->GetToolName()) == 0)
        return *iterator;
    return nullptr;
  }

  NDIPassiveTool *LancetVegaTrackingDevice::GetInternalTool(std::string portHandle)
  {
    std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
    auto end = m_6DTools.end();
    for (auto iterator = m_6DTools.begin(); iterator != end; ++iterator)
      if (portHandle.compare((*iterator)->GetPortHandle()) == 0)
        return *iterator;
    return nullptr;
  }

  TrackingTool *LancetVegaTrackingDevice::AddTool(const char *toolName,
                                                  const char *fileName,
                                                  TrackingPriority p /*= NDIPassiveTool::Dynamic*/)
  {
    NDIPassiveTool::Pointer t = NDIPassiveTool::New();
    if (t->LoadSROMFile(fileName) == false)
      return nullptr;
    t->SetToolName(toolName);
    t->SetTrackingPriority(p);
    if (this->InternalAddTool(t) == false)
    {
      MITK_ERROR << "InternalAddTool failed";
      return nullptr;
    }

    return t.GetPointer();
  }

  TrackingTool *LancetVegaTrackingDevice::AddRefTool(unsigned int toolNumber, unsigned int refNumber)
  {
    mitk::NDIPassiveTool::New();
    mitk::NDIPassiveTool::Pointer pTool = mitk::NDIPassiveTool::New();
    pTool->SetToolName(std::to_string(toolNumber) + '\n' + std::to_string(refNumber));
    this->m_6DTools.push_back(pTool);
    return pTool;
  }

  bool LancetVegaTrackingDevice::RemoveTool(mitk::TrackingTool *pTool)

  {
    Tool6DContainerType::const_iterator cit = std::find(
      this->m_6DTools.cbegin(),
      this->m_6DTools.cend(),
      pTool
      );
    if (cit == this->m_6DTools.cend())
    {
      return false;
    }
    this->m_6DTools.erase(cit);
    return true;
  }

  void LancetVegaTrackingDevice::ClearTool()
  {
    this->m_6DTools.clear();
  }

  OperationMode LancetVegaTrackingDevice::GetOperationMode() const
  {
    return m_OperationMode;
  }

  void LancetVegaTrackingDevice::TrackTools()
  {
    MITK_INFO << "tracktools called";
    /* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
    // keep lock until end of scope
    std::lock_guard<std::mutex> lock(m_TrackingFinishedMutex);
    if (this->GetState() != Tracking)
    {
      MITK_INFO << "TrackTools Return: state not tracking";
      return;
    }

    int returnvalue;
    returnvalue = m_capi.startTracking();
    if (warningOrError(returnvalue, "m_capi.startTracking()"))
    {
      MITK_INFO << "TrackTools Return: startTracking warning";
      return;
    }

    bool localStopTracking;
    // Because m_StopTracking is used by two threads, access has to be guarded by a mutex. To minimize thread locking, a local copy is used here
    this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
    localStopTracking = this->m_StopTracking;
    this->m_StopTrackingMutex.unlock();
    while ((this->GetState() == Tracking) && (localStopTracking == false))
    {
      //MITK_INFO << "tracking";
      std::vector<ToolData> dataOfTools = m_capi.getTrackingDataBX();
      if (dataOfTools.empty())
      {
        MITK_INFO << "dataOfTools.empty()";
        break;
      }

      for (auto toolData : dataOfTools)
      {
        NDIPassiveTool::Pointer tool = GetInternalTool(intToString(toolData.transform.toolHandle, 2));
        mitk::Quaternion quaternion{toolData.transform.qx, toolData.transform.qy, toolData.transform.qz,
                                    toolData.transform.q0};
        tool->SetOrientation(quaternion);
        mitk::Point3D position;
        position[0] = toolData.transform.tx;
        position[1] = toolData.transform.ty;
        position[2] = toolData.transform.tz;
        
        tool->SetPosition(position);
        tool->SetTrackingError(toolData.transform.error);
        tool->SetErrorMessage("");
        tool->SetFrameNumber(toolData.frameNumber);
        
        tool->SetDataValid(!toolData.transform.isMissing());
        
      }
      /* Update the local copy of m_StopTracking */
      this->m_StopTrackingMutex.lock();
      localStopTracking = m_StopTracking;
      this->m_StopTrackingMutex.unlock();
    }
    /* StopTracking was called, thus the mode should be changed back to Ready now that the tracking loop has ended. */

    returnvalue = m_capi.stopTracking();
    if (warningOrError(returnvalue, "m_capi.stopTracking()"))
    {
      MITK_INFO << "m_capi.stopTracking() err";
      return;
    }
    MITK_INFO << "m_capi stopTracking()";
    return;
    // returning from this function (and ThreadStartTracking()) this will end the thread and transfer control back to main thread by releasing trackingFinishedLockHolder
  }

  void LancetVegaTrackingDevice::ThreadStartTracking()
  {
    MITK_INFO << "threadStartTracking Called";
    if (this->GetOperationMode() == ToolTracking6D)
      this->TrackTools(); // call TrackTools() from the original object
                                    // else if (trackingDevice->GetOperationMode() == MarkerTracking3D)
    // trackingDevice->TrackMarkerPositions(); // call TrackMarkerPositions() from the original object
    // else if (trackingDevice->GetOperationMode() == ToolTracking5D)
    // trackingDevice->TrackMarkerPositions(); // call TrackMarkerPositions() from the original object
    // else if (trackingDevice->GetOperationMode() == HybridTracking)
    //{
    //  trackingDevice->TrackToolsAndMarkers();
    //}
  }

  bool mitk::LancetVegaTrackingDevice::determineApiSupportForBX2()
  {
    // Lookup the API revision
    std::string response = m_capi.getApiRevision();

    // Refer to the API guide for how to interpret the APIREV response
    char deviceFamily = response[0];
    int majorVersion = m_capi.stringToInt(response.substr(2, 3));

    // As of early 2017, the only NDI device supporting BX2 is the Vega
    // Vega is a Polaris device with API major version 003
    if (deviceFamily == 'G' && majorVersion >= 3)
    {
      MITK_INFO << "BX2 supported";
      return true;
    }
    MITK_INFO << "BX2 unsupported";
    return false;
  }

  bool LancetVegaTrackingDevice::warningOrError(int code, const char *message) const
  {
    if (code >= 0)
    {
      return false;
    }
    /*std::string errStr = CombinedApi::errorToString(code);
    MITK_WARN(message) << errStr;
    return true;*/
    //std::string codeToString = CombinedApi::errorToString(code);
    code *= -1; // restore the errorCode to a positive value
    if (code > 1000)
    {
      std::string errString = CombinedApi::errorToString(code *= -1);
      MITK_WARN(message) << code << errString;
      return false;
    }
    else
    {
      std::string errString = CombinedApi::errorToString(code *= -1);
      MITK_ERROR(message) << code << errString;
      return true;
    }
  }
}
