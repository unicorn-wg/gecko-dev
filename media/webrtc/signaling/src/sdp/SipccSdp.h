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

  const SdpAttributeList& GetAttributeList() const {
    return mAttributeList;
  }
  SdpAttributeList& GetAttributeList() {
    return mAttributeList;
  }

  const SdpMediaSection &GetMediaSection(uint16_t level) const {
    if (level >= mMediaSections.length()) {
      MOZ_CRASH();
    }
    return mMediaSections[level];
  }
  SdpMediaSection &GetMediaSection(uint16_t level) {
    if (level >= mMediaSections.length()) {
      MOZ_CRASH();
    }
    return mMediaSections[level];
  }

private:
  SipccSdp(sdp_t* sdp) : mSdp(sdp) {}

  void Load();

  sdp_t *mSdp;
  std::vector<SipccSdpMediaSection> mMediaSections;
  SipccSdpAttributeList mAttributeList;
};

} // namespace mozilla

#endif // _sdp_h_
