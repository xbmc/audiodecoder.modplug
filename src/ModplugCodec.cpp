/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
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
  CModplugCodec(KODI_HANDLE instance) :
    CInstanceAudioDecoder(instance) {}

  virtual ~CModplugCodec()
  {
    if (module)
      ModPlug_Unload(module);
  }

  virtual bool Init(const std::string& filename, unsigned int filecache,
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

    // Now load the module
    module = ModPlug_Load(data, len);
    delete[] data;

    if (!module)
      return false;

    channels = 2;
    samplerate = 44100;
    bitspersample = 16;
    totaltime = (int64_t)ModPlug_GetLength(module);
    format = AE_FMT_S16NE;
    channellist = { AE_CH_FL, AE_CH_FR };
    bitrate, ModPlug_NumChannels(module);

    return true;
  }

  virtual int ReadPCM(uint8_t* buffer, int size, int& actualsize) override
  {
    if (!module)
      return 1;

    if ((actualsize = ModPlug_Read(module, buffer, size)) == size)
      return 0;
  
    return 1;
  }

  virtual int64_t Seek(int64_t time) override
  {
    if (!module)
      return -1;

    ModPlug_Seek(module, (int)time);
    return time;
  }

private:
  ModPlugFile* module = nullptr;
};


class ATTRIBUTE_HIDDEN CMyAddon : public kodi::addon::CAddonBase
{
public:
  CMyAddon() { }
  virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance) override
  {
    addonInstance = new CModplugCodec(instance);
    return ADDON_STATUS_OK;
  }
  virtual ~CMyAddon()
  {
  }
};


ADDONCREATOR(CMyAddon);
