/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdp.h"

#include "mozilla/Assertions.h"

namespace mozilla {


const Maybe<std::string>&
SipccSdp::GetBandwidth(const std::string& type) const {
  return Maybe<std::string>();
}

const SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level) const
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return mMediaSections[level];
}

SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level)
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return mMediaSections[level];
}

void
SipccSdp::Load(sdp_t* sdp) {
  mAttributeList.Load(sdp, 0);

  // TODO load other session-level stuff

  for (int i = 0; i < sdp_get_num_media_lines(sdp); ++i) {
    // note that we pass a "level" here that is one higher
    // sipcc counts media sections from 1, using 0 as the "session"
    SipccSdpMediaSection section();
    section.Load(sdp, i + 1);
    mMediaSections.push_back(section);
  }
}


} // namespace mozilla
