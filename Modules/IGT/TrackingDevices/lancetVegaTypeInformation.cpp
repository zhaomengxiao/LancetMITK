#include "lancetVegaTypeInformation.h"
#include "lancetVegaTrackingDevice.h"

namespace lancet
{
    std::string NDIVegaTypeInformation::GetTrackingDeviceName()
    {
        return "Vega";
    }

    mitk::TrackingDeviceData NDIVegaTypeInformation::GetDeviceDataLancetVegaTrackingDevice()
    {
        mitk::TrackingDeviceData data = { NDIVegaTypeInformation::GetTrackingDeviceName(), "Vega", "NDIPolarisSpectraExtendedPyramid.stl", "X" };
        return data;
    }

    NDIVegaTypeInformation::NDIVegaTypeInformation()
    {
        this->m_DeviceName = NDIVegaTypeInformation::GetTrackingDeviceName();
        this->m_TrackingDeviceData.push_back(NDIVegaTypeInformation::GetDeviceDataLancetVegaTrackingDevice());
    }

    mitk::TrackingDeviceSource::Pointer NDIVegaTypeInformation::CreateTrackingDeviceSource(
        mitk::TrackingDevice::Pointer trackingDevice,
        mitk::NavigationToolStorage::Pointer navigationTools,
        std::string* errorMessage,
        std::vector<int>* toolCorrespondencesInToolStorage)
    {
        
        mitk::TrackingDeviceSource::Pointer returnValue = mitk::TrackingDeviceSource::New();
        NDIVegaTrackingDevice::Pointer thisDevice = dynamic_cast<lancet::NDIVegaTrackingDevice*>(trackingDevice.GetPointer());
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