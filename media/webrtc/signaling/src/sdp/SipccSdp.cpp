/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdp.h"

namespace mozilla {

const SdpMediaSection &
SipccSdp::GetMediaSection(uint16_t level) const
{
  return mMediaSections[level];
}
SdpMediaSection &
SipccSdp::GetMediaSection(uint16_t level)
{
  return mMediaSections[level];
}

void
SipccSdp::Load() {
  if (!mMediaSections.empty()) {
    return;
  }

  for (int i = 0; i < sdp_get_num_media_lines(mSdp); ++i) {
    // note that we pass a "level" here that is one higher
    // sipcc counts media sections from 1, using 0 as the "session"
    SipccSdpMediaSection section(mSdp, i + 1);
    section.Load();
    mMediaSections.push_back(section);
  }
}


} // namespace mozilla
