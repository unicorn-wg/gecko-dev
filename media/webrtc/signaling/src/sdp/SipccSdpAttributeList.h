/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPATTRIBUTELIST_H_
#define _SIPCCSDPATTRIBUTELIST_H_

#include "signaling/src/sdp/SdpAttributeList.h"
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdpMediaSection;

class SipccSdpAttributeList : public SdpAttributeList
{
  friend class SipccSdpMediaSection;
public:
  virtual unsigned int CountAttributes(sdp::AttributeType type) const MOZ_OVERRIDE;
  virtual bool HasAttribute(sdp::AttributeType type) const MOZ_OVERRIDE;

  virtual UniquePtr<SdpAttribute>
    GetAttribute(sdp::AttributeType type) const MOZ_OVERRIDE;

  virtual UniquePtr<SdpCandidateAttribute> GetCandidate() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpConnectionAttribute> GetConnection() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpExtmapAttribute> GetExtmap() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpFingerprintAttribute> GetFingerprint() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpFmtpAttribute> GetFmtp() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpGroupAttribute> GetGroup() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpIceOptionsAttribute> GetIceOptions() const MOZ_OVERRIDE;
  virtual std::string GetIcePwd() const MOZ_OVERRIDE;
  virtual std::string GetIceUfrag() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpIdentityAttribute> GetIdentity() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpImageattrAttribute> GetImageattr() const MOZ_OVERRIDE;
  virtual std::string GetLabel() const MOZ_OVERRIDE;
  virtual unsigned int GetMaxprate() const MOZ_OVERRIDE;
  virtual unsigned int GetMaxptime() const MOZ_OVERRIDE;
  virtual std::string GetMid() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpMsidAttribute> GetMsid() const MOZ_OVERRIDE;
  virtual unsigned int GetPtime() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpRtcpAttribute> GetRtcp() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpRtcpFbAttribute> GetRtcpFb() const MOZ_OVERRIDE;
//  virtual UniquePtr<SdpRtcpRemoteCandidates> GetRemoteCandidates() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpRtpmapAttribute> GetRtpmap() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpSctpmapAttribute> GetSctpmap() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpSetupAttribute> GetSetup() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpSsrcAttribute> GetSsrc() const MOZ_OVERRIDE;
  virtual UniquePtr<SdpSsrcGroupAttribute> GetSsrcGroup() const MOZ_OVERRIDE;

  virtual void SetAttribute(const SdpAttribute &) MOZ_OVERRIDE;
private:
  SipccSdpAttributeList(sdp_t* sdp, uint16_t level)
  : mSdp(sdp), mLevel(level) {}

  sdp_t* mSdp;
  uint16_t mLevel;
};

}

#endif
