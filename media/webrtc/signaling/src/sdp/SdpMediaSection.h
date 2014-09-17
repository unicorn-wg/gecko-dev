/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPMEDIASECTION_H_
#define _SDPMEDIASECTION_H_

namespace mozilla {

class SdpAttributeList;

class SdpConnection;
class SdpBandwidth;

class SdpMediaSection
{
public:
  sdp::MediaType GetMediaType() const;
  unsigned int GetPort() const;
  unsigned int GetPortCount() const;
  sdp::Protocol GetProtocol() const;
  SdpConnection GetConnection() const;
  SdpBandwidth GetBandwidth() const; // optional, may repeat
  std::vector<std::string> GetFormats() const;

  const SdpAttributeList &GetAttributeList() const;
  SdpAttributeList &GetAttributeList();
};

class SdpConnection
{
public:
  sdp::NetType GetNetType() const;
  sdp::AddrType GetAddrType() const;
  std::string GetAddress() const;
  int GetTtl() const;
  int GetCount() const;
};

class SdpBandwidth
{
public:
  std::string GetBwtype() const;
  int GetBandwidth() const;
};

}

#endif
