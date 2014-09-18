/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdp.h"

#include "mozilla/Assertions.h"

namespace mozilla {

SipccSdp::~SipccSdp() {
  for (auto i = mMediaSections.begin(); i != mMediaSections.end(); ++i) {
    delete *i;
  }
}

const SdpOrigin& SipccSdp::GetOrigin() const {
  return *mOrigin;
}

const std::string&
SipccSdp::GetBandwidth(const std::string& type) const {
  static std::string emptyString("");
  auto found = mBandwidths.find(type);
  if (found == mBandwidths.end()) {
    return emptyString;
  }
  return found->second;
}

const SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level) const
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return *mMediaSections[level];
}

SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level)
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return *mMediaSections[level];
}

void
SipccSdp::Load(sdp_t* sdp) {
  // Believe it or not, SDP_SESSION_LEVEL is 0xFFFF
  mAttributeList.Load(sdp, SDP_SESSION_LEVEL);

  // TODO load other session-level stuff

  for (int i = 0; i < sdp_get_num_media_lines(sdp); ++i) {
    // note that we pass a "level" here that is one higher
    // sipcc counts media sections from 1, using 0 as the "session"
    SipccSdpMediaSection* section = new SipccSdpMediaSection(&mAttributeList);
    section->Load(sdp, i + 1);
    mMediaSections.push_back(section);
  }
}


} // namespace mozilla
