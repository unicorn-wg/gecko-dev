/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpMediaSection.h"

#include "signaling/src/sdp/SdpErrorHolder.h"

namespace mozilla {

unsigned int
SipccSdpMediaSection::GetPort() const {
  return mPort;
}

unsigned int
SipccSdpMediaSection::GetPortCount() const {
  return mPortCount;
}

SdpMediaSection::Protocol
SipccSdpMediaSection::GetProtocol() const {
  return mProtocol;
}

const SdpConnection&
SipccSdpMediaSection::GetConnection() const {
  return *mConnection;
}

const std::string&
SipccSdpMediaSection::GetBandwidth(const std::string& type) const {
  static std::string emptyString("");
  auto found = mBandwidths.find(type);
  if (found == mBandwidths.end()) {
    return emptyString;
  }
  return found->second;
}

const std::vector<std::string>&
SipccSdpMediaSection::GetFormats() const {
  return mFormats;
}

const SdpAttributeList&
SipccSdpMediaSection::GetAttributeList() const {
  return mAttributeList;
}

SdpAttributeList&
SipccSdpMediaSection::GetAttributeList() {
  return mAttributeList;
}

bool
SipccSdpMediaSection::Load(sdp_t* sdp, uint16_t level,
                           SdpErrorHolder& errorHolder)
{
  switch (sdp_get_media_type(sdp, level)) {
  case SDP_MEDIA_AUDIO: mMediaType = kAudio; break;
  case SDP_MEDIA_VIDEO: mMediaType = kVideo; break;
  case SDP_MEDIA_APPLICATION: mMediaType = kApplication; break;
  case SDP_MEDIA_TEXT: mMediaType = kText; break;

  default:
    errorHolder.AddParseError(0, "Unsupported media section type");
    return false;
  }

  mPort = sdp_get_media_portnum(sdp, level);
  mPortCount = sdp_get_media_portcount(sdp, level);

  switch (sdp_get_media_transport(sdp, level)) {
  case SDP_TRANSPORT_RTPAVP: mProtocol = kRtpAvp; break;
  case SDP_TRANSPORT_RTPSAVP: mProtocol = kRtpSavp; break;
  case SDP_TRANSPORT_RTPAVPF: mProtocol = kRtpAvpf; break;
  case SDP_TRANSPORT_RTPSAVPF: mProtocol = kRtpSavpf; break;
  case SDP_TRANSPORT_UDPTLSRTPSAVP: mProtocol = kUdpTlsRtpSavp; break;
  case SDP_TRANSPORT_UDPTLSRTPSAVPF: mProtocol = kUdpTlsRtpSavpf; break;
  case SDP_TRANSPORT_TCPTLSRTPSAVP: mProtocol = kTcpTlsRtpSavp; break;
  case SDP_TRANSPORT_TCPTLSRTPSAVPF: mProtocol = kTcpTlsRtpSavpf; break;

  default:
    errorHolder.AddParseError(0, "Unsupported media transport type");
    return false;
  }

  if (!mAttributeList.Load(sdp, level, errorHolder)) {
    return false;
  }

  return LoadConnection(sdp, level, errorHolder);
}

bool
SipccSdpMediaSection::LoadConnection(sdp_t* sdp, uint16_t level,
                                     SdpErrorHolder& errorHolder) {
  if (!sdp_connection_valid(sdp, level)) {
    level = SDP_SESSION_LEVEL;
    if (!sdp_connection_valid(sdp, level)) {
      errorHolder.AddParseError(0, "Missing c= line");
      return false;
    }
  }

  sdp_nettype_e type = sdp_get_conn_nettype(sdp, level);
  if (type != SDP_NT_INTERNET) {
    errorHolder.AddParseError(0, "Unsupported network type");
    return false;
  }

  sdp::AddrType addrType;
  switch (sdp_get_conn_addrtype(sdp, level)) {
    case SDP_AT_IP4:
      addrType = sdp::kIPv4;
      break;
    case SDP_AT_IP6:
      addrType = sdp::kIPv6;
      break;
    default:
      errorHolder.AddParseError(0, "Unsupported address type");
      return false;
  }

  std::string address = sdp_get_conn_address(sdp, level);
  int16_t ttl = static_cast<uint16_t>(sdp_get_mcast_ttl(sdp, level));
  uint32_t numAddr = static_cast<uint32_t>(sdp_get_mcast_num_of_addresses(sdp, level));
  mConnection = MakeUnique<SdpConnection>(addrType, address, ttl, numAddr);
  return true;
}

}
