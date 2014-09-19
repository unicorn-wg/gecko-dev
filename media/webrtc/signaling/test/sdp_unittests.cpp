/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "timecard.h"

#include "CSFLog.h"

#include <string>
#include <sstream>

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

#include "nspr.h"
#include "nss.h"
#include "ssl.h"

#include "nsThreadUtils.h"
#include "FakeMediaStreams.h"
#include "FakeMediaStreamsImpl.h"
#include "PeerConnectionImpl.h"
#include "PeerConnectionCtx.h"

#include "mtransport_test_utils.h"
MtransportTestUtils *test_utils;
nsCOMPtr<nsIThread> gThread;

#include "signaling/src/sdp/SipccSdpParser.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SdpAttribute.h"

extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
#include "signaling/src/sdp/sipcc/sdp_private.h"
}

using namespace mozilla;

namespace test {

static bool SetupGlobalThread() {
  if (!gThread) {
    nsIThread *thread;

    nsresult rv = NS_NewNamedThread("pseudo-main",&thread);
    if (NS_FAILED(rv))
      return false;

    gThread = thread;
    sipcc::PeerConnectionCtx::InitializeGlobal(gThread,
                                               test_utils->sts_target());
  }
  return true;
}

class SdpTest : public ::testing::Test {
  public:
    SdpTest() : sdp_ptr_(nullptr) {
      sdp_media_e supported_media[] = {
        SDP_MEDIA_AUDIO,
        SDP_MEDIA_VIDEO,
        SDP_MEDIA_APPLICATION,
        SDP_MEDIA_DATA,
        SDP_MEDIA_CONTROL,
        SDP_MEDIA_NAS_RADIUS,
        SDP_MEDIA_NAS_TACACS,
        SDP_MEDIA_NAS_DIAMETER,
        SDP_MEDIA_NAS_L2TP,
        SDP_MEDIA_NAS_LOGIN,
        SDP_MEDIA_NAS_NONE,
        SDP_MEDIA_IMAGE,
      };

      config_p_ = sdp_init_config();
      unsigned int i;
      for (i = 0; i < sizeof(supported_media) / sizeof(sdp_media_e); i++) {
        sdp_media_supported(config_p_, supported_media[i], true);
      }
      sdp_nettype_supported(config_p_, SDP_NT_INTERNET, true);
      sdp_addrtype_supported(config_p_, SDP_AT_IP4, true);
      sdp_addrtype_supported(config_p_, SDP_AT_IP6, true);
      sdp_transport_supported(config_p_, SDP_TRANSPORT_RTPSAVPF, true);
      sdp_transport_supported(config_p_, SDP_TRANSPORT_UDPTL, true);
      sdp_require_session_name(config_p_, false);
    }

    static void SetUpTestCase() {
      ASSERT_TRUE(SetupGlobalThread());
    }

    void SetUp() {
      final_level_ = 0;
      sdp_ptr_ = nullptr;
    }

    static void TearDownTestCase() {
      gThread = nullptr;
    }

    void ResetSdp() {
      if (!sdp_ptr_) {
        sdp_free_description(sdp_ptr_);
      }
      sdp_ptr_ = sdp_init_description(config_p_);
    }

    void ParseSdp(const std::string &sdp_str) {
      const char *buf = sdp_str.data();
      ResetSdp();
      ASSERT_EQ(sdp_parse(sdp_ptr_, buf, sdp_str.size()), SDP_SUCCESS);
    }

    void InitLocalSdp() {
      ResetSdp();
      ASSERT_EQ(sdp_set_version(sdp_ptr_, 0), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_username(sdp_ptr_, "-"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_sessionid(sdp_ptr_, "132954853"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_version(sdp_ptr_, "0"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_network_type(sdp_ptr_, SDP_NT_INTERNET),
                SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_address_type(sdp_ptr_, SDP_AT_IP4), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_owner_address(sdp_ptr_, "198.51.100.7"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_session_name(sdp_ptr_, "SDP Unit Test"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_time_start(sdp_ptr_, "0"), SDP_SUCCESS);
      ASSERT_EQ(sdp_set_time_stop(sdp_ptr_, "0"), SDP_SUCCESS);
    }

    std::string SerializeSdp() {
      flex_string fs;
      flex_string_init(&fs);
      EXPECT_EQ(sdp_build(sdp_ptr_, &fs), SDP_SUCCESS);
      std::string body(fs.buffer);
      flex_string_free(&fs);
      return body;
    }

    // Returns "level" for new media section
    int AddNewMedia(sdp_media_e type) {
      final_level_++;
      EXPECT_EQ(sdp_insert_media_line(sdp_ptr_, final_level_), SDP_SUCCESS);
      EXPECT_EQ(sdp_set_conn_nettype(sdp_ptr_, final_level_, SDP_NT_INTERNET),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_set_conn_addrtype(sdp_ptr_, final_level_, SDP_AT_IP4),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_set_conn_address(sdp_ptr_, final_level_, "198.51.100.7"),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_set_media_type(sdp_ptr_, final_level_, SDP_MEDIA_VIDEO),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_set_media_transport(sdp_ptr_, final_level_,
                                        SDP_TRANSPORT_RTPAVP),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_set_media_portnum(sdp_ptr_, final_level_, 12345, 0),
                SDP_SUCCESS);
      EXPECT_EQ(sdp_add_media_payload_type(sdp_ptr_, final_level_, 120,
                                           SDP_PAYLOAD_NUMERIC),
                SDP_SUCCESS);
      return final_level_;
    }

    u16 AddNewRtcpFbAck(int level, sdp_rtcp_fb_ack_type_e type,
                         u16 payload = SDP_ALL_PAYLOADS) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_RTCP_FB,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_rtcp_fb_ack(sdp_ptr_, level, payload, inst_num,
                                         type), SDP_SUCCESS);
      return inst_num;
    }

    u16 AddNewRtcpFbNack(int level, sdp_rtcp_fb_nack_type_e type,
                         u16 payload = SDP_ALL_PAYLOADS) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_RTCP_FB,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_rtcp_fb_nack(sdp_ptr_, level, payload, inst_num,
                                          type), SDP_SUCCESS);
      return inst_num;
    }

    u16 AddNewRtcpFbTrrInt(int level, u32 interval,
                         u16 payload = SDP_ALL_PAYLOADS) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_RTCP_FB,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_rtcp_fb_trr_int(sdp_ptr_, level, payload, inst_num,
                                             interval), SDP_SUCCESS);
      return inst_num;
    }

