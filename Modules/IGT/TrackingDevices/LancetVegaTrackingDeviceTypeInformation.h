#ifndef LANCET_VEGA_TRACKING_DEVICE_TYPE_INFORMATION_H
#define LANCET_VEGA_TRACKING_DEVICE_TYPE_INFORMATION_H
// mitk
#include <mitkTrackingDeviceTypeInformation.h>

namespace mitk
{

    class MITKIGT_EXPORT LancetVegaTrackingDeviceTypeInformation : public mitk::TrackingDeviceTypeInformation
    {
    public:
        LancetVegaTrackingDeviceTypeInformation();
        mitk::TrackingDeviceSource::Pointer CreateTrackingDeviceSource(mitk::TrackingDevice::Pointer trackingDevice,
            mitk::NavigationToolStorage::Pointer navigationTools,
            std::string* errorMessage,
            std::vector<int>* toolCorrespondencesInToolStorage) override;
        static std::string GetTrackingDeviceName();
        static mitk::TrackingDeviceData GetDeviceDataLancetVegaTrackingDevice();
    };
}
#endif //!LANCET_VEGA_TRACKING_DEVICE_TYPE_INFORMATION_H