/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPMEDIASECTION_H_
#define _SDPMEDIASECTION_H_


#include "mozilla/Maybe.h"
#include "signaling/src/sdp/SdpEnum.h"
#include "signaling/src/sdp/SdpAttributeList.h"
#include <string>
#include <vector>

#include "signaling/src/sdp/SdpEnum.h"

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
    kTcpTlsRtpSavp,      // TCP/TLS/RTP/SAVP [JSEP-TBD]
    kDccpTlsRtpSavp,     // DCCP/TLS/RTP/SAVP [RFC5764]
    kUdpTlsRtpSavpf,     // UDP/TLS/RTP/SAVPF [RFC5764]
    kTcpTlsRtpSavpf,     // TCP/TLS/RTP/SAVPF [JSEP-TBD]
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

  SdpMediaSection(size_t level) : mLevel(level) {}

  virtual MediaType GetMediaType() const = 0;
  virtual unsigned int GetPort() const = 0;
  virtual unsigned int GetPortCount() const = 0;
  virtual Protocol GetProtocol() const = 0;
  virtual const SdpConnection& GetConnection() const = 0;
  virtual uint32_t GetBandwidth(const std::string& type) const = 0;
  virtual const std::vector<std::string>& GetFormats() const = 0;

  virtual const SdpAttributeList &GetAttributeList() const = 0;
  virtual SdpAttributeList &GetAttributeList() = 0;

  virtual SdpDirectionAttribute GetDirectionAttribute() const = 0;

  virtual void Serialize(std::ostream&) const = 0;

  virtual void AddCodec(uint8_t pt, const std::string& name,
                        uint32_t clockrate, uint16_t channels = 0,
                        const std::string& fmtp = "") = 0;

  virtual void AddDataChannel(uint16_t pt = 5000U,
                              const std::string& sctpmap = "webrtc-datachannel") = 0;

  size_t GetLevel() const { return mLevel; }

private:
  size_t mLevel;
};

inline std::ostream& operator <<(std::ostream& os, const SdpMediaSection &ms)
{
  ms.Serialize(os);
  return os;
}

inline std::ostream& operator <<(std::ostream& os, SdpMediaSection::MediaType t)
{
  switch (t) {
    case SdpMediaSection::kAudio: os << "audio"; break;
    case SdpMediaSection::kVideo: os << "video"; break;
    case SdpMediaSection::kText: os << "text"; break;
    case SdpMediaSection::kApplication: os << "application"; break;
    case SdpMediaSection::kMessage: os << "message"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

inline std::ostream& operator <<(std::ostream& os, SdpMediaSection::Protocol p)
{
  switch (p) {
    case SdpMediaSection::kRtpAvp: os << "RTP/AVP"; break;
    case SdpMediaSection::kUdp: os << "udp"; break;
    case SdpMediaSection::kVat: os << "vat"; break;
    case SdpMediaSection::kRtp: os << "rtp"; break;
    case SdpMediaSection::kUdptl: os << "udptl"; break;
    case SdpMediaSection::kTcp: os << "TCP"; break;
    case SdpMediaSection::kRtpAvpf: os << "RTP/AVPF"; break;
    case SdpMediaSection::kTcpRtpAvp: os << "TCP/RTP/AVP"; break;
    case SdpMediaSection::kRtpSavp: os << "RTP/SAVP"; break;
    case SdpMediaSection::kTcpBfcp: os << "TCP/BFCP"; break;
    case SdpMediaSection::kTcpTlsBfcp: os << "TCP/TLS/BFCP"; break;
    case SdpMediaSection::kTcpTls: os << "TCP/TLS"; break;
    case SdpMediaSection::kFluteUdp: os << "FLUTE/UDP"; break;
    case SdpMediaSection::kTcpMsrp: os << "TCP/MSRP"; break;
    case SdpMediaSection::kTcpTlsMsrp: os << "TCP/TLS/MSRP"; break;
    case SdpMediaSection::kDccp: os << "DCCP"; break;
    case SdpMediaSection::kDccpRtpAvp: os << "DCCP/RTP/AVP"; break;
    case SdpMediaSection::kDccpRtpSavp: os << "DCCP/RTP/SAVP"; break;
    case SdpMediaSection::kDccpRtpAvpf: os << "DCCP/RTP/AVPF"; break;
    case SdpMediaSection::kDccpRtpSavpf: os << "DCCP/RTP/SAVPF"; break;
    case SdpMediaSection::kRtpSavpf: os << "RTP/SAVPF"; break;
    case SdpMediaSection::kUdpTlsRtpSavp: os << "UDP/TLS/RTP/SAVP"; break;
    case SdpMediaSection::kTcpTlsRtpSavp: os << "TCP/TLS/RTP/SAVP"; break;
    case SdpMediaSection::kDccpTlsRtpSavp: os << "DCCP/TLS/RTP/SAVP"; break;
    case SdpMediaSection::kUdpTlsRtpSavpf: os << "UDP/TLS/RTP/SAVPF"; break;
    case SdpMediaSection::kTcpTlsRtpSavpf: os << "TCP/TLS/RTP/SAVPF"; break;
    case SdpMediaSection::kDccpTlsRtpSavpf: os << "DCCP/TLS/RTP/SAVPF"; break;
    case SdpMediaSection::kUdpMbmsFecRtpAvp:
      os << "UDP/MBMS-FEC/RTP/AVP"; break;
    case SdpMediaSection::kUdpMbmsFecRtpSavp:
      os << "UDP/MBMS-FEC/RTP/SAVP"; break;
    case SdpMediaSection::kUdpMbmsRepair: os << "UDP/MBMS-REPAIR"; break;
    case SdpMediaSection::kFecUdp: os << "FEC/UDP"; break;
    case SdpMediaSection::kUdpFec: os << "UDP/FEC"; break;
    case SdpMediaSection::kTcpMrcpv2: os << "TCP/MRCPv2"; break;
    case SdpMediaSection::kTcpTlsMrcpv2: os << "TCP/TLS/MRCPv2"; break;
    case SdpMediaSection::kPstn: os << "PSTN"; break;
    case SdpMediaSection::kUdpTlsUdptl: os << "UDP/TLS/UDPTL"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

class SdpConnection
{
public:
  SdpConnection(sdp::AddrType addrType, std::string addr,
                uint8_t ttl = 0, uint32_t count = 0)
      : mAddrType(addrType), mAddr(addr),
        mTtl(ttl), mCount(count) {}
  ~SdpConnection() {}


  sdp::AddrType GetAddrType() const { return mAddrType; }
  const std::string& GetAddress() const { return mAddr; }
  uint8_t GetTtl() const { return mTtl; }
  uint32_t GetCount() const { return mCount; }

  void Serialize(std::ostream& os) const {
    sdp::NetType netType = sdp::kInternet;

    os << "c="
       << netType << " "
       << mAddrType << " "
       << mAddr;

    if (mTtl) {
      os << "/" << static_cast<uint32_t>(mTtl);
      if (mCount) {
        os << "/" << mCount;
      }
    }
    os << CRLF;
  }
private:
  sdp::AddrType mAddrType;
  const std::string mAddr;
  uint8_t mTtl; // 0-255; 0 when unset
  uint32_t mCount; // 0 when unset
};

inline std::ostream& operator <<(std::ostream& os, const SdpConnection &c)
{
  c.Serialize(os);
  return os;
}

}

#endif
