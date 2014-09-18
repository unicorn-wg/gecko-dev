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
  virtual bool HasAttribute(AttributeType type) const MOZ_OVERRIDE;
  virtual const SdpAttribute* GetAttribute(AttributeType type) const MOZ_OVERRIDE;
  virtual void SetAttribute(SdpAttribute* attr) MOZ_OVERRIDE;

  virtual const SdpConnectionAttribute& GetConnection() const MOZ_OVERRIDE;
  virtual const SdpFingerprintAttribute& GetFingerprint() const MOZ_OVERRIDE;
  virtual const SdpGroupAttribute& GetGroup() const MOZ_OVERRIDE;
  virtual const SdpIceOptionsAttribute& GetIceOptions() const MOZ_OVERRIDE;
  virtual const SdpRtcpAttribute& GetRtcp() const MOZ_OVERRIDE;
  virtual const SdpRemoteCandidatesAttribute& GetRemoteCandidates() const MOZ_OVERRIDE;
  virtual const SdpSetupAttribute& GetSetup() const MOZ_OVERRIDE;
  virtual const SdpSsrcAttribute& GetSsrc() const MOZ_OVERRIDE;
  virtual const SdpSsrcGroupAttribute& GetSsrcGroup() const MOZ_OVERRIDE;

  // These attributes can appear multiple times, so the returned
  // classes actually represent a collection of values.
  virtual const SdpCandidateAttributeList& GetCandidate() const MOZ_OVERRIDE;
  virtual const SdpExtmapAttributeList& GetExtmap() const MOZ_OVERRIDE;
  virtual const SdpFmtpAttributeList& GetFmtp() const MOZ_OVERRIDE;
  virtual const SdpImageattrAttributeList& GetImageattr() const MOZ_OVERRIDE;
  virtual const SdpMsidAttributeList& GetMsid() const MOZ_OVERRIDE;
  virtual const SdpRtcpFbAttributeList& GetRtcpFb() const MOZ_OVERRIDE;
  virtual const SdpRtpmapAttributeList& GetRtpmap() const MOZ_OVERRIDE;
  virtual const SdpSctpmapAttributeList& GetSctpmap() const MOZ_OVERRIDE;

  // These attributes are effectively simple types, so we'll make life
  // easy by just returning their value.
  virtual const std::string& GetIcePwd() const MOZ_OVERRIDE;
  virtual const std::string& GetIceUfrag() const MOZ_OVERRIDE;
  virtual const std::string& GetIdentity() const MOZ_OVERRIDE;
  virtual const std::string& GetLabel() const MOZ_OVERRIDE;
  virtual unsigned int GetMaxprate() const MOZ_OVERRIDE;
  virtual unsigned int GetMaxptime() const MOZ_OVERRIDE;
  virtual const std::string& GetMid() const MOZ_OVERRIDE;
  virtual unsigned int GetPtime() const MOZ_OVERRIDE;

  virtual ~SipccSdpAttributeList();

private:
  static std::string sEmptyString;

  SipccSdpAttributeList(SipccSdpAttributeList* sessionLevel = nullptr) {}

  void Load(sdp_t* sdp, uint16_t level);
  void LoadSimpleString(sdp_t* sdp, uint16_t level, sdp_attr_e attr,
                        AttributeType targetType, const std::string& name);

  SipccSdpAttributeList* mSessionLevel;

  std::map<AttributeType, SdpAttribute*> mAttributes;
};

}

#endif
