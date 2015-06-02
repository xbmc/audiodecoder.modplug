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

#include "libXBMC_addon.h"

extern "C" {
#include <libmodplug/modplug.h>
#include "kodi_audiodec_dll.h"
#include "AEChannelData.h"

ADDON::CHelper_libXBMC_addon *XBMC           = NULL;

//! \brief Create addon
//! \details Called on load. Addon should fully initalize or return error status
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!XBMC)
    XBMC = new ADDON::CHelper_libXBMC_addon;

  if (!XBMC->RegisterMe(hdl))
  {
    delete XBMC, XBMC=NULL;
    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  return ADDON_STATUS_OK;
}

//! \brief Stop addon
//! \details This dll must cease all runtime activities
void ADDON_Stop()
{
}

//! \brief Destroy addon
//! \details Do everything before unload of this add-on
void ADDON_Destroy()
{
  XBMC=NULL;
}

//! \brief Declare whether or not the addon has settings 
//! \return true if this add-on use settings
bool ADDON_HasSettings()
{
  return false;
}

//! \brief Get status of addon
//! \details Returns the current Status of this audio decoder
ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//! \brief Returns runtime-generated settings
//! \details Return the settings for XBMC to display
unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//! \brief Free runtime-generated settings
//! \details Free the settings struct passed from XBMC
void ADDON_FreeSettings()
{
}

//! \brief Set the value of a given setting
//! \details Set a specific Setting value (called from XBMC)
ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  return ADDON_STATUS_OK;
}

//! \brief Receive announcements from XBMC
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

#define SET_IF(ptr, value) \
{ \
  if ((ptr)) \
   *(ptr) = (value); \
}

//! \brief Initialize an audio decoder for a given file
void* Init(const char* strFile, unsigned int filecache, int* channels,
           int* samplerate, int* bitspersample, int64_t* totaltime,
           int* bitrate, AEDataFormat* format, const AEChannel** channelinfo)
{
  if (!strFile)
    return NULL;

  void* file = XBMC->OpenFile(strFile,0);
  if (!file)
    return NULL;

  int len = XBMC->GetFileLength(file);
  char *data = new char[len];
  if (!data)
  {
    XBMC->CloseFile(file);
    return NULL;
  }

  XBMC->ReadFile(file, data, len);
  XBMC->CloseFile(file);

  // Now load the module
  ModPlugFile* module = ModPlug_Load(data, len);
  delete[] data;

  if (!module)
    return NULL;

  SET_IF(channels, 2)
  SET_IF(samplerate, 44100)
  SET_IF(bitspersample, 16)
  SET_IF(totaltime,(int64_t)(ModPlug_GetLength(module)));
  SET_IF(format, AE_FMT_S16NE)
  static enum AEChannel map[3] = {
    AE_CH_FL, AE_CH_FR, AE_CH_NULL
  };
  SET_IF(channelinfo, map)
  SET_IF(bitrate, ModPlug_NumChannels(module));

  return module;
}

//! \brief Return decoded data
int ReadPCM(void* context, uint8_t* pBuffer, int size, int *actualsize)
{
  if (!context)
    return 1;

  if ((*actualsize = ModPlug_Read((ModPlugFile*)context, pBuffer, size)) == size)
    return 0;
  
  return 1;
}

//! \brief Seek to a given time
int64_t Seek(void* context, int64_t time)
{
  if (!context)
    return -1;

  ModPlug_Seek((ModPlugFile*)context, (int)time);
  return time;
}

//! \brief Deinitialize decoder
bool DeInit(void* context)
{
  if (!context)
    return true;

  ModPlug_Unload((ModPlugFile*)context);

  return true;
}

//! \brief Returns any tag values for file
bool ReadTag(const char* strFile, char* title, char* artist,
             int* length)
{
  return false;
}

//! \brief Returns track count for a given file
int TrackCount(const char* strFile)
{
  return 1;
}
}