    u16 AddNewRtcpFbCcm(int level, sdp_rtcp_fb_ccm_type_e type,
                         u16 payload = SDP_ALL_PAYLOADS) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_RTCP_FB,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_rtcp_fb_ccm(sdp_ptr_, level, payload, inst_num,
                                         type), SDP_SUCCESS);
      return inst_num;
    }
    u16 AddNewExtMap(int level, const char* uri) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_EXTMAP,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_extmap(sdp_ptr_, level, inst_num,
                                    uri, inst_num), SDP_SUCCESS);
      return inst_num;
    }

    u16 AddNewFmtpMaxFs(int level, u32 max_fs) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_FMTP,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_payload_type(sdp_ptr_, level, 0, inst_num,
                                               120), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_max_fs(sdp_ptr_, level, 0, inst_num, max_fs),
                                         SDP_SUCCESS);
      return inst_num;
    }

    u16 AddNewFmtpMaxFr(int level, u32 max_fr) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_FMTP,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_payload_type(sdp_ptr_, level, 0, inst_num,
                                               120), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_max_fr(sdp_ptr_, level, 0, inst_num, max_fr),
                                         SDP_SUCCESS);
      return inst_num;
    }

     u16 AddNewFmtpMaxFsFr(int level, u32 max_fs, u32 max_fr) {
      u16 inst_num = 0;
      EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, level, 0, SDP_ATTR_FMTP,
                                 &inst_num), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_payload_type(sdp_ptr_, level, 0, inst_num,
                                               120), SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_max_fs(sdp_ptr_, level, 0, inst_num, max_fs),
                                         SDP_SUCCESS);
      EXPECT_EQ(sdp_attr_set_fmtp_max_fr(sdp_ptr_, level, 0, inst_num, max_fr),
                                         SDP_SUCCESS);
      return inst_num;
    }

  protected:
    int final_level_;
    sdp_conf_options_t *config_p_;
    sdp_t *sdp_ptr_;
};

