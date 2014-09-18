/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpMediaSection.h"

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
  return mAttributes;
}

SdpAttributeList&
SipccSdpMediaSection::GetAttributeList() {
  return mAttributes;
}

void
SipccSdpMediaSection::Load(sdp_t* sdp, uint16_t level)
{
  switch (sdp_get_media_type(sdp, level)) {
  case SDP_MEDIA_AUDIO:
    mMediaType = kAudio;
    break;
  case SDP_MEDIA_VIDEO:
    mMediaType = kVideo;
    break;
  case SDP_MEDIA_APPLICATION:
    mMediaType = kApplication;
    break;
  case SDP_MEDIA_TEXT:
    mMediaType = kText;
    break;
  case SDP_MEDIA_DATA:
    mMediaType = kMessage;
    break;
  default:
    // TODO: log this
    mMediaType = kUnknownMediaType;
    break;
  }

  mPort = sdp_get_media_portnum(sdp, level);
  mPortCount = sdp_get_media_portcount(sdp, level);

  switch (sdp_get_media_transport(sdp, level)) {
    // TODO add right protocols to sipcc
  default:
    mProtocol = kUnknownProtocol;
  }

  mAttributeList.Load(sdp, level);

  LoadConnection(sdp, level);
}

void
SipccSdpMediaSection::LoadConnection(sdp_t* sdp, uint16_t level) {
  sdp_nettype_e type = sdp_get_conn_nettype(sdp, level);
  if (type != SDP_NT_INTERNET) {
    level = 0; // hop up to the session level and see if that works out better
    type = sdp_get_conn_nettype(sdp, level);
  }

  SdpConnection::AddrType addrType;
  switch (sdp_get_conn_addrtype(sdp, level)) {
    case SDP_AT_IP4:
      addrType = SdpConnection::kIPv4;
      break;
    case SDP_AT_IP6:
      addrType = SdpConnection::kIPv6;
      break;
    default:
      // TODO: log this
      addrType = SdpConnection::kAddrTypeUnknown;
      break;
  }

  std::string address = sdp_get_conn_address(sdp, level);
  int16_t ttl = -1;
  uint32_t numAddr = 1;
  if (sdp_is_mcast_addr(sdp, level)) {
    // TODO: log a warning about this
    ttl = static_cast<uint16_t>(sdp_get_mcast_ttl(sdp, level));
    numAddr = static_cast<uint32_t>(sdp_get_mcast_num_of_addresses(sdp, level));
  }

  mConnection = MakeUnique<SdpConnection>(addrType, address, ttl, numAddr);
}

}
