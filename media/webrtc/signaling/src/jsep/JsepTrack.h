/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPTRACK_H_
#define _JSEPTRACK_H_

#include <mozilla/RefPtr.h>
#include <mozilla/UniquePtr.h>

#include "signaling/src/jsep/JsepTransport.h"
#include "signaling/src/sdp/Sdp.h"
#include "signaling/src/sdp/SdpMediaSection.h"

namespace mozilla {
namespace jsep {

// Forward reference.
class JsepCodecDescription;  // Is this a VideoCodecConfig
class JsepTransport;

class JsepTrack {
 public:
virtual ~JsepTrack();

  virtual mozilla::SdpMediaSection::MediaType media_type();
  virtual mozilla::SdpMediaSection::Protocol protocol();
  virtual Maybe<std::string> bandwidth(const std::string& type) const;

  virtual size_t num_codecs() const;
  virtual nsresult get_codec(size_t index, JsepCodecDescription* config);

  virtual bool rtcp_mux() const = 0;
};


class JsepTrackPair {
  UniquePtr<JsepTrack> mSending;
  UniquePtr<JsepTrack> mReceiving;
//  RefPtr<JsepTransport> mTransport;
};

}  // namespace jsep
}  // namespace mozilla


#endif
