/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPMEDIASTREAMTRACK_H_
#define _JSEPMEDIASTREAMTRACK_H_

namespace mozilla {
namespace jsep {

// This class represents a MST that comes in from the browser.
// It's only used to add to JsepSession to indicate what we
// intend to send.
class JsepMediaStreamTrack {
 public:
  virtual mozilla::SdpMediaSection::MediaType media_type() const = 0;
};

}  // namespace jsep
}  // namespace mozilla


#endif
