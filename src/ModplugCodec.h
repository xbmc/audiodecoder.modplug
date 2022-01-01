/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

//! \file ModplugCodec.h
//! \author Arne Morten Kvarving
//! \brief Modplug audio decoder for Kodi

#pragma once

#include <kodi/addon-instance/AudioDecoder.h>

extern "C"
{
#include <libmodplug/modplug.h>
}

class ATTR_DLL_LOCAL CModplugCodec : public kodi::addon::CInstanceAudioDecoder
{
public:
  CModplugCodec(const kodi::addon::IInstanceInfo& instance);
  ~CModplugCodec() override;

  bool Init(const std::string& filename,
            unsigned int filecache,
            int& channels,
            int& samplerate,
            int& bitspersample,
            int64_t& totaltime,
            int& bitrate,
            AudioEngineDataFormat& format,
            std::vector<AudioEngineChannel>& channellist) override;
  int ReadPCM(uint8_t* buffer, size_t size, size_t& actualsize) override;
  int64_t Seek(int64_t time) override;

private:
  ModPlugFile* m_module = nullptr;
};
