/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <map>

#include "nspr.h"
#include "nss.h"
#include "ssl.h"

#include "mozilla/RefPtr.h"

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

#include "FakeMediaStreams.h"
#include "FakeMediaStreamsImpl.h"

#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SipccSdpParser.h"
#include "signaling/src/jsep/JsepCodecDescription.h"
#include "signaling/src/jsep/JsepMediaStreamTrack.h"
#include "signaling/src/jsep/JsepMediaStreamTrackFake.h"
#include "signaling/src/jsep/JsepSession.h"
#include "signaling/src/jsep/JsepSessionImpl.h"
#include "signaling/src/jsep/JsepTrack.h"

using mozilla::jsep::JsepSessionImpl;
using mozilla::jsep::JsepOfferOptions;
using mozilla::jsep::JsepAnswerOptions;
using mozilla::jsep::JsepMediaStreamTrackFake;
using mozilla::jsep::JsepMediaStreamTrack;
using mozilla::jsep::JsepTrackPair;
using mozilla::jsep::JsepCodecDescription;
using mozilla::SipccSdpParser;

namespace mozilla {
class JsepSessionTestBase : public ::testing::Test {


};

class JsepSessionTest : public JsepSessionTestBase,
                        public ::testing::WithParamInterface<std::string> {
 public:
  JsepSessionTest() :
      mSessionOff("Offerer"),
      mSessionAns("Answerer") {
    AddTransportData(&mSessionOff, &mOffererTransport);
    AddTransportData(&mSessionAns, &mAnswererTransport);
  }

protected:
  struct TransportData {
    std::string mIceUfrag;
    std::string mIcePwd;
    std::map<std::string, std::string> mFingerprints;
  };

  void AddDtlsFingerprint(const std::string& alg, JsepSessionImpl* session,
                          TransportData* tdata) {
    std::string c = (session->name() == "Offerer") ? "4F" : "41";
    size_t len = (alg == "sha-1") ? 20 : 32;
    std::string fp;

    for (size_t i = 0; i < len; ++i) {
      if (!fp.empty()) {
        fp += ":";
      }
      fp += c;
    }
    session->AddDtlsFingerprint(alg, fp);
    tdata->mFingerprints[alg] = fp;
  }

  void AddTransportData(JsepSessionImpl* session, TransportData* tdata) {
    // Values here semi-borrowed from JSEP draft.
    tdata->mIceUfrag = session->name() + "-ufrag";
    tdata->mIcePwd = session->name() + "-1234567890";
    session->SetIceCredentials(tdata->mIceUfrag,
                               tdata->mIcePwd);
    AddDtlsFingerprint("sha-1", session, tdata);
    AddDtlsFingerprint("sha-256", session, tdata);
  }

  std::string CreateOffer(const Maybe<JsepOfferOptions> options = Nothing()) {
    JsepOfferOptions defaultOptions;
    const JsepOfferOptions& optionsRef = options ? *options : defaultOptions;
    std::string offer;
    nsresult rv = mSessionOff.CreateOffer(optionsRef, &offer);
    EXPECT_EQ(NS_OK, rv);

    std::cerr << "OFFER: " << offer << std::endl;

    ValidateTransport(mOffererTransport, offer);

    return offer;
  }

  void AddTracks(JsepSessionImpl* side) {
    // Add tracks.
    if (types.empty()) {
      std::string param = GetParam();
      size_t ptr = 0;

      for(;;) {
        size_t comma = param.find(',', ptr);
        std::string chunk = param.substr(ptr, comma - ptr);

        SdpMediaSection::MediaType type;
        if (chunk == "audio") {
          type = SdpMediaSection::kAudio;
        } else if (chunk == "video") {
          type = SdpMediaSection::kVideo;
        } else {
          // TODO(ekr@rtfm.com): Add Data
          MOZ_CRASH();
        }
        types.push_back(type);

        if (comma == std::string::npos)
          break;
        ptr = comma + 1;
      }
    }

    for (auto track = types.begin(); track != types.end(); ++track) {
      RefPtr<JsepMediaStreamTrack> mst(new JsepMediaStreamTrackFake(
          *track));
      side->AddTrack(mst);
    }
  }

  std::string CreateAnswer() {
    JsepAnswerOptions options;
    std::string answer;
    nsresult rv = mSessionAns.CreateAnswer(options, &answer);
    EXPECT_EQ(NS_OK, rv);

    std::cerr << "ANSWER: " << answer << std::endl;

    ValidateTransport(mAnswererTransport, answer);

    return answer;
  }


  void SetLocalOffer(const std::string& offer) {
    nsresult rv = mSessionOff.SetLocalDescription(jsep::kJsepSdpOffer, offer);
    ASSERT_EQ(NS_OK, rv);
  }

  void SetRemoteOffer(const std::string& offer) {
    nsresult rv = mSessionAns.SetRemoteDescription(jsep::kJsepSdpOffer, offer);
    ASSERT_EQ(NS_OK, rv);

    // Now verify that the right stuff is in the tracks.
    ASSERT_EQ(types.size(), mSessionAns.num_remote_tracks());
    for (size_t i = 0; i < types.size(); ++i) {
      RefPtr<JsepMediaStreamTrack> rtrack;
      ASSERT_EQ(NS_OK, mSessionAns.remote_track(i, &rtrack));
      ASSERT_EQ(types[i], rtrack->media_type());
    }
  }

  void SetLocalAnswer(const std::string& answer) {
    nsresult rv = mSessionAns.SetLocalDescription(jsep::kJsepSdpAnswer,
                                                  answer);
    ASSERT_EQ(NS_OK, rv);

    // Verify that the right stuff is in the tracks.
    ASSERT_EQ(types.size(), mSessionAns.num_negotiated_track_pairs());
    for (size_t i = 0; i < types.size(); ++i) {
      const JsepTrackPair* pair;
      ASSERT_EQ(NS_OK, mSessionAns.negotiated_track_pair(i, &pair));
      ASSERT_EQ(types[i], pair->mSending->media_type());
      ASSERT_EQ(types[i], pair->mReceiving->media_type());
    }
    DumpTrackPairs(mSessionOff);
  }

  void SetRemoteAnswer(const std::string& answer) {
    nsresult rv = mSessionOff.SetRemoteDescription(jsep::kJsepSdpAnswer,
                                                   answer);
    ASSERT_EQ(NS_OK, rv);

    // Verify that the right stuff is in the tracks.
    ASSERT_EQ(types.size(), mSessionAns.num_negotiated_track_pairs());
    for (size_t i = 0; i < types.size(); ++i) {
      const JsepTrackPair* pair;
      ASSERT_EQ(NS_OK, mSessionAns.negotiated_track_pair(i, &pair));
      ASSERT_TRUE(pair->mSending);
      ASSERT_EQ(types[i], pair->mSending->media_type());
      ASSERT_TRUE(pair->mReceiving);
      ASSERT_EQ(types[i], pair->mReceiving->media_type());
    }
    DumpTrackPairs(mSessionAns);
  }

  void DumpTrack(const jsep::JsepTrack& track) {
    std::cerr << "  type=" << track.media_type() << std::endl;
    std::cerr << "  protocol=" << track.protocol() << std::endl;
    std::cerr << "  codecs=" << std::endl;
    size_t num_codecs = track.num_codecs();
    for (size_t i = 0; i < num_codecs; ++i) {
      const JsepCodecDescription *codec;
      ASSERT_EQ(NS_OK, track.get_codec(i, &codec));
      std::cerr << "    " << codec->mName << std::endl;
    }
  }

  void DumpTrackPairs(const JsepSessionImpl& session) {
    size_t count = mSessionAns.num_negotiated_track_pairs();
    for (size_t i = 0; i < count; ++i) {
      std::cerr << "Track pair " << i << std::endl;
      const JsepTrackPair* pair;
       ASSERT_EQ(NS_OK, mSessionAns.negotiated_track_pair(i, &pair));
       if (pair->mSending) {
         std::cerr << "Sending-->" << std::endl;
         DumpTrack(*pair->mSending);
       }
       if (pair->mReceiving) {
         std::cerr << "Receiving-->" << std::endl;
         DumpTrack(*pair->mReceiving);
       }
    }
  }

  JsepSessionImpl mSessionOff;
  JsepSessionImpl mSessionAns;
  std::vector<SdpMediaSection::MediaType> types;

private:
  void ValidateTransport(TransportData& source, const std::string& sdp_str) {
    SipccSdpParser parser;
    auto sdp = mozilla::Move(parser.Parse(sdp_str));
    ASSERT_TRUE(sdp) << "Should have valid SDP";
    size_t num_m_sections = sdp->GetMediaSectionCount();
    for (size_t i =0; i < num_m_sections; ++i) {
      const SdpAttributeList& attrs = sdp->GetMediaSection(i).
          GetAttributeList();
      ASSERT_EQ(source.mIceUfrag, attrs.GetIceUfrag());
      ASSERT_EQ(source.mIcePwd, attrs.GetIcePwd());
      const SdpFingerprintAttributeList& fps =
          attrs.GetFingerprint();
      for (auto fp = fps.mFingerprints.begin();
           fp != fps.mFingerprints.end(); ++fp) {
        std::string alg_str = "None";

        if (fp->hashFunc == SdpFingerprintAttributeList::kSha1) {
          alg_str = "sha-1";
        } else if (fp->hashFunc == SdpFingerprintAttributeList::kSha256) {
          alg_str = "sha-256";
        }

        ASSERT_EQ(source.mFingerprints[alg_str], fp->fingerprint);
      }
      ASSERT_EQ(source.mFingerprints.size(), fps.mFingerprints.size());
    }
  }

  TransportData mOffererTransport;
  TransportData mAnswererTransport;
};

TEST_F(JsepSessionTestBase, CreateDestroy) {
}

TEST_P(JsepSessionTest, CreateOffer) {
  AddTracks(&mSessionOff);
  CreateOffer();
}

TEST_P(JsepSessionTest, CreateOfferSetLocal) {
  AddTracks(&mSessionOff);
  std::string offer = CreateOffer();
  SetLocalOffer(offer);
}

TEST_P(JsepSessionTest, CreateOfferSetLocalSetRemote) {
  AddTracks(&mSessionOff);
  std::string offer = CreateOffer();
  SetLocalOffer(offer);
  SetRemoteOffer(offer);
}

TEST_P(JsepSessionTest, CreateOfferSetLocalSetRemoteCreateAnswer) {
  AddTracks(&mSessionOff);
  std::string offer = CreateOffer();
  SetLocalOffer(offer);
  SetRemoteOffer(offer);
  AddTracks(&mSessionAns);
  std::string answer = CreateAnswer();
}

TEST_P(JsepSessionTest, CreateOfferSetLocalSetRemoteCreateAnswerSetLocal) {
  AddTracks(&mSessionOff);
  std::string offer = CreateOffer();
  SetLocalOffer(offer);
  SetRemoteOffer(offer);
  AddTracks(&mSessionAns);
  std::string answer = CreateAnswer();
  SetLocalAnswer(answer);
}

TEST_P(JsepSessionTest, FullCall) {
  AddTracks(&mSessionOff);
  std::string offer = CreateOffer();
  SetLocalOffer(offer);
  SetRemoteOffer(offer);
  AddTracks(&mSessionAns);
  std::string answer = CreateAnswer();
  SetLocalAnswer(answer);
  SetRemoteAnswer(answer);
}

INSTANTIATE_TEST_CASE_P(Variants, JsepSessionTest,
                        ::testing::Values("audio", "video", "audio,video"));

// offerToReceiveXxx variants

TEST_F(JsepSessionTest, CreateOfferRecvOnlyLines) {
  JsepOfferOptions options;
  options.mOfferToReceiveAudio = Some(static_cast<size_t>(1U));
  options.mOfferToReceiveVideo = Some(static_cast<size_t>(2U));
  std::string offer = CreateOffer(Some(options));

  SipccSdpParser parser;
  auto outputSdp = mozilla::Move(parser.Parse(offer));
  ASSERT_TRUE(outputSdp) << "Should have valid SDP";

  ASSERT_EQ(3U, outputSdp->GetMediaSectionCount());
  ASSERT_EQ(SdpMediaSection::kAudio, outputSdp->GetMediaSection(0)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kRecvonly, outputSdp->GetMediaSection(0)
            .GetAttributeList().GetDirection());
  ASSERT_EQ(SdpMediaSection::kVideo, outputSdp->GetMediaSection(1)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kRecvonly, outputSdp->GetMediaSection(1)
            .GetAttributeList().GetDirection());
  ASSERT_EQ(SdpMediaSection::kVideo, outputSdp->GetMediaSection(2)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kRecvonly, outputSdp->GetMediaSection(2)
            .GetAttributeList().GetDirection());
}

TEST_F(JsepSessionTest, CreateOfferSendOnlyLines) {
  RefPtr<JsepMediaStreamTrack> msta(new JsepMediaStreamTrackFake(
      SdpMediaSection::kAudio));
  mSessionOff.AddTrack(msta);
  RefPtr<JsepMediaStreamTrack> mstv1(new JsepMediaStreamTrackFake(
      SdpMediaSection::kVideo));
  mSessionOff.AddTrack(mstv1);
  RefPtr<JsepMediaStreamTrack> mstv2(new JsepMediaStreamTrackFake(
      SdpMediaSection::kVideo));
  mSessionOff.AddTrack(mstv2);

  JsepOfferOptions options;
  options.mOfferToReceiveAudio = Some(static_cast<size_t>(0U));
  options.mOfferToReceiveVideo = Some(static_cast<size_t>(1U));
  std::string offer = CreateOffer(Some(options));

  SipccSdpParser parser;
  auto outputSdp = mozilla::Move(parser.Parse(offer));
  ASSERT_TRUE(outputSdp) << "Should have valid SDP";

  ASSERT_EQ(3U, outputSdp->GetMediaSectionCount());
  ASSERT_EQ(SdpMediaSection::kAudio, outputSdp->GetMediaSection(0)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kSendonly, outputSdp->GetMediaSection(0)
            .GetAttributeList().GetDirection());
  ASSERT_EQ(SdpMediaSection::kVideo, outputSdp->GetMediaSection(1)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kSendrecv, outputSdp->GetMediaSection(1)
            .GetAttributeList().GetDirection());
  ASSERT_EQ(SdpMediaSection::kVideo, outputSdp->GetMediaSection(2)
            .GetMediaType());
  ASSERT_EQ(SdpDirectionAttribute::kSendonly, outputSdp->GetMediaSection(2)
            .GetAttributeList().GetDirection());
}

} // namespace mozilla

int main(int argc, char **argv) {
  NSS_NoDB_Init(nullptr);
  NSS_SetDomesticPolicy();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
