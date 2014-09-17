/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPENUM_H_
#define _SDPENUM_H_

namespace mozilla {
namespace sdp {

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
  kUdpTlsUdptl,        // UDP/TLS/UDPTL [RFC7345]
  kUnknown
};

enum AttributeType {
  kBundleOnly,
  kCandidate,
  kConnection,
  kDtlsFingerprint,
  kExtmap,
  kFingerprint,
  kFmtp,
  kGroup,
  kIceLite,
  kIceMismatch,
  kIceOptions,
  kIcePwd,
  kIceUfrag,
  kIdentity,
  kImageattr,
  kInactive,
  kLabel,
  kMaxprate,
  kMaxptime,
  kMid,
  kMsid,
  kPtime,
  kRecvonly,
  kRemoteCandidates,
  kRtcp,
  kRtcpFb,
  kRtcpMux,
  kRtcpRsize,
  kRtpmap,
  kSctpmap,
  kSendonly,
  kSendrecv,
  kSetup,
  kSsrc,
  kSsrcGroup,
  kOther
};

}
}

#endif