//TEST_F(SdpTest, parseRtcpFbAckRpsi) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ack rpsi\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_ACK_RPSI);
//}
//
//TEST_F(SdpTest, parseRtcpFbAckApp) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ack app\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1), SDP_RTCP_FB_ACK_APP);
//}
//
//TEST_F(SdpTest, parseRtcpFbAckAppFoo) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ack app foo\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1), SDP_RTCP_FB_ACK_APP);
//}
//
//TEST_F(SdpTest, parseRtcpFbAckFooBar) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ack foo bar\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_ACK_UNKNOWN);
//}
//
//TEST_F(SdpTest, parseRtcpFbAckFooBarBaz) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ack foo bar baz\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_ACK_UNKNOWN);
//}
//
//TEST_F(SdpTest, parseRtcpFbNack) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_BASIC);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackPli) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack pli\r\n");
//}
//
//TEST_F(SdpTest, parseRtcpFbNackSli) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack sli\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_SLI);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackRpsi) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack rpsi\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_RPSI);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackApp) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack app\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_APP);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackAppFoo) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack app foo\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_APP);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackAppFooBar) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack app foo bar\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_APP);
//}
//
//TEST_F(SdpTest, parseRtcpFbNackFooBarBaz) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 nack foo bar baz\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_UNKNOWN);
//}
//
//TEST_F(SdpTest, parseRtcpFbTrrInt0) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 trr-int 0\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_trr_int(sdp_ptr_, 1, 120, 1), 0U);
//}
//
//TEST_F(SdpTest, parseRtcpFbTrrInt123) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 trr-int 123\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_trr_int(sdp_ptr_, 1, 120, 1), 123U);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmFir) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm fir\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1), SDP_RTCP_FB_CCM_FIR);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmTmmbr) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm tmmbr\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_CCM_TMMBR);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmTmmbrSmaxpr) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm tmmbr smaxpr=456\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_CCM_TMMBR);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmTstr) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm tstr\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_CCM_TSTR);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmVbcm) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm vbcm 123 456 789\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//                                     SDP_RTCP_FB_CCM_VBCM);
//  // We don't currently parse out VBCM submessage types, since we don't have
//  // any use for them.
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmFoo) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm foo\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_CCM_UNKNOWN);
//}
//
//TEST_F(SdpTest, parseRtcpFbCcmFooBarBaz) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 ccm foo bar baz\r\n");
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_CCM_UNKNOWN);
//}
//
//TEST_F(SdpTest, parseRtcpFbFoo) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 foo\r\n");
//}
//
//TEST_F(SdpTest, parseRtcpFbFooBar) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 foo bar\r\n");
//}
//
//TEST_F(SdpTest, parseRtcpFbFooBarBaz) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:120 foo bar baz\r\n");
//}
//
//
//TEST_F(SdpTest, parseRtcpFbKitchenSink) {
//  ParseSdp(kVideoSdp +
//    "a=rtcp-fb:120 ack rpsi\r\n"
//    "a=rtcp-fb:120 ack app\r\n"
//    "a=rtcp-fb:120 ack app foo\r\n"
//    "a=rtcp-fb:120 ack foo bar\r\n"
//    "a=rtcp-fb:120 ack foo bar baz\r\n"
//    "a=rtcp-fb:120 nack\r\n"
//    "a=rtcp-fb:120 nack pli\r\n"
//    "a=rtcp-fb:120 nack sli\r\n"
//    "a=rtcp-fb:120 nack rpsi\r\n"
//    "a=rtcp-fb:120 nack app\r\n"
//    "a=rtcp-fb:120 nack app foo\r\n"
//    "a=rtcp-fb:120 nack app foo bar\r\n"
//    "a=rtcp-fb:120 nack foo bar baz\r\n"
//    "a=rtcp-fb:120 trr-int 0\r\n"
//    "a=rtcp-fb:120 trr-int 123\r\n"
//    "a=rtcp-fb:120 ccm fir\r\n"
//    "a=rtcp-fb:120 ccm tmmbr\r\n"
//    "a=rtcp-fb:120 ccm tmmbr smaxpr=456\r\n"
//    "a=rtcp-fb:120 ccm tstr\r\n"
//    "a=rtcp-fb:120 ccm vbcm 123 456 789\r\n"
//    "a=rtcp-fb:120 ccm foo\r\n"
//    "a=rtcp-fb:120 ccm foo bar baz\r\n"
//    "a=rtcp-fb:120 foo\r\n"
//    "a=rtcp-fb:120 foo bar\r\n"
//    "a=rtcp-fb:120 foo bar baz\r\n");
//
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 1), SDP_RTCP_FB_ACK_RPSI);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 2), SDP_RTCP_FB_ACK_APP);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 3), SDP_RTCP_FB_ACK_APP);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 4),
//            SDP_RTCP_FB_ACK_UNKNOWN);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 5),
//            SDP_RTCP_FB_ACK_UNKNOWN);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, 120, 6),
//            SDP_RTCP_FB_ACK_NOT_FOUND);
//
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 1),
//            SDP_RTCP_FB_NACK_BASIC);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 2),
//            SDP_RTCP_FB_NACK_PLI);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 3),
//            SDP_RTCP_FB_NACK_SLI);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 4),
//            SDP_RTCP_FB_NACK_RPSI);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 5),
//            SDP_RTCP_FB_NACK_APP);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 6),
//            SDP_RTCP_FB_NACK_APP);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 7),
//            SDP_RTCP_FB_NACK_APP);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 8),
//            SDP_RTCP_FB_NACK_UNKNOWN);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_nack(sdp_ptr_, 1, 120, 9),
//            SDP_RTCP_FB_NACK_NOT_FOUND);
//
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_trr_int(sdp_ptr_, 1, 120, 1), 0U);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_trr_int(sdp_ptr_, 1, 120, 2), 123U);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_trr_int(sdp_ptr_, 1, 120, 3), 0xFFFFFFFF);
//
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 1), SDP_RTCP_FB_CCM_FIR);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 2),
//            SDP_RTCP_FB_CCM_TMMBR);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 3),
//            SDP_RTCP_FB_CCM_TMMBR);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 4),
//            SDP_RTCP_FB_CCM_TSTR);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 5),
//            SDP_RTCP_FB_CCM_VBCM);
//  // We don't currently parse out VBCM submessage types, since we don't have
//  // any use for them.
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 6),
//            SDP_RTCP_FB_CCM_UNKNOWN);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 7),
//            SDP_RTCP_FB_CCM_UNKNOWN);
//  ASSERT_EQ(sdp_attr_get_rtcp_fb_ccm(sdp_ptr_, 1, 120, 8),
//            SDP_RTCP_FB_CCM_NOT_FOUND);
//}
//
//TEST_F(SdpTest, addRtcpFbAckRpsi) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbAck(level, SDP_RTCP_FB_ACK_RPSI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ack rpsi\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbAckRpsiAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbAck(level, SDP_RTCP_FB_ACK_RPSI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ack rpsi\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbAckApp) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbAck(level, SDP_RTCP_FB_ACK_APP, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ack app\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbAckAppAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbAck(level, SDP_RTCP_FB_ACK_APP);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ack app\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNack) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_BASIC, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_BASIC);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackSli) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_SLI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack sli\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackSliAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_SLI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack sli\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackPli) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_PLI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack pli\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackPliAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_PLI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack pli\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackRpsi) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_RPSI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack rpsi\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackRpsiAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_RPSI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack rpsi\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackApp) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_APP, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack app\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackAppAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_APP);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack app\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackRai) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_RAI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack rai\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackRaiAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_RAI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack rai\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackTllei) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_TLLEI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack tllei\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackTlleiAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_TLLEI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack tllei\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackPslei) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_PSLEI, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack pslei\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackPsleiAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_PSLEI);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack pslei\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackEcn) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_ECN, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 nack ecn\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackEcnAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbNack(level, SDP_RTCP_FB_NACK_ECN);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* nack ecn\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbTrrInt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbTrrInt(level, 12345, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 trr-int 12345\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbNackTrrIntAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbTrrInt(level, 0);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* trr-int 0\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmFir) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_FIR, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ccm fir\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmFirAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_FIR);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ccm fir\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmTmmbr) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_TMMBR, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ccm tmmbr\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmTmmbrAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_TMMBR);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ccm tmmbr\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmTstr) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_TSTR, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ccm tstr\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmTstrAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_TSTR);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ccm tstr\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmVbcm) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_VBCM, 120);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:120 ccm vbcm\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addRtcpFbCcmVbcmAllPt) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewRtcpFbCcm(level, SDP_RTCP_FB_CCM_VBCM);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=rtcp-fb:* ccm vbcm\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, parseRtcpFbAllPayloads) {
//  ParseSdp(kVideoSdp + "a=rtcp-fb:* ack rpsi\r\n");
//  for (int i = 0; i < 128; i++) {
//    ASSERT_EQ(sdp_attr_get_rtcp_fb_ack(sdp_ptr_, 1, i, 1),
//              SDP_RTCP_FB_ACK_RPSI);
//  }
//}
//TEST_F(SdpTest, addExtMap) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewExtMap(level, SDP_EXTMAP_AUDIO_LEVEL);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, parseExtMap) {
//  ParseSdp(kVideoSdp +
//    "a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\n");
//  ASSERT_STREQ(sdp_attr_get_extmap_uri(sdp_ptr_, 1, 1),
//            SDP_EXTMAP_AUDIO_LEVEL);
//  ASSERT_EQ(sdp_attr_get_extmap_id(sdp_ptr_, 1, 1),
//            1);
//
//}
//
//TEST_F(SdpTest, parseFmtpMaxFs) {
//  u32 val = 0;
//  ParseSdp(kVideoSdp + "a=fmtp:120 max-fs=300;max-fr=30\r\n");
//  ASSERT_EQ(sdp_attr_get_fmtp_max_fs(sdp_ptr_, 1, 0, 1, &val), SDP_SUCCESS);
//  ASSERT_EQ(val, 300);
//}
//TEST_F(SdpTest, parseFmtpMaxFr) {
//  u32 val = 0;
//  ParseSdp(kVideoSdp + "a=fmtp:120 max-fs=300;max-fr=30\r\n");
//  ASSERT_EQ(sdp_attr_get_fmtp_max_fr(sdp_ptr_, 1, 0, 1, &val), SDP_SUCCESS);
//  ASSERT_EQ(val, 30);
//}
//
//TEST_F(SdpTest, addFmtpMaxFs) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewFmtpMaxFs(level, 300);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=fmtp:120 max-fs=300\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addFmtpMaxFr) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewFmtpMaxFr(level, 30);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=fmtp:120 max-fr=30\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, addFmtpMaxFsFr) {
//  InitLocalSdp();
//  int level = AddNewMedia(SDP_MEDIA_VIDEO);
//  AddNewFmtpMaxFsFr(level, 300, 30);
//  std::string body = SerializeSdp();
//  ASSERT_NE(body.find("a=fmtp:120 max-fs=300;max-fr=30\r\n"),
//            std::string::npos);
//}
//
//TEST_F(SdpTest, addIceLite) {
//    InitLocalSdp();
//    u16 inst_num = 0;
//    EXPECT_EQ(sdp_add_new_attr(sdp_ptr_, SDP_SESSION_LEVEL, 0,
//                               SDP_ATTR_ICE_LITE, &inst_num), SDP_SUCCESS);
//    std::string body = SerializeSdp();
//    ASSERT_NE(body.find("a=ice-lite\r\n"), std::string::npos);
//}
//
//TEST_F(SdpTest, parseIceLite) {
//    std::string sdp =
//        "v=0\r\n"
//        "o=- 137331303 2 IN IP4 127.0.0.1\r\n"
//        "s=SIP Call\r\n"
//        "t=0 0\r\n"
//        "a=ice-lite\r\n";
//  ParseSdp(sdp);
//  ASSERT_TRUE(sdp_attr_is_present(sdp_ptr_, SDP_ATTR_ICE_LITE,
//                                  SDP_SESSION_LEVEL, 0));
//}

