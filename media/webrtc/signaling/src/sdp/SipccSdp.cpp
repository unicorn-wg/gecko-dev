/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdp.h"

namespace mozilla {

SdpOrigin
SipccSdp::GetOrigin() const {
  return SdpOrigin();
}

std::string
SipccSdp::GetSessionName() const {
  return "TODO";
}

const Maybe<std::string>&
SipccSdp::GetBandwidth(const std::string& type) const {
  if (mBandwidths.count(type) > 0) {
    return Nothing();
  }
  return Some(mBandwidths[type]);
}

const SdpMediaSection &
SipccSdp::GetMediaSection(unsigned int level) const
{
  return mMediaSections[level];
}
SdpMediaSection &
SipccSdp::GetMediaSection(unsigned int level)
{
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
