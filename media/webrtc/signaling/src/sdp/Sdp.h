/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include <vector>
#include "mozilla/UniquePtr.h"
#include "mozilla/Maybe.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SdpAttributeList.h"

namespace mozilla {

class SdpOrigin;
class SdpBandwidth;
class SdpEncryptionKey;

class SdpMediaSection;

class Sdp
{
public:
  Sdp() {};

  virtual const SdpOrigin& GetOrigin() const = 0;
  virtual const std::string& GetSessionName() const = 0;
  // Note: connection information is always retrieved from media sections
  virtual const std::string& GetBandwidth(const std::string& type) const = 0;

  virtual const SdpAttributeList &GetAttributeList() const = 0;
  virtual SdpAttributeList &GetAttributeList() = 0;

  virtual uint16_t GetMediaSectionCount() const = 0;
  virtual const SdpMediaSection &GetMediaSection(uint16_t level) const = 0;
  virtual SdpMediaSection &GetMediaSection(uint16_t level) = 0;

  virtual ~Sdp() {};
};

class SdpOrigin
{
public:
  SdpOrigin() :
    mSessionId(0),
    mSessionVersion(0),
    mAddrType(SdpConnection::kIPv4)
  {}

  const std::string& GetUsername() const {
    return mUsername;
  }

  uint64_t GetSessionId() const {
    return mSessionId;
  }

  uint64_t GetSessionVersion() const {
    return mSessionVersion;
  }

  const SdpConnection::AddrType GetAddrType() const {
    return mAddrType;
  }

  const std::string& GetAddress() const {
    return mAddress;
  }

private:
  std::string mUsername;
  uint64_t mSessionId;
  uint64_t mSessionVersion;
  SdpConnection::AddrType mAddrType;
  std::string mAddress;
};

} // namespace mozilla

#endif
