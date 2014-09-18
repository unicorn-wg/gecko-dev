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

  enum Protocol {
    kRtpAvp,             // RTP/AVP [RFC4566]
    kUdp,                // udp [RFC4566]
    kVat,                // vat [historic]
    kRtp,                // rtp [historic]
    kUdptl,              // udptl [ITU-T]
    kTcp,                // TCP [RFC4145]
    kRtpAvpf,            // RTP/AVPF [RFC4585]
    kTcpRtpAvp,          // TCP/RTP/AVP [RFC4571]
    kRtpSavp,            // RTP/SAVP [RFC3711]
    kTcpBfcp,            // TCP/BFCP [RFC4583]
    kTcpTlsBfcp,         // TCP/TLS/BFCP [RFC4583]
    kTcpTls,             // TCP/TLS [RFC4572]
    kFluteUdp,           // FLUTE/UDP [RFC-mehta-rmt-flute-sdp-05]
    kTcpMsrp,            // TCP/MSRP [RFC4975]
    kTcpTlsMsrp,         // TCP/TLS/MSRP [RFC4975]
    kDccp,               // DCCP [RFC5762]
    kDccpRtpAvp,         // DCCP/RTP/AVP [RFC5762]
    kDccpRtpSavp,        // DCCP/RTP/SAVP [RFC5762]
    kDccpRtpAvpf,        // DCCP/RTP/AVPF [RFC5762]
    kDccpRtpSavpf,       // DCCP/RTP/SAVPF [RFC5762]
    kRtpSavpf,           // RTP/SAVPF [RFC5124]
    kUdpTlsRtpSavp,      // UDP/TLS/RTP/SAVP [RFC5764]
    kDccpTlsRtpSavp,     // DCCP/TLS/RTP/SAVP [RFC5764]
    kUdpTlsRtpSavpf,     // UDP/TLS/RTP/SAVPF [RFC5764]
    kDccpTlsRtpSavpf,    // DCCP/TLS/RTP/SAVPF [RFC5764]
    kUdpMbmsFecRtpAvp,   // UDP/MBMS-FEC/RTP/AVP [RFC6064]
    kUdpMbmsFecRtpSavp,  // UDP/MBMS-FEC/RTP/SAVP [RFC6064]
    kUdpMbmsRepair,      // UDP/MBMS-REPAIR [RFC6064]
    kFecUdp,             // FEC/UDP [RFC6364]
    kUdpFec,             // UDP/FEC [RFC6364]
    kTcpMrcpv2,          // TCP/MRCPv2 [RFC6787]
    kTcpTlsMrcpv2,       // TCP/TLS/MRCPv2 [RFC6787]
    kPstn,               // PSTN [RFC7195]
    kUdpTlsUdptl,        // UDP/TLS/UDPTL [RFC7345]
    kUnknownProtocol
  };

  virtual MediaType GetMediaType() const = 0;
  virtual unsigned int GetPort() const = 0;
  virtual unsigned int GetPortCount() const = 0;
  virtual Protocol GetProtocol() const = 0;
  virtual const SdpConnection& GetConnection() const = 0;
  virtual const std::string& GetBandwidth(const std::string& type) const = 0;
  virtual const std::vector<std::string>& GetFormats() const = 0;

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
  const std::string mAddr;
  int16_t mTtl; // 0-255; -1 for unset
  uint32_t mCount;
};

}

#endif
