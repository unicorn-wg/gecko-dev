/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPSESSIONIMPL_H_
#define _JSEPSESSIONIMPL_H_


#include <vector>

#include "signaling/src/jsep/JsepCodecDescription.h"
#include "signaling/src/jsep/JsepMediaStreamTrack.h"
#include "signaling/src/jsep/JsepSession.h"
#include "signaling/src/sdp/SipccSdpParser.h"

namespace mozilla {
namespace jsep {

class JsepSessionImpl : public JsepSession {
 public:
  JsepSessionImpl(const std::string& name) :
      JsepSession(name) {
    Init();
  }

  JsepSessionImpl() :
      JsepSession("anonymous"),
      mSessionId(0),
      mSessionVersion(0) {
    Init();
  }

   virtual ~JsepSessionImpl() {}

  // Implement JsepSession methods.
  virtual nsresult AddTrack(const RefPtr<JsepMediaStreamTrack>& track)
      MOZ_OVERRIDE;
  virtual nsresult RemoveTrack(size_t track_index) MOZ_OVERRIDE {
    MOZ_CRASH();  // Stub
  }
  virtual nsresult ReplaceTrack(
    size_t track_index,
      const RefPtr<JsepMediaStreamTrack>& track) MOZ_OVERRIDE {
    MOZ_CRASH(); // Stub
  }

  virtual size_t num_local_tracks() const MOZ_OVERRIDE {
    return mLocalTracks.size();
  }
  virtual nsresult local_track(size_t index,
                               RefPtr<JsepMediaStreamTrack>* track)
    const MOZ_OVERRIDE;

  virtual size_t num_remote_tracks() const MOZ_OVERRIDE {
    return mRemoteTracks.size();
  }
  virtual nsresult remote_track(size_t index,
                                RefPtr<JsepMediaStreamTrack>* track)
      const MOZ_OVERRIDE;

  virtual nsresult CreateOffer(const JsepOfferOptions& options,
                               std::string* offer) MOZ_OVERRIDE;
  virtual nsresult SetLocalDescription(JsepSdpType type,
                                       const std::string& sdp) MOZ_OVERRIDE;

  virtual nsresult SetRemoteDescription(JsepSdpType type,
                                        const std::string& sdp) MOZ_OVERRIDE;

// STUBS BELOW THIS POINT
  virtual nsresult CreateAnswer(const JsepAnswerOptions& options,
                                std::string* answer) MOZ_OVERRIDE {
    MOZ_CRASH(); }

  // Access the negotiated track pairs.
  virtual nsresult num_negotiated_track_pairs(size_t* pairs)
      const MOZ_OVERRIDE { MOZ_CRASH(); }
  virtual nsresult track_pair(size_t index, const JsepTrackPair** pair)
      const MOZ_OVERRIDE { MOZ_CRASH(); }

 private:
  struct JsepSendingTrack {
    RefPtr<JsepMediaStreamTrack> mTrack;
    Maybe<size_t> mAssignedMLine;
  };

  struct JsepReceivingTrack {
    RefPtr<JsepMediaStreamTrack> mTrack;
    Maybe<size_t> mAssignedMLine;
  };

  void Init();
  nsresult CreateGenericSDP(UniquePtr<Sdp>* sdp);
  void SetupDefaultCodecs();
  void SetState(JsepSignalingState state);
  nsresult ParseSdp(const std::string& sdp, UniquePtr<Sdp>* parsedp);
  nsresult SetRemoteDescriptionOffer(UniquePtr<Sdp> offer);
  nsresult SetRemoteDescriptionAnswer(JsepSdpType type,
                                      UniquePtr<Sdp> answer);

  std::vector<JsepSendingTrack> mLocalTracks;
  std::vector<JsepSendingTrack> mRemoteTracks;

  uint64_t mSessionId;
  uint64_t mSessionVersion;
  UniquePtr<Sdp> mGeneratedLocalDescription; // Created but not set.
  UniquePtr<Sdp> mCurrentLocalDescription;
  UniquePtr<Sdp> mCurrentRemoteDescription;
  UniquePtr<Sdp> mPendingLocalDescription;
  UniquePtr<Sdp> mPendingRemoteDescription;
  std::vector<JsepCodecDescription> mCodecs;
  SipccSdpParser mParser;
};

}  // namespace jsep
}  // namespace mozilla

#endif
