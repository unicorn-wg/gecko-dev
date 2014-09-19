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
class JsepSessionTest : public ::testing::Test {
  public:
    JsepSessionTest() {}

    JsepSessionImpl mSession;
    SipccSdpParser mParser;
};

TEST_F(JsepSessionTest, CreateDestroy) {
}

TEST_F(JsepSessionTest, CreateOfferAudio1) {
  JsepOfferOptions options;
  std::string offer;

  RefPtr<JsepMediaStreamTrack> audio(new JsepMediaStreamTrackFake(
      SdpMediaSection::kAudio));
  mSession.AddTrack(audio);
  nsresult rv = mSession.CreateOffer(options, &offer);
  ASSERT_EQ(NS_OK, rv);

  std::cerr << offer << std::endl;
}

TEST_F(JsepSessionTest, CreateOfferAudio1SetLocal) {
  JsepOfferOptions options;
  std::string offer;

  RefPtr<JsepMediaStreamTrack> audio(new JsepMediaStreamTrackFake(
      SdpMediaSection::kAudio));
  mSession.AddTrack(audio);
  nsresult rv = mSession.CreateOffer(options, &offer);
  ASSERT_EQ(NS_OK, rv);

  std::cerr << offer << std::endl;

  rv = mSession.SetLocalDescription(jsep::kJsepSdpOffer, offer);
  ASSERT_EQ(NS_OK, rv);
}

TEST_F(JsepSessionTest, CreateOfferVideo1) {
  JsepOfferOptions options;
  std::string offer;

  RefPtr<JsepMediaStreamTrack> video(new JsepMediaStreamTrackFake(
      SdpMediaSection::kVideo));
  mSession.AddTrack(video);
  nsresult rv = mSession.CreateOffer(options, &offer);
  ASSERT_EQ(NS_OK, rv);

  std::cerr << offer << std::endl;
}

TEST_F(JsepSessionTest, CreateOfferAudio1Video1) {
  JsepOfferOptions options;
  std::string offer;

  RefPtr<JsepMediaStreamTrack> audio(new JsepMediaStreamTrackFake(
      SdpMediaSection::kAudio));
  mSession.AddTrack(audio);

  RefPtr<JsepMediaStreamTrack> video(new JsepMediaStreamTrackFake(
      SdpMediaSection::kVideo));
  mSession.AddTrack(video);
  nsresult rv = mSession.CreateOffer(options, &offer);
  ASSERT_EQ(NS_OK, rv);

  std::cerr << offer << std::endl;
}


} // namespace mozilla

int main(int argc, char **argv) {
  NSS_NoDB_Init(nullptr);
  NSS_SetDomesticPolicy();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