class NewSdpTest : public ::testing::Test {
  public:
    NewSdpTest() {}

    void ParseSdp(const std::string &sdp, bool expectSuccess = true) {
      mSdp = mozilla::Move(mParser.Parse(sdp));

      if (expectSuccess) {
        ASSERT_TRUE(mSdp) << "Parse failed: " << GetParseErrors();
      }
    }

    // For streaming parse errors
    std::string GetParseErrors() {
      std::stringstream output;
      for (auto e = mParser.GetParseErrors().begin();
           e != mParser.GetParseErrors().end();
           ++e) {
        output << e->first << ": " << e->second << std::endl;
      }
      return output.str();
    }

  void CheckRtpmap(const std::string&pt, const std::string&name,
                   uint32_t clock, uint16_t channels,
                   const SdpRtpmapAttributeList::Rtpmap& attr) {
    ASSERT_EQ(pt, attr.pt);
    ASSERT_EQ(name, attr.name);
    ASSERT_EQ(clock, attr.clock);
    ASSERT_EQ(channels, attr.channels);
  }

    SipccSdpParser mParser;
    mozilla::UniquePtr<Sdp> mSdp;
};

TEST_F(NewSdpTest, CreateDestroy) {
}

TEST_F(NewSdpTest, ParseEmpty) {
  ParseSdp("", false);
  ASSERT_FALSE(mSdp);
  ASSERT_NE(0U, mParser.GetParseErrors().size())
    << "Expected at least one parse error.";
}

const std::string kBadSdp = "This is SPARTA!!!!";

TEST_F(NewSdpTest, ParseGarbage) {
  ParseSdp(kBadSdp, false);
  ASSERT_FALSE(mSdp);
  ASSERT_NE(0U, mParser.GetParseErrors().size())
    << "Expected at least one parse error.";
}

