/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPATTRIBUTELIST_H_
#define _SIPCCSDPATTRIBUTELIST_H_

#include "signaling/src/sdp/SdpAttribute.h"
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

class SipccSdp;
class SipccSdpMediaSection;

class SipccSdpAttributeList : public SdpAttributeList
{
  friend class SipccSdpMediaSection;
  friend class SipccSdp;
public:
  virtual size_t CountAttributes(AttributeType type) const MOZ_OVERRIDE;
  virtual bool HasAttribute(AttributeType type) const MOZ_OVERRIDE;
  virtual SdpAttribute GetAttribute(AttributeType type, size_t index = 0) const MOZ_OVERRIDE;

  virtual const SdpCandidateAttribute& GetCandidate() const MOZ_OVERRIDE;
  virtual const SdpConnectionAttribute& GetConnection() const MOZ_OVERRIDE;
  virtual const SdpExtmapAttribute& GetExtmap() const MOZ_OVERRIDE;
  virtual const SdpFingerprintAttribute& GetFingerprint() const MOZ_OVERRIDE;
  virtual const SdpFmtpAttribute& GetFmtp() const MOZ_OVERRIDE;
  virtual const SdpGroupAttribute& GetGroup() const MOZ_OVERRIDE;
  virtual const SdpIceOptionsAttribute& GetIceOptions() const MOZ_OVERRIDE;
  virtual const std::string& GetIcePwd() const MOZ_OVERRIDE;
  virtual const std::string& GetIceUfrag() const MOZ_OVERRIDE;
  virtual const SdpIdentityAttribute& GetIdentity() const MOZ_OVERRIDE;
  virtual const SdpImageattrAttribute& GetImageattr() const MOZ_OVERRIDE;
  virtual const std::string& GetLabel() const MOZ_OVERRIDE;
  virtual uint32_t GetMaxprate() const MOZ_OVERRIDE;
  virtual uint32_t GetMaxptime() const MOZ_OVERRIDE;
  virtual const std::string& GetMid() const MOZ_OVERRIDE;
  virtual const SdpMsidAttribute& GetMsid() const MOZ_OVERRIDE;
  virtual uint32_t GetPtime() const MOZ_OVERRIDE;
  virtual const SdpRtcpAttribute& GetRtcp() const MOZ_OVERRIDE;
  virtual const SdpRtcpFbAttribute& GetRtcpFb() const MOZ_OVERRIDE;
//  virtual const SdpRtcpRemoteCandidates& GetRemoteCandidates() const MOZ_OVERRIDE;
  virtual const SdpRtpmapAttribute& GetRtpmap() const MOZ_OVERRIDE;
  virtual const SdpSctpmapAttribute& GetSctpmap() const MOZ_OVERRIDE;
  virtual const SdpSetupAttribute& GetSetup() const MOZ_OVERRIDE;
  virtual const SdpSsrcAttribute& GetSsrc() const MOZ_OVERRIDE;
  virtual const SdpSsrcGroupAttribute& GetSsrcGroup() const MOZ_OVERRIDE;

  virtual void SetAttribute(const SdpAttribute &) MOZ_OVERRIDE;
  virtual ~SipccSdpAttributeList() {}

private:
  SipccSdpAttributeList() {}

  void Load(sdp_t* sdp, uint16_t level);

  sdp_t* mSdp;
  uint16_t mLevel;
};

}

#endif
