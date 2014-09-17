/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPMEDIASECTION_H_
#define _SIPCCSDPMEDIASECTION_H_

#include "signaling/src/sdp/sipcc/sdp.h"

namespace mozilla {

class SipccSdp;

class SipccSdpMediaSection : public SdpMediaSection
{
  friend class SipccSdp;
private:
  SipccSdpMediaSection(sdp_t* sdp, uint16_t level)
    : mSdp(sdp), mLevel(level) {}
  ~SipccSdpMediaSection() {}

  // this is just on loan, so we don't delete on destruct;
  // this doesn't live beyond the SipccSdp instance that owns this
  sdp_t* mSdp;
  uint16_t mLevel;
};

}

#endif
