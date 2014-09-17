/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_H_
#define _SDP_H_

#include "signaling/src/sdp/SdpAttribute.h"
#include "mozilla/UniquePtr.h"
#include <vector>

namespace mozilla {

class SdpOrigin;
class SdpConnection;
class SdpBandwidth;
class SdpEncryptionKey;

class SdpAttributeList;
class SdpAttribute;
class SdpMediaSection;

class Sdp
{
public:
  Sdp();

  unsigned int GetVersion() const;
  SdpOriginator GetOriginator() const;
  std::string GetSessionName() const;
  SdpConnection GetConnection() const; // optional
  SdpBandwidth GetBandwidth() const; // optional, may repeat

  const SdpAttributeList &GetAttributeList() const;
  SdpAttributeList &GetAttributeList();

  const SdpMediaSection &GetMediaSection(unsigned int level) const;
  SdpMediaSection &GetMediaSection(unsigned int level);

  enum NetType {
    kInternet
  };

  enum AddrType {
    kIPv4,
    kIPv6
  };

  enum MediaType {
    kAudio,
    kVideo,
    kText,
    kApplication,
    kMessage,
    kUnknown
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
    kUdpTlsUdptl         // UDP/TLS/UDPTL [RFC7345]
  };

protected:
  virtual ~Sdp() {};
};

class SdpOrigin
{
public:
  std::string GetUsername() const;
  uint64_t GetSessionId() const;
  uint64_t GetSessionVersion() const;
  Sdp::NetType GetNetType() const;
  Sdp::AddrType GetAddrType() const;
  std::string GetAddress() const;
};

class SdpConnection
{
public:
  Sdp::NetType GetNetType() const;
  Sdp::AddrType GetAddrType() const;
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

class SdpMediaSection
{
public:
  Sdp::MediaType GetMediaType() const;
  unsigned int GetPort() const;
  unsigned int GetPortCount() const;
  Sdp::Protocol GetProtocol const;
  SdpConnection GetConnection() const; // optional
  SdpBandwidth GetBandwidth() const; // optional, may repeat
  std::vector<std::string> GetFormats() const;

  const SdpAttributeList &GetAttributeList() const;
  SdpAttributeList &GetAttributeList();
};

}

#endif