TEST_F(NewSdpTest, ParseGarbageTwice) {
  ParseSdp(kBadSdp, false);
  ASSERT_FALSE(mSdp);
  size_t errorCount = mParser.GetParseErrors().size();
  ASSERT_NE(0U, errorCount)
    << "Expected at least one parse error.";
  ParseSdp(kBadSdp, false);
  ASSERT_FALSE(mSdp);
  ASSERT_EQ(errorCount, mParser.GetParseErrors().size())
    << "Expected same error count for same SDP.";
}

static const std::string kVideoSdp =
  "v=0\r\n"
  "o=- 137331303 2 IN IP4 127.0.0.1\r\n"
  "s=SIP Call\r\n"
  "c=IN IP4 198.51.100.7\r\n"
  "t=0 0\r\n"
  "m=video 56436 RTP/SAVPF 120\r\n"
  "a=rtpmap:120 VP8/90000\r\n";

TEST_F(NewSdpTest, ParseMinimal) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(0U, mParser.GetParseErrors().size()) <<
    "Got parse errors: " << GetParseErrors();
}

TEST_F(NewSdpTest, CheckOriginGetUsername) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ("-", mSdp->GetOrigin().GetUsername())
    << "Wrong username in origin";
}

TEST_F(NewSdpTest, CheckOriginGetSessionId) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(137331303 , mSdp->GetOrigin().GetSessionId())
    << "Wrong session id in origin";
}

TEST_F(NewSdpTest, CheckOriginGetSessionVersion) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(2 , mSdp->GetOrigin().GetSessionVersion())
    << "Wrong version in origin";
}

TEST_F(NewSdpTest, CheckOriginGetAddrType) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(sdp::kIPv4, mSdp->GetOrigin().GetAddrType())
    << "Wrong address type in origin";
}

TEST_F(NewSdpTest, CheckOriginGetAddress) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ("127.0.0.1" , mSdp->GetOrigin().GetAddress())
    << "Wrong address in origin";
}

TEST_F(NewSdpTest, CheckGetMissingBandwidth) {
  ParseSdp(kVideoSdp);
  ASSERT_TRUE((mSdp->GetBandwidth("CT")).empty())
    << "Wrong bandwidth in session";
}

TEST_F(NewSdpTest, CheckGetBandwidth) {
  ParseSdp("v=0\r\n"
           "o=- 137331303 2 IN IP4 127.0.0.1\r\n"
           "b=CT:5000\r\n"
           "s=SIP Call\r\n"
           "c=IN IP4 198.51.100.7\r\n"
           "t=0 0\r\n"
           "m=video 56436 RTP/SAVPF 120\r\n");
  ASSERT_EQ("5000", mSdp->GetBandwidth("CT"))
    << "Wrong bandwidth in session";
}

TEST_F(NewSdpTest, CheckGetMediaSectionsCount) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(1U, mSdp->GetMediaSectionCount())
    << "Wrong number of media sections";
}

TEST_F(NewSdpTest, CheckMediaSectionGetMediaType) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(SdpMediaSection::kVideo, mSdp->GetMediaSection(0).GetMediaType())
    << "Wrong type for first media section";
}

TEST_F(NewSdpTest, CheckMediaSectionGetProtocol) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(SdpMediaSection::kRtpSavpf, mSdp->GetMediaSection(0).GetProtocol())
    << "Wrong protocol for video";
}

TEST_F(NewSdpTest, CheckMediaSectionGetFormats) {
  ParseSdp(kVideoSdp);
  auto video_formats = mSdp->GetMediaSection(0).GetFormats();
  ASSERT_EQ(1U, video_formats.size()) << "Wrong number of formats for video";
  ASSERT_EQ("120", video_formats[0]);
}

TEST_F(NewSdpTest, CheckMediaSectionGetPort) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(56436U, mSdp->GetMediaSection(0).GetPort())
    << "Wrong port number in media section";
}

TEST_F(NewSdpTest, CheckMediaSectionGetMissingPortCount) {
  ParseSdp(kVideoSdp);
  ASSERT_EQ(0U, mSdp->GetMediaSection(0).GetPortCount())
    << "Wrong port count in media section";
}

TEST_F(NewSdpTest, CheckMediaSectionGetPortCount) {
  ParseSdp(kVideoSdp + "m=audio 12345/2 RTP/SAVPF 0\r\n");
  ASSERT_EQ(2U, mSdp->GetMediaSectionCount())
    << "Wrong number of media sections";
  ASSERT_EQ(2U, mSdp->GetMediaSection(1).GetPortCount())
    << "Wrong port count in media section";
}

TEST_F(NewSdpTest, CheckMediaSectionGetMissingBandwidth) {
  ParseSdp(kVideoSdp);
  ASSERT_TRUE((mSdp->GetMediaSection(0).GetBandwidth("CT")).empty())
    << "Wrong bandwidth in media section";
}

TEST_F(NewSdpTest, CheckMediaSectionGetBandwidth) {
  ParseSdp("v=0\r\n"
           "o=- 137331303 2 IN IP4 127.0.0.1\r\n"
           "c=IN IP4 198.51.100.7\r\n"
           "t=0 0\r\n"
           "m=video 56436 RTP/SAVPF 120\r\n"
           "b=CT:1000\r\n"
           "a=rtpmap:120 VP8/90000\r\n");
  ASSERT_EQ("1000", mSdp->GetMediaSection(0).GetBandwidth("CT"))
    << "Wrong bandwidth in media section";
}


