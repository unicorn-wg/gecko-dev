/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPTRACK_H_
#define _JSEPTRACK_H_

#include <mozilla/Maybe.h>
#include <mozilla/RefPtr.h>

#include "signaling/src/jsep/JsepTransport.h"
#include "signaling/src/sdp/Sdp.h"
#include "signaling/src/sdp/SdpMediaSection.h"

namespace mozilla {
namespace jsep {

// Forward reference.
class JsepCodecConfig;  // Is this a VideoCodecConfig
class JsepTransport;

class JsepTrack {
 public:
  virtual ~JsepTrack() = 0;

  virtual mozilla::SdpMediaSection::MediaType media_type() const;
  virtual mozilla::SdpMediaSection::Protocol protocol() const = 0;
  virtual Maybe<std::string> bandwidth(const std::string& type) const = 0;

  virtual size_t num_codecs() const = 0;
  virtual nsresult get_codec(size_t index, JsepCodecConfig* config) const = 0;

  virtual bool rtcp_mux() const = 0;
};


class JsepTrackPair {
  Maybe<JsepTrack> mSending;
  Maybe<JsepTrack> mReceiving;
  RefPtr<JsepTransport> mTransport;
};

}  // namespace jsep
}  // namespace mozilla


#endif
