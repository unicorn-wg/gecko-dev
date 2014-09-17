/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include "signaling/src/sdp/SdpAttribute.h"
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

  virtual SdpOrigin GetOriginator() const = 0;
  virtual std::string GetSessionName() const = 0;
  // Note: connection information is always retrieved from media sections
  virtual std::string GetBandwidth(std::string type) const = 0;

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
  virtual std::string GetUsername() const = 0;
  virtual uint64_t GetSessionId() const = 0;
  virtual uint64_t GetSessionVersion() const = 0;
  virtual sdp::NetType GetNetType() const = 0;
  virtual sdp::AddrType GetAddrType() const = 0;
  virtual std::string GetAddress() const = 0;
};

}

#endif