// SDP from a basic A/V apprtc call FFX/FFX
const std::string kBasicAudioVideoOffer =
"v=0" CRLF
"o=Mozilla-SIPUA-35.0a1 5184 0 IN IP4 0.0.0.0" CRLF
"s=SIP Call" CRLF
"c=IN IP4 224.0.0.1/100/12" CRLF
"t=0 0" CRLF
"a=ice-ufrag:4a799b2e" CRLF
"a=ice-pwd:e4cc12a910f106a0a744719425510e17" CRLF
"a=ice-lite" CRLF
"a=fingerprint:sha-256 DF:2E:AC:8A:FD:0A:8E:99:BF:5D:E8:3C:E7:FA:FB:08:3B:3C:54:1D:D7:D4:05:77:A0:72:9B:14:08:6D:0F:4C" CRLF
"m=audio 9 RTP/SAVPF 109 9 0 8 101" CRLF
"c=IN IP4 0.0.0.0" CRLF
"a=rtpmap:109 opus/48000/2" CRLF
"a=ptime:20" CRLF
"a=maxptime:20" CRLF
"a=rtpmap:9 G722/8000" CRLF
"a=rtpmap:0 PCMU/8000" CRLF
"a=rtpmap:8 PCMA/8000" CRLF
"a=rtpmap:101 telephone-event/8000" CRLF
"a=fmtp:101 0-15" CRLF
"a=sendonly" CRLF
"a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level" CRLF
"a=setup:actpass" CRLF
"a=rtcp-mux" CRLF
"a=candidate:0 1 UDP 2130379007 10.0.0.36 62453 typ host" CRLF
"a=candidate:2 1 UDP 1694236671 24.6.134.204 62453 typ srflx raddr 10.0.0.36 rport 62453" CRLF
"a=candidate:3 1 UDP 100401151 162.222.183.171 49761 typ relay raddr 162.222.183.171 rport 49761" CRLF
"a=candidate:6 1 UDP 16515071 162.222.183.171 51858 typ relay raddr 162.222.183.171 rport 51858" CRLF
"a=candidate:3 2 UDP 100401150 162.222.183.171 62454 typ relay raddr 162.222.183.171 rport 62454" CRLF
"a=candidate:2 2 UDP 1694236670 24.6.134.204 55428 typ srflx raddr 10.0.0.36 rport 55428" CRLF
"a=candidate:6 2 UDP 16515070 162.222.183.171 50340 typ relay raddr 162.222.183.171 rport 50340" CRLF
"a=candidate:0 2 UDP 2130379006 10.0.0.36 55428 typ host" CRLF
"m=video 9 RTP/SAVPF 120" CRLF
"c=IN IP6 ::1" CRLF
"a=rtpmap:120 VP8/90000" CRLF
"a=recvonly" CRLF
"a=rtcp-fb:120 nack" CRLF
"a=rtcp-fb:120 nack pli" CRLF
"a=rtcp-fb:120 ccm fir" CRLF
"a=setup:active" CRLF
"a=rtcp-mux" CRLF
"a=candidate:0 1 UDP 2130379007 10.0.0.36 59530 typ host" CRLF
"a=candidate:0 2 UDP 2130379006 10.0.0.36 64378 typ host" CRLF
"a=candidate:2 2 UDP 1694236670 24.6.134.204 64378 typ srflx raddr 10.0.0.36 rport 64378" CRLF
"a=candidate:6 2 UDP 16515070 162.222.183.171 64941 typ relay raddr 162.222.183.171 rport 64941" CRLF
"a=candidate:6 1 UDP 16515071 162.222.183.171 64800 typ relay raddr 162.222.183.171 rport 64800" CRLF
"a=candidate:2 1 UDP 1694236671 24.6.134.204 59530 typ srflx raddr 10.0.0.36 rport 59530" CRLF
"a=candidate:3 1 UDP 100401151 162.222.183.171 62935 typ relay raddr 162.222.183.171 rport 62935" CRLF
"a=candidate:3 2 UDP 100401150 162.222.183.171 61026 typ relay raddr 162.222.183.171 rport 61026" CRLF
"m=audio 9 RTP/SAVPF 0" CRLF
"a=rtpmap:0 PCMU/8000" CRLF
"a=ice-lite" CRLF;


TEST_F(NewSdpTest, BasicAudioVideoSdpParse) {
  ParseSdp(kBasicAudioVideoOffer);
}

TEST_F(NewSdpTest, CheckIceUfrag) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_TRUE(mSdp->GetAttributeList().HasAttribute(
        SdpAttribute::kIceUfragAttribute));
  auto ice_ufrag = mSdp->GetAttributeList().GetIceUfrag();
  ASSERT_EQ("4a799b2e", ice_ufrag) << "Wrong ice-ufrag value";
}

TEST_F(NewSdpTest, CheckIcePwd) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_TRUE(mSdp->GetAttributeList().HasAttribute(
        SdpAttribute::kIcePwdAttribute));
  auto ice_pwd = mSdp->GetAttributeList().GetIcePwd();
  ASSERT_EQ("e4cc12a910f106a0a744719425510e17", ice_pwd) << "Wrong ice-pwd value";
}

TEST_F(NewSdpTest, CheckFingerprint) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_TRUE(mSdp->GetAttributeList().HasAttribute(
        SdpAttribute::kFingerprintAttribute));
  auto fingerprints = mSdp->GetAttributeList().GetFingerprint();
  ASSERT_EQ(1U, fingerprints.mFingerprints.size());
  ASSERT_EQ(SdpFingerprintAttributeList::kSha256,
      fingerprints.mFingerprints.front().hashFunc)
    << "Wrong hash function";
  ASSERT_EQ("DF:2E:AC:8A:FD:0A:8E:99:BF:5D:E8:3C:E7:FA:FB:08:"
            "3B:3C:54:1D:D7:D4:05:77:A0:72:9B:14:08:6D:0F:4C",
            fingerprints.mFingerprints.front().fingerprint)
    << "Wrong fingerprint";
}

