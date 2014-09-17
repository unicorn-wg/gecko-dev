/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpMediaSection.h"

namespace mozilla {

sdp::MediaType
SipccSdpMediaSection::GetMediaType() const
{
  if (mMediaType == kUnknown) {
    switch (sdp_get_media_type(mSdp, mLevel)) {
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
      mMediaType = kUnknown;
    }
  }
  return mMediaType;
}


}

#endif
