/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "logging.h"

#include "signaling/src/jsep/JsepSessionImpl.h"

#include "nspr.h"
#include "nss.h"
#include "pk11pub.h"


#include <mozilla/Move.h>
#include <mozilla/UniquePtr.h>

#include "signaling/src/sdp/Sdp.h"
#include "signaling/src/sdp/SipccSdp.h"
#include "signaling/src/sdp/SipccSdpParser.h"

namespace mozilla {
namespace jsep {

MOZ_MTLOG_MODULE("jsep")


// Represents a remote track. Only exposed via the abstract interface.
class JsepMediaStreamTrackRemote : public JsepMediaStreamTrack {
 public:
  JsepMediaStreamTrackRemote(mozilla::SdpMediaSection::MediaType type) :
      mType(type) {}

  virtual mozilla::SdpMediaSection::MediaType media_type() const MOZ_OVERRIDE {
    return mType;
  }

 private:
  virtual ~JsepMediaStreamTrackRemote() {}

  mozilla::SdpMediaSection::MediaType mType;
};


// TODO(ekr@rtfm.com): Add state checks.

void JsepSessionImpl::Init() {
  SECStatus rv = PK11_GenerateRandom(
      reinterpret_cast<unsigned char *>(&mSessionId), sizeof(mSessionId));
  mSessionId >>= 2; // Discard high order bits.
  if (rv != SECSuccess) {
    MOZ_CRASH();
  }

  SetupDefaultCodecs();
}

nsresult JsepSessionImpl::AddTrack(const RefPtr<JsepMediaStreamTrack>& track) {
  JsepSendingTrack strack;
  strack.mTrack = track;

  mLocalTracks.push_back(strack);

  return NS_OK;
}

nsresult JsepSessionImpl::local_track(
    size_t index,
    RefPtr<JsepMediaStreamTrack>* track) const {
  if (index >= mLocalTracks.size()) {
    return NS_ERROR_INVALID_ARG;
  }

  *track = mLocalTracks[index].mTrack;

  return NS_OK;
}

nsresult JsepSessionImpl::remote_track(
    size_t index,
    RefPtr<JsepMediaStreamTrack>* track) const {
  if (index >= mRemoteTracks.size()) {
    return NS_ERROR_INVALID_ARG;
  }

  *track = mRemoteTracks[index].mTrack;

  return NS_OK;
}

nsresult JsepSessionImpl::CreateOffer(const JsepOfferOptions& options,
                                      std::string* offer) {
  UniquePtr<Sdp> sdp;

  // Make the basic SDP that is common to offer/answer.
  nsresult rv = CreateGenericSDP(&sdp);
  if (NS_FAILED(rv))
    return rv;

  // Now add all the m-lines that we are attempting to negotiate.
  size_t mline_index = 0;

  for (auto track = mLocalTracks.begin();
       track != mLocalTracks.end(); ++track) {
    // TODO(ekr@rtfm.com): process options for sendrecv versus sendonly.
    SdpMediaSection& msection =
      sdp->AddMediaSection(track->mTrack->media_type());

    for (auto codec = mCodecs.begin(); codec != mCodecs.end(); ++codec) {
      if (codec->mEnabled && (codec->mType == track->mTrack->media_type())) {
        msection.AddCodec(codec->mDefaultPt,
                          codec->mName,
                          codec->mClock,
                          codec->mChannels);
      }
    }
    track->mAssignedMLine = Some(mline_index);
    ++mline_index;
  }

  // TODO(ekr@rtfm.com): Do renegotiation.
  *offer = sdp->toString();
  mGeneratedLocalDescription = Move(sdp);

  return NS_OK;
}

nsresult JsepSessionImpl::CreateAnswer(const JsepAnswerOptions& options,
                                       std::string* answer) {
  // This is the heart of the negotiation code. Depressing that it's
  // so bad.
  //
  // Here's the current algorithm:
  // 1. Walk through all the m-lines on the other side.
  // 2. For each m-line, walk through all of our local tracks
  //    in sequence and see if any are unassigned. If so, assign
  //    them and mark it sendrecv, otherwise it's recvonly.
  // 3. Just replicate their media attributes.
  // 4. Profit.
  UniquePtr<Sdp> sdp;

  // Make the basic SDP that is common to offer/answer.
  nsresult rv = CreateGenericSDP(&sdp);
  if (NS_FAILED(rv))
    return rv;

  const Sdp& offer = *mPendingRemoteDescription;

  size_t num_m_lines = offer.GetMediaSectionCount();

  for (size_t i = 0; i < num_m_lines; ++i) {
    const SdpMediaSection& remote_msection = offer.GetMediaSection(i);
    SdpMediaSection& msection =
      sdp->AddMediaSection(remote_msection.GetMediaType());

    bool matched = false;

    for (auto track = mLocalTracks.begin();
       track != mLocalTracks.end(); ++track) {
      if (track->mAssignedMLine.isSome())
        continue;
      if (track->mTrack->media_type() != remote_msection.GetMediaType())
        continue;

      matched = true;
      track->mAssignedMLine = Some(i);
      break;
    }

    // If we matched, then it's sendrecv, else recvonly. No way to
    // do sendonly here. inactive would be used if we had a codec
    // mismatch, but we don't have that worked out yet.
    msection.GetAttributeList().SetAttribute(
      new SdpDirectionAttribute(matched ?
                                SdpDirectionAttribute::kSendrecv :
                                SdpDirectionAttribute::kRecvonly));
  }

  *answer = sdp->toString();
  mGeneratedLocalDescription = Move(sdp);

  return NS_OK;
}

nsresult JsepSessionImpl::SetLocalDescription(JsepSdpType type,
                                              const std::string& sdp) {
  // TODO(ekr@rtfm.com): Check state.
  UniquePtr<Sdp> parsed;
  nsresult rv = ParseSdp(sdp, &parsed);
  if (NS_FAILED(rv))
    return rv;

  // TODO(ekr@rtfm.com): Compare the generated offer to the passed
  // in argument.

  SetState(kJsepStateHaveLocalOffer);
  return NS_OK;
}

nsresult JsepSessionImpl::SetRemoteDescription(JsepSdpType type,
                                               const std::string& sdp) {
  // Parse.
  UniquePtr<Sdp> parsed;
  nsresult rv = ParseSdp(sdp, &parsed);
  if (NS_FAILED(rv))
    return rv;

  rv = NS_ERROR_FAILURE;

  switch (type ) {
    case kJsepSdpOffer:
      rv = SetRemoteDescriptionOffer(Move(parsed));
      break;
    case kJsepSdpAnswer:
    case kJsepSdpPranswer:
      rv = SetRemoteDescriptionAnswer(type, Move(parsed));
      break;
  }

  return rv;
}

nsresult JsepSessionImpl::ParseSdp(const std::string& sdp,
                                   UniquePtr<Sdp>* parsedp) {
  UniquePtr<Sdp> parsed = mParser.Parse(sdp);
  if (!parsed) {
    MOZ_MTLOG(ML_ERROR, "Bogus SDP = " + sdp);
    const std::vector<std::pair<size_t, std::string>>& errors =
        mParser.GetParseErrors();
    for (auto err = errors.begin(); err != errors.end(); ++err) {
      MOZ_MTLOG(ML_ERROR, " Error at "
                << err->first
                << ": "
                << err->second);
    }
    return NS_ERROR_FAILURE;
  }

  *parsedp = Move(parsed);
  return NS_OK;
}

nsresult JsepSessionImpl::SetRemoteDescriptionOffer(UniquePtr<Sdp> offer) {
  // TODO(ekr@rtfm.com): Check state.
  size_t num_m_lines = offer->GetMediaSectionCount();

  for (size_t i = 0; i < num_m_lines; ++i) {
    const SdpMediaSection& msection = offer->GetMediaSection(i);
    JsepMediaStreamTrackRemote* remote = new JsepMediaStreamTrackRemote(
        msection.GetMediaType());
    JsepReceivingTrack rtrack;
    rtrack.mTrack = remote;
    mRemoteTracks.push_back(rtrack);
  }

  mPendingRemoteDescription = Move(offer);

  SetState(kJsepStateHaveRemoteOffer);
  return NS_OK;
}

nsresult JsepSessionImpl::SetRemoteDescriptionAnswer(
    JsepSdpType type, UniquePtr<Sdp> answer) {
  MOZ_CRASH();
}


nsresult JsepSessionImpl::CreateGenericSDP(UniquePtr<Sdp>* sdpp) {
  // draft-ietf-rtcweb-jsep-08 Section 5.2.1:
  //  o  The second SDP line MUST be an "o=" line, as specified in
  //     [RFC4566], Section 5.2.  The value of the <username> field SHOULD
  //     be "-".  The value of the <sess-id> field SHOULD be a
  //     cryptographically random number.  To ensure uniqueness, this
  //     number SHOULD be at least 64 bits long.  The value of the <sess-
  //     version> field SHOULD be zero.  The value of the <nettype>
  //     <addrtype> <unicast-address> tuple SHOULD be set to a non-
  //     meaningful address, such as IN IP4 0.0.0.0, to prevent leaking the
  //     local address in this field.  As mentioned in [RFC4566], the
  //     entire o= line needs to be unique, but selecting a random number
  //     for <sess-id> is sufficient to accomplish this.


  auto origin = MakeUnique<SdpOrigin>("-",
                                      mSessionId,
                                      mSessionVersion,
                                      sdp::kIPv4,
                                      "0.0.0.0");

  *sdpp = MakeUnique<SipccSdp>(origin.release());

  return NS_OK;
}

void JsepSessionImpl::SetupDefaultCodecs() {
  // Supported audio codecs.
  mCodecs.push_back(JsepCodecDescription(
      SdpMediaSection::kAudio,
      109,
      "opus",
      48000,
      2
                      ));

  mCodecs.push_back(JsepCodecDescription(
      SdpMediaSection::kAudio,
      9,
      "G722",
      8000,
      0  // This means default 1
                      ));

  mCodecs.push_back(JsepCodecDescription(
      SdpMediaSection::kAudio,
      0,
      "PCMU",
      8000,
      0  // This means default 1
                      ));

  mCodecs.push_back(JsepCodecDescription(
      SdpMediaSection::kAudio,
      8,
      "PCMA",
      8000,
      0  // This means default 1
                      ));

  // Supported video codecs.
  mCodecs.push_back(JsepCodecDescription(
      SdpMediaSection::kVideo,
      120,
      "VP8",
      9000,
      0  // This means default 1
                      ));
}

void JsepSessionImpl::SetState(JsepSignalingState state) {
  if (state == mState)
    return;

  MOZ_MTLOG(ML_NOTICE, "[" << mName << "]: " <<
            state_str(mState) << " -> " << state_str(state));
  mState = state;
}

}  // namespace jsep
}  // namespace mozilla
