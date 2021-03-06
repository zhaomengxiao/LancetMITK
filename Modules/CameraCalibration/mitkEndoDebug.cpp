/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#include "mitkEndoDebug.h"
#include <itksys/SystemTools.hxx>
#include <mutex>
#include <fstream>
#include <ctime>
#include <cstdio>

namespace mitk
{
  struct EndoDebugData
  {
    EndoDebugData()
      : m_DebugEnabled(false)
      , m_ShowImagesInDebug(false)
      , m_ShowImagesTimeOut(false)
      , m_DebugImagesOutputDirectory("")
    {

    }

    std::set<std::string> m_FilesToDebug;
    std::set<std::string> m_SymbolsToDebug;
    bool m_DebugEnabled;
    bool m_ShowImagesInDebug;
    size_t m_ShowImagesTimeOut;
    std::ofstream m_Stream;
    std::mutex m_Mutex;
    std::string m_DebugImagesOutputDirectory;
  };

  EndoDebug::EndoDebug()
    : d ( new EndoDebugData )
  {

  }

  EndoDebug::~EndoDebug()
  {
    if(d->m_Stream.is_open())
      d->m_Stream.close();
    delete d;
  }

  EndoDebug& EndoDebug::GetInstance()
  {
    static EndoDebug instance;
    return instance;
  }

  std::string EndoDebug::GetUniqueFileName( const std::string& dir, const std::string& ext, const std::string& prefix )
  {
    std::stringstream s;
    s.precision( 0 );

    std::string filename;
    int i = 0;
    while( filename.empty() || itksys::SystemTools::FileExists( (dir+"/"+filename).c_str() ) )
    {
      s.str("");
      s << i;
      filename = prefix + s.str() + "." + ext;
      ++i;
    }

    filename = dir+"/"+filename;

    return filename;
  }

  std::string EndoDebug::GetFilenameWithoutExtension(const std::string& s)
  {
    return itksys::SystemTools::GetFilenameWithoutExtension( s );
  }

  bool EndoDebug::AddFileToDebug(const std::string& s)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    std::pair<std::set<std::string>::iterator, bool> res = d->m_FilesToDebug.insert( s );
    return res.second;
  }

  void EndoDebug::SetFilesToDebug(const std::set<std::string> &filesToDebug)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_FilesToDebug = filesToDebug;
  }

  std::set<std::string> EndoDebug::GetFilesToDebug()
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_FilesToDebug;
  }

  bool EndoDebug::AddSymbolToDebug(const std::string& symbolToDebug)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    std::pair<std::set<std::string>::iterator, bool> res = d->m_SymbolsToDebug.insert( symbolToDebug );
    return res.second;
  }

  void EndoDebug::SetSymbolsToDebug(const std::set<std::string> &symbolsToDebug)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_SymbolsToDebug = symbolsToDebug;
  }

  std::set<std::string> EndoDebug::GetSymbolsToDebug()
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_SymbolsToDebug;
  }

  bool EndoDebug::DebugSymbol(const std::string& s)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_SymbolsToDebug.find(s)
        != d->m_SymbolsToDebug.end();
  }

  bool EndoDebug::DebugFile(const std::string& s)
  {
    std::string filename = GetFilenameWithoutExtension(s);

    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_FilesToDebug.find(filename)
        != d->m_FilesToDebug.end();
  }

  bool EndoDebug::Debug( const std::string& fileToDebug, const std::string& symbol )
  {
    bool debug = false;

    {
      bool debugEnabled = false;
      size_t filesSize = 0;
      size_t symbolsSize = 0;
      bool symbolFound = false;
      {
        std::lock_guard<std::mutex> lock(d->m_Mutex);
        debugEnabled = d->m_DebugEnabled;
        filesSize = d->m_FilesToDebug.size();
        symbolsSize = d->m_SymbolsToDebug.size();
        symbolFound = d->m_SymbolsToDebug.find(symbol) != d->m_SymbolsToDebug.end();
      }

      if( debugEnabled )
      {
        if( filesSize  == 0 )
          debug = true;
        else
          debug = DebugFile(fileToDebug);

        // ok debug is determined so far, now check if symbol set
        if( symbolsSize > 0 )
        {
          debug = symbolFound;
        }
        else
        {
          // do not show symbol debug output if no symbols are set at all
          if( !symbol.empty() )
            debug = false;
        }
      }
    }

    return debug;
  }

  void EndoDebug::SetDebugEnabled(bool _DebugEnabled)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_DebugEnabled = _DebugEnabled;
  }

  void EndoDebug::SetDebugImagesOutputDirectory(const std::string& _DebugImagesOutputDirectory)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_DebugImagesOutputDirectory = _DebugImagesOutputDirectory;
  }

  bool EndoDebug::GetDebugEnabled()
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_DebugEnabled;
  }

  void EndoDebug::SetShowImagesInDebug(bool _ShowImagesInDebug)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_ShowImagesInDebug = _ShowImagesInDebug;
  }

  bool EndoDebug::GetShowImagesInDebug()
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_ShowImagesInDebug;
  }

  void EndoDebug::SetShowImagesTimeOut(size_t _ShowImagesTimeOut)
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_ShowImagesTimeOut = _ShowImagesTimeOut;
  }

  std::string EndoDebug::GetDebugImagesOutputDirectory() const
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_DebugImagesOutputDirectory;
  }

  size_t EndoDebug::GetShowImagesTimeOut()
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    return d->m_ShowImagesTimeOut;
  }

  void EndoDebug::SetLogFile( const std::string& file )
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    d->m_Stream.open ( file.c_str(), std::ios::out | std::ios::app);
  }

  void EndoDebug::ShowMessage( const std::string& message )
  {
    std::lock_guard<std::mutex> lock(d->m_Mutex);
    if(d->m_Stream.is_open())
    {
      char *timestr;
      struct tm *newtime;
      time_t aclock;
      time(&aclock);
      newtime = localtime(&aclock);
      timestr = asctime(newtime);

      d->m_Stream << timestr << ", " << message;
    }
    else
      std::cout << message  << std::flush;
  }
}
