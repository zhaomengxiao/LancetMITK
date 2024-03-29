/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef NAVIGATIONTOOLSTORAGE_H_INCLUDED
#define NAVIGATIONTOOLSTORAGE_H_INCLUDED

//itk headers
#include <itkObjectFactory.h>

//mitk headers
#include <mitkCommon.h>
#include <MitkIGTExports.h>
#include "mitkNavigationTool.h"
#include <mitkDataStorage.h>

// Microservices
#include <mitkServiceInterface.h>
#include <usServiceRegistration.h>
#include <usServiceProperties.h>

namespace mitk {
  /**Documentation
  * \brief An object of this class represents a collection of navigation tools.
  *        You may add/delete navigation tools or store/load the whole collection
  *        to/from the harddisc by using the class NavigationToolStorageSerializer
  *        and NavigationToolStorageDeserializer.
  *
  * \ingroup IGT
  */
  class MITKIGT_EXPORT NavigationToolStorage : public itk::Object
  {
  public:
    mitkClassMacroItkParent(NavigationToolStorage,itk::Object);
    /** @brief Constructs a NavigationToolStorage without reference to a DataStorage. The Data Nodes of tools have to be added and removed to a data storage outside this class.
     *         Normaly the other constructor should be used.
     */
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self)
    /** @brief Constructs a NavigationToolStorage with reference to a DataStorage. The Data Nodes of tools are added and removed automatically to this data storage. */
    mitkNewMacro1Param(Self,mitk::DataStorage::Pointer);


    /**
    *\brief Registers this object as a Microservice, making it available to every module and/or plugin.
    * To unregister, call UnregisterMicroservice(). Make sure to pass the id of the Device that this tool is connected to.
    */
    virtual void RegisterAsMicroservice();

    /**
    *\brief Registers this object as a Microservice, making it available to every module and/or plugin.
    */
    virtual void UnRegisterMicroservice();

    /**
    *\brief Returns the id that this device is registered with. The id will only be valid, if the
    * NavigationDataSource has been registered using RegisterAsMicroservice().
    */
    std::string GetMicroserviceID();

    /**
    *\brief These constants are used in conjunction with Microservices
    */
    // Name of the interface
    static const std::string US_INTERFACE_NAME;
    // ID of the NavigationDataSource this ToolStorage is associated with. Can be empty ("") and changed with SetSourceID().
    static const std::string US_PROPKEY_SOURCE_ID;
    // name of the storage
    static const std::string US_PROPKEY_STORAGE_NAME;


    /**
     * @brief  Adds a tool to the storage. Be sure that the tool has a unique
     *         identifier which is not already part of this storage.
     * @return Returns true if the tool was added to the storage, false if not
     *         (false can be returned if the identifier already exists in this storage
     *         for example).
     */
    bool AddTool(mitk::NavigationTool::Pointer tool);

    /**
     * @return Returns the tracking tool at the position "number"
     *         in the storage. Returns nullptr if there is no
     *         tracking tool at this position.
     */
    mitk::NavigationTool::Pointer GetTool(int number);

    /**
     * @return Returns the tracking tool with the given identifier.
     *         Returns nullptr if there is no
     *         tracking tool with this identifier in the storage.
     */
    mitk::NavigationTool::Pointer GetTool(std::string identifier);

    /**
     * @return Returns the tracking tool with the given name.
     *         Returns nullptr if there is no
     *         tracking tool with this name in the storage.
     */
    mitk::NavigationTool::Pointer GetToolByName(std::string name);

    /**
     * @return Returns the tracking tool index with the given name.
     *         Returns -1 if there is no
     *         tracking tool with this name in the storage.
     */
    int GetToolIndexByName(std::string name);

    /** Assigns the given number to the tool with the given identifier. This means the tool is swapped with another tool in the internal tool vector.
     *  @return Returns true if the assignment was successfull. Returns false if assignment is not possible, e.g. because the identifier does not exist or if the given number is not available.
     **/
    bool AssignToolNumber(std::string identifier1, int number2);

    /**
     * @brief Deletes a tool from the collection.
     * Warning, this method operates on the data storage and is not thread save. Calling it from outside the main thread may cause crashes.
     */
    bool DeleteTool(int number);

    /**
     * @brief Deletes all tools from the collection.
     * Warning, this method operates on the data storage and is not thread save. Calling it from outside the main thread may cause crashes.
     */
    bool DeleteAllTools();

    /**
     * @return Returns the number of tools stored in the storage.
     */
    unsigned int GetToolCount();

    /**
     * @return Returns true if the storage is empty, false if not.
     */
    bool isEmpty();

    /**
     * @return Returns the corresponding data storage if one is set to this NavigationToolStorage.
     *         Returns nullptr if none is set.
     */
    itkGetMacro(DataStorage,mitk::DataStorage::Pointer);

    /** Sets the name of this storage. The name should be understandable for the user.
     *  Something like "NDI Aurora Tool Storage". If a storage is loaded from the harddisk
     *  the name might be the filename.
     */
    void SetName(std::string);

    /** @return Returns the name of this storage. */
    std::string GetName() const;

    /** Sets the name of this storage. The name should be understandable for the user.
    *  Something like "NDI Aurora Tool Storage". If a storage is loaded from the harddisk
    *  the name might be the filename.
    *  @warning: if your microservice is already registered, you need to call UpdateMicroservice after changing the ID.
    *  This can't be done inside this functions, as we might use different threads.
    */
    void SetSourceID(std::string);

    /** @return Returns the name of this storage. */
    std::string GetSourceID() const;

    /** Locks the storage. A logged storage may not be modified.
     *  If a method tries to modify the storage anyway a waring message is given.
     *  The storage is unlocked by default. A Storage might be locked when a
     *  tracking device is active and needs the storage to stay consistent.
     */
    void LockStorage();

    /** Unlocks the storage again. */
    void UnLockStorage();

    /** @return Returns true if the storage is locked at the moment, false if not. */
    bool isLocked();

    /** Sets the properties which causes the microservice to emit an update signal. */
    void UpdateMicroservice();

  protected:
    NavigationToolStorage();
    NavigationToolStorage(mitk::DataStorage::Pointer);
    ~NavigationToolStorage() override;

    std::vector<mitk::NavigationTool::Pointer> m_ToolCollection;
    mitk::DataStorage::Pointer m_DataStorage;
    std::string m_Name;
    std::string m_SourceID;
    bool m_storageLocked;

  private:
    us::ServiceRegistration<Self> m_ServiceRegistration;
    us::ServiceProperties m_props;

  };
} // namespace mitk
MITK_DECLARE_SERVICE_INTERFACE(mitk::NavigationToolStorage, "org.mitk.services.NavigationToolStorage")
#endif //NAVIGATIONTOOLSTORAGE
