#include "LancetVegaTrackingDeviceTypeInformation.h"
#include "LancetVegaTrackingDevice.h"

namespace mitk
{
    std::string LancetVegaTrackingDeviceTypeInformation::GetTrackingDeviceName()
    {
        return "Vega";
    }

    mitk::TrackingDeviceData LancetVegaTrackingDeviceTypeInformation::GetDeviceDataLancetVegaTrackingDevice()
    {
        mitk::TrackingDeviceData data = { LancetVegaTrackingDeviceTypeInformation::GetTrackingDeviceName(), "Vega", "NDIPolarisSpectraExtendedPyramid.stl", "X" };
        return data;
    }

    LancetVegaTrackingDeviceTypeInformation::LancetVegaTrackingDeviceTypeInformation()
    {
        this->m_DeviceName = LancetVegaTrackingDeviceTypeInformation::GetTrackingDeviceName();
        this->m_TrackingDeviceData.push_back(LancetVegaTrackingDeviceTypeInformation::GetDeviceDataLancetVegaTrackingDevice());
    }

    mitk::TrackingDeviceSource::Pointer LancetVegaTrackingDeviceTypeInformation::CreateTrackingDeviceSource(
        mitk::TrackingDevice::Pointer trackingDevice,
        mitk::NavigationToolStorage::Pointer navigationTools,
        std::string* errorMessage,
        std::vector<int>* toolCorrespondencesInToolStorage)
    {
        
        mitk::TrackingDeviceSource::Pointer returnValue = mitk::TrackingDeviceSource::New();
        LancetVegaTrackingDevice::Pointer thisDevice = dynamic_cast<mitk::LancetVegaTrackingDevice*>(trackingDevice.GetPointer());
        *toolCorrespondencesInToolStorage = std::vector<int>();
        //add the tools to the tracking device
        for (unsigned int i = 0; i < navigationTools->GetToolCount(); i++)
        {
            mitk::NavigationTool::Pointer thisNavigationTool = navigationTools->GetTool(i);
            toolCorrespondencesInToolStorage->push_back(i);
            bool toolAddSuccess = thisDevice->AddTool(thisNavigationTool->GetToolName().c_str(), thisNavigationTool->GetCalibrationFile().c_str());
            if (!toolAddSuccess)
            {
                //todo: error handling
                errorMessage->append("Can't add tool, is the SROM-file valid?");
                return nullptr;
            }
            thisDevice->GetTool(i)->SetToolTipPosition(thisNavigationTool->GetToolTipPosition(), thisNavigationTool->GetToolAxisOrientation());
        }
        returnValue->SetTrackingDevice(thisDevice);
        return returnValue;
    }
}