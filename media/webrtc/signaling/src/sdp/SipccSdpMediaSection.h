/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPMEDIASECTION_H_
#define _SIPCCSDPMEDIASECTION_H_

#include "mozilla/Attributes.h"
#include "signaling/src/sdp/SdpMediaSection.h"
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdp;

class SipccSdpMediaSection MOZ_FINAL : public SdpMediaSection
{
  friend class SipccSdp;
public:
  virtual sdp::MediaType
  GetMediaType() const MOZ_OVERRIDE
  {
    return mMediaType;
  }

private:
  SipccSdpMediaSection(sdp_t* sdp, uint16_t level)
    : mSdp(sdp), mLevel(level) {}
  ~SipccSdpMediaSection() {}

  void Load();

  // this is just on loan, so we don't delete on destruct;
  // this doesn't live beyond the SipccSdp instance that owns this
  sdp_t* mSdp;
  uint16_t mLevel;

  // the following values are cached on first get
  sdp::MediaType mMediaType;
  uint16_t mPort;
  uint16_t mPortCount;
  sdp::Protocol mProtocol;
  std::vector<std::string> mFormats;

  SdpConnection mConnection;
};

}

#endif
