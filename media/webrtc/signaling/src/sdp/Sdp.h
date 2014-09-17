/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include "signaling/src/sdp/SdpAttribute.h"

namespace mozilla {

class SdpOrigin;
class SdpConnection;
class SdpBandwidth;
class SdpTiming;
class SdpRepeatTimes;
class SdpEncryptionKey;

class SdpAttributeList;
class SdpAttribute;
class SdpMediaSectionList;
class SdpMediaSection;

class Sdp
{
public:
  Sdp();

  unsigned int GetVersion() const;
  const SdpOriginator &GetOriginator() const;
  const std::string &GetSessionName() const;
  const std::string &GetSessionInformation() const;
  const std::string &GetURI() const;
  const std::string &GetEmail() const;
  const std::string &GetPhone() const;
  const SdpConnection &GetConnection() const;
  const SdpBandwidth &GetBandwidth() const;
  const SdpRepeatTimes &GetRepeatTimes() const;
  const SdpRepeatTimes &GetEncryptionKey() const;

  enum NetType {
    kInternet
  };

  enum AddrType {
    kIPv4,
    kIPv6
  };

protected:
  virtual ~Sdp() {};
};

class SdpOrigin
{
public:
  const std::string &GetUsername() const;
  uint64_t GetSessionId() const;
  uint64_t GetSessionVersion() const;
  const Sdp::NetType &GetNetType() const;
  const Sdp::AddrType &GetAddrType() const;
  const std::string &GetAddress() const;
};

class SdpConnection
{
public:
  Sdp::NetType GetNetType() const;
  Sdp::AddrType GetAddrType() const;
  const std::string &GetAddress() const;
  int GetTtl() const;
  int GetCount() const;
};

class SdpBandwidth
{
public:
  const std::string &GetBwtype() const;
  int GetBandwidth() const;
};

class SdpTiming
{
public:
  uint64_t GetStartTime() const;
  uint64_t GetStopTime() const;
};

class SdpRepeatTimes
{
public:
  uint64_t GetRepeatInterval() const;
  uint64_t GetActiveDuration() const;
  uint64_t GetOffset() const;
};

class SdpEncryptionKey
{
public:
  const std::string &GetMethod() const;
  const std::string &GetKey() const;
};

}

#endif
