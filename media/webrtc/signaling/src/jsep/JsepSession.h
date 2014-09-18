/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPSESSION_H_
#define _JSEPSESSION_H_

#include <string>
#include <vector>

namespace mozilla {
namespace jsep {

// Forward declarations tracks and track pairs
class JsepTrackPair;

enum JsepSignalingState {
  kJsepStateStable,
  kJsepStateHaveLocalOffer,
  kJsepStateHaveRemoteOffer,
  kJsepStateHaveLocalPranswer,
  kJsepStateHaveRemotePranswer,
  kJsepStateClosed
};

enum JsepSdpType {
  kJsepSdpOffer,
  kJsepSdpAnswer,
  kJsepSdpPranswer,
};

class JsepOAOptions {};
class JsepOfferOptions : public JsepOAOptions {}
class JsepAnswerOptions : public JsepOAOptions {}

class JsepSession {
 public:
  JsepSession(const std::string& name) : mName(name) {}

  // Accessors for basic properties.
  virtual const std::string& name() const { return mName; }
  virtual const std::string& state() const { return mState; }

  // TODO(ekr@rtfm.com): Somehow let this know about media streams.

  // Basic JSEP operations.
  virtual nsresult CreateOffer(const JsepOfferOptions& options) = 0;
  virtual nsresult CreateAnswer(const JsepAnswerOptions& options) = 0;
  virtual nsresult SetLocalDescription(JsepSdpType type,
                                       const UniquePtr<mozilla::Sdp>& sdp) = 0;
  virtual nsresult SetRemoteDescription(JsepSdpType type,
                                        const UniquePtr<mozilla::Sdp>& sdp) = 0;

  // Access the negotiated track pairs.
  virtual nsresult num_negotiated_track_pairs(size_t* pairs) const = 0;
  virtual nsresult track_pair(size_t index, const JsepTrackPair** pair) const = 0;

 private:
  const std::string mName;
  JsepSignalingState mState;
};

}  // namespace jsep
}  // namespace mozilla

#endif
