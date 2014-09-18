/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _sdp_h_
#define _sdp_h_

#include <vector>
#include "mozilla/Attributes.h"
#include "mozilla/UniquePtr.h"

#include "signaling/src/sdp/Sdp.h"
#include "signaling/src/sdp/SipccSdpMediaSection.h"
#include "signaling/src/sdp/SipccSdpAttributeList.h"
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdpParser;

class SipccSdp MOZ_FINAL : public Sdp
{
  friend class SipccSdpParser;
public:
  ~SipccSdp() {
    sdp_free_description(mSdp);
  }

  virtual SdpOrigin GetOrigin() const MOZ_OVERRIDE;
  virtual std::string GetSessionName() const MOZ_OVERRIDE;
  // Note: connection information is always retrieved from media sections
  virtual Maybe<std::string> GetBandwidth(
      const std::string& type) const MOZ_OVERRIDE;

  virtual uint16_t GetMediaSectionCount() const MOZ_OVERRIDE {
    return static_cast<uint16_t>(mMediaSections.size());
  }

  virtual const SdpAttributeList &GetAttributeList() const MOZ_OVERRIDE {
    return mAttributeList;
  }

  virtual SdpAttributeList &GetAttributeList() MOZ_OVERRIDE {
    return mAttributeList;
  }

  virtual const SdpMediaSection &GetMediaSection(
      uint16_t level) const MOZ_OVERRIDE;

  virtual SdpMediaSection &GetMediaSection(uint16_t level) MOZ_OVERRIDE;

private:
  SipccSdp(sdp_t* sdp) : mSdp(sdp) {}

  void Load();

  sdp_t *mSdp;
  std::vector<SipccSdpMediaSection> mMediaSections;
  SipccSdpAttributeList mAttributeList;
};

} // namespace mozilla

#endif // _sdp_h_
