/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include "signaling/src/sdp/SdpAttribute.h"
#include "signaling/src/sdp/SdpEnum.h"
#include "mozilla/UniquePtr.h"
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

  unsigned int GetVersion() const;
  SdpOriginator GetOriginator() const;
  std::string GetSessionName() const;
  // Note: connection information is always retrieved from media sections
  std::string GetBandwidth(std::string type) const;

  const SdpAttributeList &GetAttributeList() const;
  SdpAttributeList &GetAttributeList();

  uint16_t GetMediaSectionCount() const;
  const SdpMediaSection &GetMediaSection(uint16_t level) const;
  SdpMediaSection &GetMediaSection(uint16_t level);

protected:
  virtual ~Sdp() {};
};

class SdpOrigin
{
public:
  std::string GetUsername() const;
  uint64_t GetSessionId() const;
  uint64_t GetSessionVersion() const;
  sdp::NetType GetNetType() const;
  sdp::AddrType GetAddrType() const;
  std::string GetAddress() const;
};

}

#endif
