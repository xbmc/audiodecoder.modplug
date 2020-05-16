/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

//! \file ModplugCodec.cpp
//! \author Arne Morten Kvarving
//! \brief Modplug audio decoder for Kodi

#include <kodi/addon-instance/AudioDecoder.h>
#include <kodi/Filesystem.h>

extern "C" {
#include <libmodplug/modplug.h>
}

class ATTRIBUTE_HIDDEN CModplugCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CModplugCodec(KODI_HANDLE instance, const std::string& version) :
    CInstanceAudioDecoder(instance, version) {}

  ~CModplugCodec() override
  {
    if (m_module)
      ModPlug_Unload(m_module);
  }

  bool Init(const std::string& filename, unsigned int filecache,
            int& channels, int& samplerate,
            int& bitspersample, int64_t& totaltime,
            int& bitrate, AEDataFormat& format,
            std::vector<AEChannel>& channellist) override
  {
    kodi::vfs::CFile file;
    if (!file.OpenFile(filename,0))
      return false;

    int len = file.GetLength();
    char *data = new char[len];
    if (!data)
      return false;

    file.Read(data, len);
    file.Close();

    // Now load the m_module
    m_module = ModPlug_Load(data, len);
    delete[] data;

    if (!m_module)
      return false;

    channels = 2;
    samplerate = 44100;
    bitspersample = 16;
    totaltime = (int64_t)ModPlug_GetLength(m_module);
    format = AE_FMT_S16NE;
    channellist = { AE_CH_FL, AE_CH_FR };
    bitrate, ModPlug_NumChannels(m_module);

    return true;
  }

  int ReadPCM(uint8_t* buffer, int size, int& actualsize) override
  {
    if (!m_module)
      return 1;

    if ((actualsize = ModPlug_Read(m_module, buffer, size)) == size)
      return 0;
  
    return 1;
  }

  int64_t Seek(int64_t time) override
  {
    if (!m_module)
      return -1;

    ModPlug_Seek(m_module, (int)time);
    return time;
  }

private:
  ModPlugFile* m_module = nullptr;
};


class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() = default;
  ADDON_STATUS CreateInstance(int instanceType, const std::string& instanceID, KODI_HANDLE instance, const std::string& version, KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CModplugCodec(instance, version);
    return ADDON_STATUS_OK;
  }
  ~CMyAddon() override = default;
};


ADDONCREATOR(CMyAddon);
