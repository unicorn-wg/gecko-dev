/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPMEDIASECTION_H_
#define _SDPMEDIASECTION_H_

#include "mozilla/Maybe.h"
#include "signaling/src/sdp/SdpEnum.h"

#include <string>
#include <vector>

namespace mozilla {

class SdpAttributeList;

class SdpConnection;

class SdpMediaSection
{
public:
  enum MediaType {
    kAudio,
    kVideo,
    kText,
    kApplication,
    kMessage,
    kUnknownMediaType
  };

  virtual MediaType GetMediaType() const = 0;
  virtual unsigned int GetPort() const = 0;
  virtual unsigned int GetPortCount() const = 0;
  virtual sdp::Protocol GetProtocol() const = 0;
  virtual SdpConnection GetConnection() const = 0;
  virtual Maybe<std::string> GetBandwidth(const std::string& type) const = 0;
  virtual std::vector<std::string> GetFormats() const = 0;

  virtual const SdpAttributeList &GetAttributeList() const = 0;
  virtual SdpAttributeList &GetAttributeList() = 0;
};

class SdpConnection
{
public:
  SdpConnection(sdp::NetType netType, sdp::AddrType addrType,
                std::string addr, int16_t ttl = -1,
                uint32_t count = 1)
      : mNetType(netType), mAddrType(addrType),
      mAddr(addr), mTtl(ttl), mCount(count) {}
  ~SdpConnection() {}

  sdp::NetType GetNetType() const { return mNetType; }
  sdp::AddrType GetAddrType() const { return mAddrType; }
  const std::string& GetAddress() const { return mAddr; }
  int16_t GetTtl() const { return mTtl; }
  uint32_t GetCount() const { return mCount; }

private:
  sdp::NetType mNetType;
  sdp::AddrType mAddrType;
  std::string mAddr;
  int16_t mTtl; // 0-255; -1 for unset
  uint32_t mCount;
};

}

#endif
