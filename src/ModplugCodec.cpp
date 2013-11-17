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

#include "xbmc/libXBMC_addon.h"

extern "C" {
#include <libmodplug/modplug.h>
#include "xbmc/xbmc_audiodec_dll.h"
#include "xbmc/AEChannelData.h"

ADDON::CHelper_libXBMC_addon *XBMC           = NULL;

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
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

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Stop()
{
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Destroy()
{
  XBMC=NULL;
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
bool ADDON_HasSettings()
{
  return false;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_GetStatus()
{
  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

void ADDON_FreeSettings()
{
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  return ADDON_STATUS_OK;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}

void* Init(const char* strFile, unsigned int filecache, int* channels,
           int* samplerate, int* bitspersample, int64_t* totaltime,
           int* bitrate, AEDataFormat* format, const AEChannel** channelinfo)
{
  void* file = XBMC->OpenFile(strFile,0);
  if (!file)
    return NULL;

  int len = XBMC->GetFileLength(file);
  char *data = new char[len];
  XBMC->ReadFile(file, data, len);
  XBMC->CloseFile(file);

  // Now load the module
  ModPlugFile* module = ModPlug_Load(data, len);
  delete[] data;

  if (!module)
    return NULL;

  *channels = 2;
  *samplerate = 44100;
  *bitspersample = 16;
  *totaltime = (int64_t)(ModPlug_GetLength(module));
  *format = AE_FMT_S16NE;
  *channelinfo = NULL;
  *bitrate = ModPlug_NumChannels(module);

  return module;
}

int ReadPCM(void* context, uint8_t* pBuffer, int size, int *actualsize)
{
  if (!context)
    return 1;

  if ((*actualsize = ModPlug_Read((ModPlugFile*)context, pBuffer, size)) == size)
    return 0;
  
  return 1;
}

int64_t Seek(void* context, int64_t time)
{
  ModPlug_Seek((ModPlugFile*)context, (int)time);
  return time;
}

bool DeInit(void* context)
{
  ModPlug_Unload((ModPlugFile*)context);

  return true;
}

bool ReadTag(const char* strFile, char* title, char* artist,
             int* length)
{
  return false;
}

int TrackCount(const char* strFile)
{
  return 1;
}
}
