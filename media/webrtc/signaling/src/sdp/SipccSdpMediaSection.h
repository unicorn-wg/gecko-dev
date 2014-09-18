/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPMEDIASECTION_H_
#define _SIPCCSDPMEDIASECTION_H_

#include "mozilla/Attributes.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SdpEnum.h"
#include "signaling/src/sdp/SipccSdpAttributeList.h"

extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdp;

class SipccSdpMediaSection MOZ_FINAL : public SdpMediaSection
{
  friend class SipccSdp;
public:
  ~SipccSdpMediaSection() {}

  virtual sdp::MediaType
  GetMediaType() const MOZ_OVERRIDE
  {
    return mMediaType;
  }

  virtual unsigned int GetPort() const MOZ_OVERRIDE;
  virtual unsigned int GetPortCount() const MOZ_OVERRIDE;
  virtual sdp::Protocol GetProtocol() const MOZ_OVERRIDE;
  virtual SdpConnection GetConnection() const MOZ_OVERRIDE;
  virtual Maybe<std::string> GetBandwidth(const std::string& type) const MOZ_OVERRIDE;
  virtual std::vector<std::string> GetFormats() const MOZ_OVERRIDE;

  virtual const SdpAttributeList &GetAttributeList() const MOZ_OVERRIDE;
  virtual SdpAttributeList &GetAttributeList() MOZ_OVERRIDE;

private:
  SipccSdpMediaSection(sdp_t* sdp, uint16_t level)
    : mSdp(sdp),
      mLevel(level),
      mAttributes(sdp, level),
      mConnection(sdp::kInternet, sdp::kIPv4, "0.0.0.0") {}

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
  SipccSdpAttributeList mAttributes;

  SdpConnection mConnection;
};

}

#endif
