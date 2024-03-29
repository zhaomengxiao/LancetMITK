/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkNavigationToolStorage.h"

//Microservices
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usModuleContext.h>

const std::string  mitk::NavigationToolStorage::US_INTERFACE_NAME = "org.mitk.services.NavigationToolStorage"; // Name of the interface
const std::string  mitk::NavigationToolStorage::US_PROPKEY_SOURCE_ID = US_INTERFACE_NAME + ".sourceID";
const std::string  mitk::NavigationToolStorage::US_PROPKEY_STORAGE_NAME = US_INTERFACE_NAME + ".name";

mitk::NavigationToolStorage::NavigationToolStorage()
  : m_ToolCollection(std::vector<mitk::NavigationTool::Pointer>()),
    m_DataStorage(nullptr),
  m_storageLocked(false)
{
  this->SetName("ToolStorage (no name given)");
}

mitk::NavigationToolStorage::NavigationToolStorage(mitk::DataStorage::Pointer ds)
  : m_storageLocked(false)
{
  m_ToolCollection = std::vector<mitk::NavigationTool::Pointer>();
  this->m_DataStorage = ds;
  this->SetName("Tool Storage (no name given)");
}

void mitk::NavigationToolStorage::SetName(std::string n)
{
  m_Name = n;
  m_props[US_PROPKEY_STORAGE_NAME] = m_Name;
}

std::string mitk::NavigationToolStorage::GetName() const
{
  return m_Name;
}

void mitk::NavigationToolStorage::UpdateMicroservice()
{
  if (m_ServiceRegistration) { m_ServiceRegistration.SetProperties(m_props); }
}

mitk::NavigationToolStorage::~NavigationToolStorage()
{
  if (m_DataStorage.IsNotNull()) //remove all nodes from the data storage
  {
    for (std::vector<mitk::NavigationTool::Pointer>::iterator it = m_ToolCollection.begin(); it != m_ToolCollection.end(); it++)
      m_DataStorage->Remove((*it)->GetDataNode());
  }
}

void mitk::NavigationToolStorage::RegisterAsMicroservice(){
  // Get Context
  us::ModuleContext* context = us::GetModuleContext();

  // Define ServiceProps
  m_ServiceRegistration = context->RegisterService(this, m_props);
  //Tell all widgets, that there is a new toolStorage registered, e.g. the old one might have changed.
  UpdateMicroservice();
}

void mitk::NavigationToolStorage::UnRegisterMicroservice(){
  if (!m_ServiceRegistration)
  {
    MITK_WARN("NavigationToolStorage")
      << "Cannot unregister microservice as it wasn't registered before.";
    return;
  }

  m_ServiceRegistration.Unregister();
  m_ServiceRegistration = 0;
}

bool mitk::NavigationToolStorage::DeleteTool(int number)
{
  if (m_storageLocked)
  {
    MITK_WARN << "Storage is locked, cannot modify it!";
    return false;
  }

  else if ((unsigned int)number > m_ToolCollection.size())
  {
    MITK_WARN << "Tool no " << number << "doesn't exist, can't delete it!";
    return false;
  }
  std::vector<mitk::NavigationTool::Pointer>::iterator it = m_ToolCollection.begin() + number;
  if (m_DataStorage.IsNotNull())
    m_DataStorage->Remove((*it)->GetDataNode());
  m_ToolCollection.erase(it);

  //This line is important so that other widgets can get a notice that the toolStorage has changed!
  this->UpdateMicroservice();
  return true;
}

bool mitk::NavigationToolStorage::DeleteAllTools()
{
  if (m_storageLocked)
  {
    MITK_WARN << "Storage is locked, cannot modify it!";
    return false;
  }

  while (m_ToolCollection.size() > 0) if (!DeleteTool(0)) return false;
  return true;
}

bool mitk::NavigationToolStorage::AddTool(mitk::NavigationTool::Pointer tool)
{
  if (m_storageLocked)
  {
    MITK_WARN << "Storage is locked, cannot modify it!";
    return false;
  }
  else if (GetTool(tool->GetIdentifier()).IsNotNull())
  {
    MITK_WARN << "Tool ID already exists in storage, can't add!";
    return false;
  }
  else
  {
    m_ToolCollection.push_back(tool);
    if (m_DataStorage.IsNotNull())
    {
      if (!m_DataStorage->Exists(tool->GetDataNode()))
        m_DataStorage->Add(tool->GetDataNode());
    }
    //This line is important so that other widgets can get a notice that the toolStorage has changed!
    this->UpdateMicroservice();
    return true;
  }
}

mitk::NavigationTool::Pointer mitk::NavigationToolStorage::GetTool(int number)
{
  //todo check if number > toolCount
  return m_ToolCollection.at(number);
}

mitk::NavigationTool::Pointer mitk::NavigationToolStorage::GetTool(std::string identifier)
{
  for (unsigned int i = 0; i < GetToolCount(); i++) if ((GetTool(i)->GetIdentifier()) == identifier) return GetTool(i);
  return nullptr;
}

mitk::NavigationTool::Pointer mitk::NavigationToolStorage::GetToolByName(std::string name)
{
  for (unsigned int i = 0; i < GetToolCount(); i++) if ((GetTool(i)->GetToolName()) == name) return GetTool(i);
  return nullptr;
}

int mitk::NavigationToolStorage::GetToolIndexByName(std::string name)
{
  for (unsigned int i = 0; i < GetToolCount(); i++)
    if ((GetTool(i)->GetToolName()) == name)
      return i;
  return -1;
}

unsigned int mitk::NavigationToolStorage::GetToolCount()
{
  return m_ToolCollection.size();
}

bool mitk::NavigationToolStorage::isEmpty()
{
  return m_ToolCollection.empty();
}

void mitk::NavigationToolStorage::LockStorage()
{
  m_storageLocked = true;
}

void mitk::NavigationToolStorage::UnLockStorage()
{
  m_storageLocked = false;
}

bool mitk::NavigationToolStorage::isLocked()
{
  return m_storageLocked;
}

bool mitk::NavigationToolStorage::AssignToolNumber(std::string identifier1, int number2)
{
  if (this->GetTool(identifier1).IsNull())
  {
    MITK_WARN << "Identifier does not exist, cannot assign new number";
    return false;
  }

  if ((number2 >= static_cast<int>(m_ToolCollection.size())) || (number2 < 0))
  {
    MITK_WARN << "Invalid number, cannot assign new number";
    return false;
  }

  mitk::NavigationTool::Pointer tool2 = m_ToolCollection.at(number2);

  int number1 = -1;

  for(int i = 0; i<static_cast<int>(m_ToolCollection.size()); i++)
  {
    if (m_ToolCollection.at(i)->GetIdentifier() == identifier1) { number1 = i; }
  }

  m_ToolCollection[number2] = m_ToolCollection.at(number1);

  m_ToolCollection[number1] = tool2;

  MITK_DEBUG << "Swapped tool " << number2 << " with tool " << number1;

  //This line is important so that other widgets can get a notice that the toolStorage has changed!
  this->UpdateMicroservice();

  return true;
}

void mitk::NavigationToolStorage::SetSourceID(std::string _id)
{
  m_SourceID = _id;
  m_props[US_PROPKEY_SOURCE_ID] = m_SourceID;
}

/** @return Returns the name of this storage. */
std::string mitk::NavigationToolStorage::GetSourceID() const
{
  return m_SourceID;
}
