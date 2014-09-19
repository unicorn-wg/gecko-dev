/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPCODECDESCRIPTION_H_
#define _JSEPCODECDESCRIPTION_H_

#include <string>
#include "signaling/src/sdp/SdpMediaSection.h"

namespace mozilla {
namespace jsep {

// A single entry in our list of known codecs.
struct JsepCodecDescription {
  JsepCodecDescription(mozilla::SdpMediaSection::MediaType type,
                       uint8_t default_pt,
                       const std::string& name,
                       uint32_t clock,
                       uint32_t channels,
                       bool enabled = true) :
      mType(type),
      mDefaultPt(default_pt),
      mName(name),
      mClock(clock),
      mChannels(channels),
      mEnabled(enabled) {}

  // These should be const except for enabled, but then
  // I can't assign, which is sad.

  mozilla::SdpMediaSection::MediaType mType;
  uint8_t mDefaultPt;
  std::string mName;
  uint32_t mClock;
  uint32_t mChannels;
  bool mEnabled;
};

}  // namespace jsep
}  // namespace mozilla

#endif
