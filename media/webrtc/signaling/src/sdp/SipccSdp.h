/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _sdp_h_
#define _sdp_h_

#include "Attributes.h"
#include "UniquePtr.h"

#include "signaling/src/sdp/Sdp.h"
#include "signaling/src/sdp/sipcc/sdp.h"

namespace mozilla {

class SipccSdpParser;

class SipccSdp MOZ_FINAL : public Sdp
{
  friend class SipccSdpParser;
private:
  SipccSdp(sdp_t* sdp) : mSdp(sdp) {}
  ~SipccSdp()
  {
    sdp_free_description(mSdp);
  }

  sdp_t *mSdp;
};

} // namespace mozilla

#endif // _sdp_h_
