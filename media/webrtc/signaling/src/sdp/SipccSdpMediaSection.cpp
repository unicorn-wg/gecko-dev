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

Protocol
SipccSdpMediaSection::GetProtocol() const {
  return mProtocol;
}

const SdpConnection&
SipccSdpMediaSection::GetConnection() const {
  return SdpConnection(sdp::kInternet, sdp::kIPv4, "0.0.0.0");
}

const Maybe<std::string>&
SipccSdpMediaSection::GetBandwidth(const std::string& type) const {
  return Maybe<std::string>();
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
  // TODO
}

}
