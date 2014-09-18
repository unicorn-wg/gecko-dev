/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include "signaling/src/sdp/SdpAttributeList.h"
#include "signaling/src/sdp/SdpEnum.h"
#include "mozilla/UniquePtr.h"
#include "mozilla/Maybe.h"
#include <vector>

namespace mozilla {

class SdpOrigin;
class SdpConnection;
class SdpBandwidth;
class SdpEncryptionKey;

class SdpMediaSection;

class Sdp
{
public:
  Sdp();

  virtual SdpOrigin GetOrigin() const = 0;
  virtual std::string GetSessionName() const = 0;
  // Note: connection information is always retrieved from media sections
  virtual Maybe<std::string> GetBandwidth(const std::string& type) const = 0;

  virtual const SdpAttributeList &GetAttributeList() const = 0;
  virtual SdpAttributeList &GetAttributeList() = 0;

  virtual uint16_t GetMediaSectionCount() const = 0;
  virtual const SdpMediaSection &GetMediaSection(uint16_t level) const = 0;
  virtual SdpMediaSection &GetMediaSection(uint16_t level) = 0;

protected:
  virtual ~Sdp() {};
};

class SdpOrigin
{
public:
  SdpOrigin() :
    mSessionId(0),
    mSessionVersion(0),
    mNetType(sdp::kInternet),
    mAddrType(sdp::kIPv4)
  {}

  const std::string& GetUsername() const {
    return mUsername;
  }

  void SetUsername(const std::string& username) {
    mUsername = username;
  }

  uint64_t GetSessionId() const {
    return mSessionId;
  }

  void SetSessionId(uint64_t sessionId) {
    mSessionId = sessionId;
  }

  uint64_t GetSessionVersion() const {
    return mSessionVersion;
  }

  void SetSessionVersion(uint64_t sessionVersion) {
    mSessionVersion = sessionVersion;
  }

  const sdp::NetType& GetNetType() const {
    return mNetType;
  }

  void SetNetType(const sdp::NetType& netType) {
    mNetType = netType;
  }

  const sdp::AddrType& GetAddrType() const {
    return mAddrType;
  }

  void SetAddrType(const sdp::AddrType& addrType) {
    mAddrType = addrType;
  }

  const std::string& GetAddress() const {
    return mAddress;
  }

  void SetAddress(const std::string& address) {
    mAddress = address;
  }

private:
  std::string mUsername;
  uint64_t mSessionId;
  uint64_t mSessionVersion;
  sdp::NetType mNetType;
  sdp::AddrType mAddrType;
  std::string mAddress;
};

} // namespace mozilla

#endif