TEST_F(NewSdpTest, CheckNumberOfMediaSections) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount()) << "Wrong number of media sections";
}

TEST_F(NewSdpTest, CheckMlines) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount()) << "Wrong number of media sections";
  ASSERT_EQ(SdpMediaSection::kAudio, mSdp->GetMediaSection(0).GetMediaType())
    << "Wrong type for first media section";
  ASSERT_EQ(SdpMediaSection::kRtpSavpf,
            mSdp->GetMediaSection(0).GetProtocol())
    << "Wrong protocol for audio";
  auto audio_formats = mSdp->GetMediaSection(0).GetFormats();
  ASSERT_EQ(5U, audio_formats.size()) << "Wrong number of formats for audio";
  ASSERT_EQ("109", audio_formats[0]);
  ASSERT_EQ("9",   audio_formats[1]);
  ASSERT_EQ("0",   audio_formats[2]);
  ASSERT_EQ("8",   audio_formats[3]);
  ASSERT_EQ("101", audio_formats[4]);

  ASSERT_EQ(SdpMediaSection::kVideo, mSdp->GetMediaSection(1).GetMediaType())
    << "Wrong type for second media section";
  ASSERT_EQ(SdpMediaSection::kRtpSavpf,
            mSdp->GetMediaSection(1).GetProtocol())
    << "Wrong protocol for video";
  auto video_formats = mSdp->GetMediaSection(1).GetFormats();
  ASSERT_EQ(1U, video_formats.size()) << "Wrong number of formats for video";
  ASSERT_EQ("120", video_formats[0]);
}

TEST_F(NewSdpTest, CheckSetup) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_TRUE(mSdp) << "Parse failed: " << GetParseErrors();
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount()) << "Wrong number of media sections";

  ASSERT_TRUE(mSdp->GetMediaSection(0).GetAttributeList().HasAttribute(
      SdpAttribute::kSetupAttribute));
  ASSERT_EQ(SdpSetupAttribute::kActpass,
      mSdp->GetMediaSection(0).GetAttributeList().GetSetup().mRole);
  ASSERT_TRUE(mSdp->GetMediaSection(1).GetAttributeList().HasAttribute(
      SdpAttribute::kSetupAttribute));
  ASSERT_EQ(SdpSetupAttribute::kActive,
      mSdp->GetMediaSection(1).GetAttributeList().GetSetup().mRole);
  ASSERT_FALSE(mSdp->GetMediaSection(2).GetAttributeList().HasAttribute(
        SdpAttribute::kSetupAttribute));
}

TEST_F(NewSdpTest, CheckRtpmap) {
  ParseSdp(kBasicAudioVideoOffer);

  const SdpMediaSection& audiosec = mSdp->GetMediaSection(0);
  const SdpRtpmapAttributeList& rtpmap = audiosec.GetAttributeList().GetRtpmap();
  ASSERT_EQ(5U, rtpmap.mRtpmaps.size())
    << "Wrong number of rtpmap attributes for audio";

  // TODO: Write a CheckRtpmap(rtpmap, payloadType, encodingName, rate)
  // Need to know name of type
  CheckRtpmap("109", "opus",           48000, 2, rtpmap.GetEntry(audiosec.GetFormats()[0]));
  CheckRtpmap("9",   "G722",            8000, 1, rtpmap.GetEntry(audiosec.GetFormats()[1]));
  CheckRtpmap("0",   "PCMU",            8000, 1, rtpmap.GetEntry(audiosec.GetFormats()[2]));
  CheckRtpmap("8",   "PCMA",            8000, 1, rtpmap.GetEntry(audiosec.GetFormats()[3]));
  CheckRtpmap("101", "telephone-event", 8000, 1, rtpmap.GetEntry(audiosec.GetFormats()[4]));

  const SdpMediaSection& videosec = mSdp->GetMediaSection(1);
  CheckRtpmap("120", "VP8",            90000, 1,
              videosec.GetAttributeList().GetRtpmap().GetEntry(
                  videosec.GetFormats()[0]));
}

TEST_F(NewSdpTest, CheckFormatParameters) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount())
    << "Wrong number of media sections";

//  ASSERT_EQ(1U, mSdp->GetMediaSection(0).GetAttributeList().Count(kFmtp));
}

TEST_F(NewSdpTest, CheckPtime) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(20U, mSdp->GetMediaSection(0).GetAttributeList().GetPtime());
  ASSERT_FALSE(mSdp->GetMediaSection(1).GetAttributeList().HasAttribute(
      SdpAttribute::kPtimeAttribute));
}

TEST_F(NewSdpTest, CheckFlags) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_FALSE(mSdp->GetMediaSection(0).GetAttributeList().HasAttribute(
      SdpAttribute::kIceLiteAttribute, false));
  ASSERT_TRUE(mSdp->GetMediaSection(0).GetAttributeList().HasAttribute(
      SdpAttribute::kIceLiteAttribute));
  ASSERT_TRUE(mSdp->GetMediaSection(2).GetAttributeList().HasAttribute(
      SdpAttribute::kIceLiteAttribute));

  ASSERT_TRUE(mSdp->GetMediaSection(0).GetAttributeList().HasAttribute(
      SdpAttribute::kRtcpMuxAttribute));
  ASSERT_FALSE(mSdp->GetMediaSection(2).GetAttributeList().HasAttribute(
      SdpAttribute::kRtcpMuxAttribute));
}

