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

sdp::Protocol
SipccSdpMediaSection::GetProtocol() const {
  return mProtocol;
}

SdpConnection
SipccSdpMediaSection::GetConnection() const {
  return SdpConnection(sdp::kInternet, sdp::kIPv4, "0.0.0.0");
}

Maybe<std::string>
SipccSdpMediaSection::GetBandwidth(const std::string& type) const {
  return Maybe<std::string>();
}

std::vector<std::string>
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
SipccSdpMediaSection::Load()
{
  switch (sdp_get_media_type(mSdp, mLevel)) {
  case SDP_MEDIA_AUDIO:
    mMediaType = sdp::kAudio;
    break;
  case SDP_MEDIA_VIDEO:
    mMediaType = sdp::kVideo;
    break;
  case SDP_MEDIA_APPLICATION:
    mMediaType = sdp::kApplication;
    break;
  case SDP_MEDIA_TEXT:
    mMediaType = sdp::kText;
    break;
  case SDP_MEDIA_DATA:
    mMediaType = sdp::kMessage;
    break;
  default:
    // TODO: log this
    mMediaType = sdp::kUnknownMediaType;
    break;
  }

  mPort = sdp_get_media_portnum(mSdp, mLevel);
  mPortCount = sdp_get_media_portcount(mSdp, mLevel);

  switch (sdp_get_media_transport(mSdp, mLevel)) {
    // TODO add right protocols to sipcc
  default:
    mProtocol = sdp::kUnknownProtocol;
  }
}

}

