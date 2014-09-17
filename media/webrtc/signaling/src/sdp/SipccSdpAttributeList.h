/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPATTRIBUTELIST_H_
#define _SIPCCSDPATTRIBUTELIST_H_

#include "signaling/src/sdp/SdpAttributeList.h"
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdpMediaSection;

class SipccSdpAttributeList : public SdpAttributeList
{
  friend class SipccSdpMediaSection;
public:
private:
  SipccSdpAttributeList(sdp_t* sdp, uint16_t level)
  : mSdp(sdp), mLevel(level) {}

  sdp_t* mSdp;
  uint16_t mLevel;
};

}

#endif