TEST_F(NewSdpTest, CheckConnectionLines) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount())
    << "Wrong number of media sections";

  const SdpConnection& conn1 = mSdp->GetMediaSection(0).GetConnection();
  ASSERT_EQ(sdp::kIPv4, conn1.GetAddrType());
  ASSERT_EQ("0.0.0.0", conn1.GetAddress());
  ASSERT_EQ(0U, conn1.GetTtl());
  ASSERT_EQ(0U, conn1.GetCount());

  const SdpConnection& conn2 = mSdp->GetMediaSection(1).GetConnection();
  ASSERT_EQ(sdp::kIPv6, conn2.GetAddrType());
  ASSERT_EQ("::1", conn2.GetAddress());
  ASSERT_EQ(0U, conn2.GetTtl());
  ASSERT_EQ(0U, conn2.GetCount());

  // tests that we can fall through to session level as appropriate
  const SdpConnection& conn3 = mSdp->GetMediaSection(2).GetConnection();
  ASSERT_EQ(sdp::kIPv4, conn3.GetAddrType());
  ASSERT_EQ("224.0.0.1", conn3.GetAddress());
  ASSERT_EQ(100U, conn3.GetTtl());
  ASSERT_EQ(12U, conn3.GetCount());
}

TEST_F(NewSdpTest, CheckDirections) {
  ParseSdp(kBasicAudioVideoOffer);

  ASSERT_EQ(SdpDirectionAttribute::kSendonly,
            mSdp->GetMediaSection(0).GetAttributeList().GetDirection());
  ASSERT_EQ(SdpDirectionAttribute::kRecvonly,
            mSdp->GetMediaSection(1).GetAttributeList().GetDirection());
  ASSERT_EQ(SdpDirectionAttribute::kSendrecv,
            mSdp->GetMediaSection(2).GetAttributeList().GetDirection());
}

TEST_F(NewSdpTest, CheckCandidates) {
  ParseSdp(kBasicAudioVideoOffer);
  ASSERT_EQ(3U, mSdp->GetMediaSectionCount()) << "Wrong number of media sections";

  ASSERT_TRUE(mSdp->GetMediaSection(0).GetAttributeList().HasAttribute(
      SdpAttribute::kCandidateAttribute));
  auto audio_candidates =
      mSdp->GetMediaSection(0).GetAttributeList().GetCandidate();
  ASSERT_EQ(8U, audio_candidates.size());
  ASSERT_EQ("0 1 UDP 2130379007 10.0.0.36 62453 typ host", audio_candidates[0]);
  ASSERT_EQ("2 1 UDP 1694236671 24.6.134.204 62453 typ srflx raddr 10.0.0.36 rport 62453", audio_candidates[1]);
  ASSERT_EQ("3 1 UDP 100401151 162.222.183.171 49761 typ relay raddr 162.222.183.171 rport 49761", audio_candidates[2]);
  ASSERT_EQ("6 1 UDP 16515071 162.222.183.171 51858 typ relay raddr 162.222.183.171 rport 51858", audio_candidates[3]);
  ASSERT_EQ("3 2 UDP 100401150 162.222.183.171 62454 typ relay raddr 162.222.183.171 rport 62454", audio_candidates[4]);
  ASSERT_EQ("2 2 UDP 1694236670 24.6.134.204 55428 typ srflx raddr 10.0.0.36 rport 55428", audio_candidates[5]);
  ASSERT_EQ("6 2 UDP 16515070 162.222.183.171 50340 typ relay raddr 162.222.183.171 rport 50340", audio_candidates[6]);
  ASSERT_EQ("0 2 UDP 2130379006 10.0.0.36 55428 typ host", audio_candidates[7]);

  ASSERT_TRUE(mSdp->GetMediaSection(1).GetAttributeList().HasAttribute(
      SdpAttribute::kCandidateAttribute));
  auto video_candidates =
      mSdp->GetMediaSection(1).GetAttributeList().GetCandidate();
  ASSERT_EQ(8U, video_candidates.size());
  ASSERT_EQ("0 1 UDP 2130379007 10.0.0.36 59530 typ host", video_candidates[0]);
  ASSERT_EQ("0 2 UDP 2130379006 10.0.0.36 64378 typ host", video_candidates[1]);
  ASSERT_EQ("2 2 UDP 1694236670 24.6.134.204 64378 typ srflx raddr 10.0.0.36 rport 64378", video_candidates[2]);
  ASSERT_EQ("6 2 UDP 16515070 162.222.183.171 64941 typ relay raddr 162.222.183.171 rport 64941", video_candidates[3]);
  ASSERT_EQ("6 1 UDP 16515071 162.222.183.171 64800 typ relay raddr 162.222.183.171 rport 64800", video_candidates[4]);
  ASSERT_EQ("2 1 UDP 1694236671 24.6.134.204 59530 typ srflx raddr 10.0.0.36 rport 59530", video_candidates[5]);
  ASSERT_EQ("3 1 UDP 100401151 162.222.183.171 62935 typ relay raddr 162.222.183.171 rport 62935", video_candidates[6]);
  ASSERT_EQ("3 2 UDP 100401150 162.222.183.171 61026 typ relay raddr 162.222.183.171 rport 61026", video_candidates[7]);

  ASSERT_FALSE(mSdp->GetMediaSection(2).GetAttributeList().HasAttribute(
      SdpAttribute::kCandidateAttribute));
}

// TODO: Tests that parse above SDP, and check various things
// For media sections 1 and 2:
//  Check fmtp
//  Check extmap
//  Check setup
//  Check rtcp-mux
//  Check candidates

} // End namespace test.

int main(int argc, char **argv) {
  test_utils = new MtransportTestUtils();
  NSS_NoDB_Init(nullptr);
  NSS_SetDomesticPolicy();

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  sipcc::PeerConnectionCtx::Destroy();
  delete test_utils;

  return result;
}
