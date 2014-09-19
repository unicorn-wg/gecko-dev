/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>

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
#include "signaling/src/jsep/JsepMediaStreamTrack.h"
#include "signaling/src/jsep/JsepMediaStreamTrackFake.h"
#include "signaling/src/jsep/JsepSession.h"
#include "signaling/src/jsep/JsepSessionImpl.h"
#include "signaling/src/jsep/JsepTrack.h"

using mozilla::jsep::JsepSessionImpl;
using mozilla::jsep::JsepOfferOptions;
using mozilla::jsep::JsepMediaStreamTrackFake;
using mozilla::jsep::JsepMediaStreamTrack;
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
  }

  void SetUp() {
    // Add tracks.
    std::string param =  GetParam();
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

protected:
  std::string CreateOffer() {
    for (auto track = types.begin(); track != types.end(); ++track) {
      RefPtr<JsepMediaStreamTrack> mst(new JsepMediaStreamTrackFake(
          *track));
      mSessionOff.AddTrack(mst);
    }

    JsepOfferOptions options;
    std::string offer;
    nsresult rv = mSessionOff.CreateOffer(options, &offer);
    EXPECT_EQ(NS_OK, rv);

    std::cerr << offer << std::endl;

    return offer;
  }

  void SetLocal(const std::string& offer) {
    nsresult rv = mSessionOff.SetLocalDescription(jsep::kJsepSdpOffer, offer);
    ASSERT_EQ(NS_OK, rv);
  }

  void SetRemote(const std::string& offer) {
    nsresult rv = mSessionOff.SetRemoteDescription(jsep::kJsepSdpOffer, offer);
    ASSERT_EQ(NS_OK, rv);
  }


  JsepSessionImpl mSessionOff;
  JsepSessionImpl mSessionAns;
  std::vector<SdpMediaSection::MediaType> types;
};

TEST_F(JsepSessionTestBase, CreateDestroy) {
}

TEST_P(JsepSessionTest, CreateOffer) {
  CreateOffer();
}

TEST_P(JsepSessionTest, CreateOfferSetLocal) {
  std::string offer = CreateOffer();
  SetLocal(offer);
}

TEST_P(JsepSessionTest, CreateOfferSetLocalSetRemote) {
  std::string offer = CreateOffer();
  SetLocal(offer);
  SetRemote(offer);
}

INSTANTIATE_TEST_CASE_P(Variants, JsepSessionTest,
                        ::testing::Values("audio", "video"));

} // namespace mozilla

int main(int argc, char **argv) {
  NSS_NoDB_Init(nullptr);
  NSS_SetDomesticPolicy();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

