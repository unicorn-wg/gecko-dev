/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

#include "mozilla/Assertions.h"

namespace mozilla {

SipccSdpAttributeList::~SipccSdpAttributeList() {
  for (auto it = mAttributes.begin(); it != mAttributes.end(); ++it) {
    delete it->second;
  }
}


void
SipccSdpAttributeList::LoadSimpleString(sdp_t* sdp, uint16_t level, sdp_attr_e attr,
                                        AttributeType targetType, const std::string& name) {
  const char* value = sdp_attr_get_simple_string(sdp, attr, level, 0, 0);
  if (value) {
    SetAttribute(new SdpOtherAttribute(targetType, name, std::string(value)));
  }
}

void
SipccSdpAttributeList::Load(sdp_t* sdp, uint16_t level) {
  LoadSimpleString(sdp, level, SDP_ATTR_MID,
                   SdpAttribute::kMidAttribute, "mid");
  LoadSimpleString(sdp, level, SDP_ATTR_LABEL,
                   SdpAttribute::kLabelAttribute, "label");
  LoadSimpleString(sdp, level, SDP_ATTR_IDENTITY,
                   SdpAttribute::kIdentityAttribute, "identity");

  char *value;
  sdp_result_e result =
      sdp_attr_get_ice_attribute(sdp, level, 0, SDP_ATTR_ICE_UFRAG, 0, &value);
  if (result == SDP_SUCCESS) {
    SetAttribute(new SdpOtherAttribute(SdpAttribute::kIceUfragAttribute,
                                       "ice-ufrag", std::string(value)));
  }
  result =
      sdp_attr_get_ice_attribute(sdp, level, 0, SDP_ATTR_ICE_PWD, 0, &value);
  if (result == SDP_SUCCESS) {
    SetAttribute(new SdpOtherAttribute(SdpAttribute::kIcePwdAttribute,
                                       "ice-pwd", std::string(value)));
  }
}

bool
SipccSdpAttributeList::HasAttribute(AttributeType type) const {
  return mAttributes.count(type) > 0;
}

const SdpAttribute*
SipccSdpAttributeList::GetAttribute(AttributeType type) const {
  return mAttributes.find(type)->second;
}

void
SipccSdpAttributeList::SetAttribute(SdpAttribute* attr) {
  mAttributes[attr->GetType()] = attr;
}

const SdpCandidateAttributeList&
SipccSdpAttributeList::GetCandidate() const {
  MOZ_CRASH();
}

const SdpConnectionAttribute&
SipccSdpAttributeList::GetConnection() const {
  MOZ_CRASH();
}

const SdpExtmapAttributeList&
SipccSdpAttributeList::GetExtmap() const {
  MOZ_CRASH();
}

const SdpFingerprintAttribute&
SipccSdpAttributeList::GetFingerprint() const {
  MOZ_CRASH();
}

const SdpFmtpAttributeList&
SipccSdpAttributeList::GetFmtp() const {
  MOZ_CRASH();
}

const SdpGroupAttribute&
SipccSdpAttributeList::GetGroup() const {
  MOZ_CRASH();
}

const SdpIceOptionsAttribute&
SipccSdpAttributeList::GetIceOptions() const {
  MOZ_CRASH();
}

const std::string&
SipccSdpAttributeList::GetIcePwd() const {
  if (!HasAttribute(SdpAttribute::kIcePwdAttribute)) {
    if (mSessionLevel) {
      return mSessionLevel->GetIcePwd();
    }
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIcePwdAttribute);
  return static_cast<const SdpOtherAttribute*>(attr)->GetValue();
}

const std::string&
SipccSdpAttributeList::GetIceUfrag() const {
  if (!HasAttribute(SdpAttribute::kIceUfragAttribute)) {
    if (mSessionLevel) {
      return mSessionLevel->GetIceUfrag();
    }
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIceUfragAttribute);
  return static_cast<const SdpOtherAttribute*>(attr)->GetValue();
}

const std::string&
SipccSdpAttributeList::GetIdentity() const {
  if (mSessionLevel) {
    MOZ_CRASH("This is session-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kIdentityAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIdentityAttribute);
  return static_cast<const SdpOtherAttribute*>(attr)->GetValue();
}

const SdpImageattrAttributeList&
SipccSdpAttributeList::GetImageattr() const {
  MOZ_CRASH();
}

const std::string&
SipccSdpAttributeList::GetLabel() const {
  if (!mSessionLevel) {
    MOZ_CRASH("This is media-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kLabelAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kLabelAttribute);
  return static_cast<const SdpOtherAttribute*>(attr)->GetValue();
}

uint32_t
SipccSdpAttributeList::GetMaxprate() const {
  return 0;
}

uint32_t
SipccSdpAttributeList::GetMaxptime() const {
  return 0;
}

const std::string&
SipccSdpAttributeList::GetMid() const {
  if (!mSessionLevel) {
    MOZ_CRASH("This is media-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kMidAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMidAttribute);
  return static_cast<const SdpOtherAttribute*>(attr)->GetValue();
}

const SdpMsidAttributeList&
SipccSdpAttributeList::GetMsid() const {
  MOZ_CRASH();
}

uint32_t
SipccSdpAttributeList::GetPtime() const {
  return 0;
}

const SdpRtcpAttribute&
SipccSdpAttributeList::GetRtcp() const {
  MOZ_CRASH();
}

const SdpRtcpFbAttributeList&
SipccSdpAttributeList::GetRtcpFb() const {
  MOZ_CRASH();
}

const SdpRemoteCandidatesAttribute&
SipccSdpAttributeList::GetRemoteCandidates() const {
  MOZ_CRASH();
}

const SdpRtpmapAttributeList&
SipccSdpAttributeList::GetRtpmap() const {
  MOZ_CRASH();
}

const SdpSctpmapAttributeList&
SipccSdpAttributeList::GetSctpmap() const {
  MOZ_CRASH();
}

const SdpSetupAttribute&
SipccSdpAttributeList::GetSetup() const {
  MOZ_CRASH();
}

const SdpSsrcAttribute&
SipccSdpAttributeList::GetSsrc() const {
  MOZ_CRASH();
}

const SdpSsrcGroupAttribute&
SipccSdpAttributeList::GetSsrcGroup() const {
  MOZ_CRASH();
}


} // namespace mozilla
