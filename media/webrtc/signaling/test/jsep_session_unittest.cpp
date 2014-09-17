/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

#include "signaling/src/jsep/JsepSession.h"
#include "signaling/src/sdp/SdpParser.h"

using mozilla::JsepSession;
using mozilla::SdpParser;

namespace test {
class JsepSessionTest : public ::testing::Test {
  public:
    JsepSessionTest() {}

    JsepSession session_;
    SdpParser parser_;
};

TEST_F(JsepSessionTest, CreateDestroy) {
}

} // namespace test

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

