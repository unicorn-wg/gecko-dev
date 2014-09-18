/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPATTRIBUTELIST_H_
#define _SDPATTRIBUTELIST_H_

#include "mozilla/UniquePtr.h"
#include "mozilla/Attributes.h"

#include "signaling/src/sdp/SdpEnum.h"
#include "signaling/src/sdp/SdpAttribute.h"

namespace mozilla {

class SdpAttributeList
{
public:
  virtual unsigned int CountAttributes(sdp::AttributeType type) const = 0;
  virtual bool HasAttribute(sdp::AttributeType type) const = 0;

  virtual UniquePtr<SdpAttribute>
    GetAttribute(sdp::AttributeType type) const = 0;

  virtual UniquePtr<SdpCandidateAttribute> GetCandidate() const = 0;
  virtual UniquePtr<SdpConnectionAttribute> GetConnection() const = 0;
  virtual UniquePtr<SdpExtmapAttribute> GetExtmap() const = 0;
  virtual UniquePtr<SdpFingerprintAttribute> GetFingerprint() const = 0;
  virtual UniquePtr<SdpFmtpAttribute> GetFmtp() const = 0;
  virtual UniquePtr<SdpGroupAttribute> GetGroup() const = 0;
  virtual UniquePtr<SdpIceOptionsAttribute> GetIceOptions() const = 0;
  virtual std::string GetIcePwd() const = 0;
  virtual std::string GetIceUfrag() const = 0;
  virtual UniquePtr<SdpIdentityAttribute> GetIdentity() const = 0;
  virtual UniquePtr<SdpImageattrAttribute> GetImageattr() const = 0;
  virtual std::string GetLabel() const = 0;
  virtual unsigned int GetMaxprate() const = 0;
  virtual unsigned int GetMaxptime() const = 0;
  virtual std::string GetMid() const = 0;
  virtual UniquePtr<SdpMsidAttribute> GetMsid() const = 0;
  virtual unsigned int GetPtime() const = 0;
  virtual UniquePtr<SdpRtcpAttribute> GetRtcp() const = 0;
  virtual UniquePtr<SdpRtcpFbAttribute> GetRtcpFb() const = 0;
  // virtual UniquePtr<SdpRtcpRemoteCandidates> GetRemoteCandidates() const = 0;
  virtual UniquePtr<SdpRtpmapAttribute> GetRtpmap() const = 0;
  virtual UniquePtr<SdpSctpmapAttribute> GetSctpmap() const = 0;
  virtual UniquePtr<SdpSetupAttribute> GetSetup() const = 0;
  virtual UniquePtr<SdpSsrcAttribute> GetSsrc() const = 0;
  virtual UniquePtr<SdpSsrcGroupAttribute> GetSsrcGroup() const = 0;

  virtual void SetAttribute(const SdpAttribute &) = 0;
};

}

#endif
