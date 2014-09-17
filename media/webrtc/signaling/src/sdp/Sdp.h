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

  virtual SdpOriginator GetOriginator() const = 0;
  virtual std::string GetSessionName() const = 0;
  virtual Maybe<SdpBandwidth> GetBandwidth(const std::string& type) const = 0;

  const SdpAttributeList &GetAttributeList() const;
  SdpAttributeList &GetAttributeList();

  const SdpMediaSection &GetMediaSection(unsigned int level) const;
  SdpMediaSection &GetMediaSection(unsigned int level);

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
