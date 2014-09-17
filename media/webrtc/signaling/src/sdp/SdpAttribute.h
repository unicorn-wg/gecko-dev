/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_ATTRIBUTE_H_
#define _SDP_ATTRIBUTE_H_

#include "mozilla/UniquePtr.h"

class SdpAttribute
{
  public:
    Type GetType () const;
    std::string GetTypeName() const;

    enum Type {
      kBundleOnly,
      kCandidate,
      kConnection,
      kDtlsFingerprint,
      kExtmap,
      kFingerprint,
      kFmtp,
      kGroup,
      kIceCandidate,
      kIceLite,
      kIceOptions,
      kIcePasswd,
      kIceUfrag,
      kIdentity,
      kImageattr,
      kInactive,
      kLabel,
      kMaxprate,
      kMaxptime,
      kMid,
      kMsid,
      kPtime,
      kRecvonly,
      kRtcp,
      kRtcpFb,
      kRtcpMux,
      kRtcpRsize,
      kRtpmap,
      kSctpmap,
      kSendonly,
      kSendrecv,
      kSetup,
      kSsrc,
      kSsrcGroup,
      kOther
    };
  protected:
    virtual ~SdpAttribute();
};

class SdpAttributeList
{
  public:
    bool HasAttribute(SdpAttributeType type,
                      unsigned int instance = 0);

    UniquePtr<SdpAttribute> GetAttibute(SdpAttributeType type,
                                        unsigned int instance = 0);
}

#endif
