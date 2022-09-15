#ifndef LANCET_VEGA_TRACKING_DEVICE_H
#define LANCET_VEGA_TRACKING_DEVICE_H
// mitk
#include <mitkTrackingDevice.h>
#include "mitkNDIPassiveTool.h"
#include "CombinedApi.h"

namespace lancet
{
  /** Documentation
    * \brief superclass for specific NDI tracking Devices that use socket communication.
    *
    * implements the TrackingDevice interface for NDI tracking devices (VEGA)
    *
    * \ingroup IGT
    */
  class MITKIGT_EXPORT NDIVegaTrackingDevice : public mitk::TrackingDevice
  {
  public:
    mitkClassMacro(NDIVegaTrackingDevice, TrackingDevice);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    typedef std::vector<mitk::NDIPassiveTool::Pointer> Tool6DContainerType;
    ///< List of 6D tools of the correct type for this tracking device
    typedef mitk::NDIPassiveTool::TrackingPriority TrackingPriority; ///< Tracking priority used for tracking a tool

    /**
     * @brief Opens the connection to the device. This have to be done before the tracking is started.
     */
    bool OpenConnection() override;

    /**
     * @brief Closes the connection and clears all resources.
     */
    bool CloseConnection() override;
    /**
     * @brief Set tool track frequency.
     * @param
     * 0 = 1/3 of the frame frequency(20hz in vega)
     *
     * 1 = 1/2 of the frame frequency(30hz in vega)
     *
     * 2 = 1/1 of the frame frequency(60hz in vega)
     *
     * @return Returns true if the TrackingFrequency is set. Returns false if there was an error.
     */
    bool SetTrackingFrequency(unsigned int n);

    /**
      * @brief Starts the tracking.
      * @return Returns true if the tracking is started. Returns false if there was an error.
      */
    bool StartTracking() override;
    /**
     * \return Returns the number of tools which have been added to the device.
     */
    unsigned int GetToolCount() const override;

    /**
     * \param toolNumber The number of the tool which should be given back.
     * \return Returns the tool which the number "toolNumber". Returns nullptr, if there is
     * no tool with this number.
     */
    mitk::TrackingTool *GetTool(unsigned int toolNumber) const override;

    mitk::TrackingTool *GetToolByName(std::string name) const override;

    mitk::NDIPassiveTool *GetInternalTool(std::string portHandle);
    ///< returns the tool object that has been assigned the port handle or nullptr if no tool can be found

    mitk::TrackingTool *AddTool(const char *toolName, const char *fileName, TrackingPriority p = mitk::NDIPassiveTool::Dynamic);

    bool RemoveTool(mitk::TrackingTool *pTool);
    void ClearTool();

    mitk::OperationMode GetOperationMode() const;
  public:

    /**
    * \brief TrackTools() continuously polls serial interface for new 6d tool positions until StopTracking is called.
    *
    * Continuously tracks the 6D position of all tools until StopTracking() is called.
    * This function is executed by the tracking thread (through StartTracking() and ThreadStartTracking()).
    * It should not be called directly.
    * @throw mitk::IGTHardwareException Throws an exception if there are errors while tracking the tools.
    */
    virtual void TrackTools();

    /**
    * \brief continuously polls serial interface for new 3D marker positions until StopTracking is called.
    *
    * Continuously tracks the 3D position of all markers until StopTracking() is called.
    * This function is executed by the tracking thread (through StartTracking() and ThreadStartTracking()).
    * It should not be called directly.
    */
    //todo virtual void TrackMarkerPositions();

    /**
    * \brief continuously polls serial interface for new 3D marker positions and 6D tool positions until StopTracking is called.
    *
    * Continuously tracks the 3D position of all markers and the 6D position of all tools until StopTracking() is called.
    * This function is executed by the tracking thread (through StartTracking() and ThreadStartTracking()).
    * It should not be called directly.
    */
    //todo virtual void TrackToolsAndMarkers();
    /**
    * \brief static start method for the tracking thread.
    */
    void ThreadStartTracking();

    //itkGetConstMacro(Vega, VegaThread*);

    itkGetConstMacro(TrackingFrequencyPara, unsigned int);
    itkSetMacro(hostName, std::string); ///< set host-name for socket communication
    itkGetConstMacro(hostName, std::string);
  private:
    bool determineApiSupportForBX2();
    bool warningOrError(int code, const char *message) const;

  protected:
    NDIVegaTrackingDevice();
    virtual ~NDIVegaTrackingDevice() override;
    /**
    * \brief Get number of supported tracking volumes, a vector containing the supported volumes and
    * a vector containing the signed dimensions in mm. For each volume 10 boundaries are stored in the order of
    * the supported volumes (see AURORA API GUIDE: SFLIST p.54).
    **/

    //todo
    /*virtual bool GetSupportedVolumes(unsigned int *numberOfVolumes,
                                     NDITrackingVolumeContainerType *volumes,
                                     TrackingVolumeDimensionType *volumesDimensions);*/

    /**
    * \brief Sets the desired tracking volume. Returns true if the volume type could be set. It is set in the OpenConnection() Method and sets the tracking volume out of m_Data.
     * @throw mitk::IGTHardwareException Throws an IGT hardware exception if the volume could not be set.
    **/
    //virtual bool SetVolume(mitk::TrackingDeviceData volume);

    /**
    * \brief Add a passive 6D tool to the list of tracked tools. This method is used by AddTool
    * @throw mitk::IGTHardwareException Throws an exception if there are errors while adding the tool.
    * \warning adding tools is not possible in tracking mode, only in setup and ready.
    */
    virtual bool InternalAddTool(mitk::NDIPassiveTool *tool);

    std::string intToString(int input, int width) const;

    //VegaThread *m_Vega = nullptr;
    Tool6DContainerType m_6DTools; ///< container for all tracking tools

    /**
     * @brief Tracking Frequency Parameter.
     *
     * 0 = 1/3 of the frame frequency(20hz in vega)
     *
     * 1 = 1/2 of the frame frequency(30hz in vega)
     *
     * 2 = 1/1 of the frame frequency(60hz in vega)
     *
     */
    unsigned int m_TrackingFrequencyPara = 2;

    CombinedApi m_capi;
    std::string m_DeviceName;
    std::string m_hostName;

    mutable std::mutex m_ToolsMutex; ///< mutex for coordinated access of tool container

    ///< creates tracking thread that continuously polls serial interface for new tracking data
    std::thread m_Thread;                          ///< ID of tracking thread
    mitk::OperationMode m_OperationMode;           ///< tracking mode (6D tool tracking, 3D marker tracking,...)
    std::mutex m_MarkerPointsMutex;                ///< mutex for marker point data container
    mitk::MarkerPointContainerType m_MarkerPoints; ///< container for markers (3D point tracking mode)
  };
}
#endif //!LANCET_VEGA_TRACKING_DEVICE_H 
