/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

#include "mozilla/Assertions.h"
#include "signaling/src/sdp/SdpErrorHolder.h"

namespace mozilla {

/* static */ std::string
SipccSdpAttributeList::sEmptyString = "";

SipccSdpAttributeList::SipccSdpAttributeList(
    const SipccSdpAttributeList* sessionLevel)
    : mSessionLevel(sessionLevel)
{
  memset(&mAttributes, 0, sizeof(mAttributes));
}

SipccSdpAttributeList::~SipccSdpAttributeList() {
  for (size_t i = 0; i < kMaxAttributeIndex; ++i) {
    delete mAttributes[i];
  }
  for (auto it = mOtherAttributes.begin();
       it != mOtherAttributes.end(); ++it) {
    delete *it;
  }
}

bool
SipccSdpAttributeList::HasAttribute(AttributeType type) const {
  return mAttributes[static_cast<size_t>(type)] != nullptr;
}

const SdpAttribute*
SipccSdpAttributeList::GetAttribute(AttributeType type) const {
  return mAttributes[static_cast<size_t>(type)];
}

void SipccSdpAttributeList::RemoveAttribute(AttributeType type) {
  if (HasAttribute(type)) {
    delete mAttributes[static_cast<size_t>(type)];
  }
}

void
SipccSdpAttributeList::SetAttribute(SdpAttribute* attr) {
  if (attr->GetType() == SdpAttribute::kOtherAttribute) {
    mOtherAttributes.push_back(attr);
  } else {
    RemoveAttribute(attr->GetType());
    mAttributes[attr->GetType()] = attr;
  }
}

bool
SipccSdpAttributeList::LoadSimpleString(sdp_t* sdp, uint16_t level, sdp_attr_e attr,
                                        AttributeType targetType, const std::string& name) {
  const char* value = sdp_attr_get_simple_string(sdp, attr, level, 0, 0);
  if (value) {
    SetAttribute(new SdpOtherAttribute(targetType, name, std::string(value)));
  }
  return value != nullptr;
}

bool
SipccSdpAttributeList::LoadDirection(sdp_t* sdp, uint16_t level,
                                     SdpErrorHolder& errorHolder) {
  SdpDirectionAttribute::Direction dir;
  switch(sdp_get_media_direction(sdp, level, 0)) {
    case SDP_DIRECTION_SENDRECV:
      dir = SdpDirectionAttribute::kSendrecv; break;
    case SDP_DIRECTION_SENDONLY:
      dir = SdpDirectionAttribute::kSendonly; break;
    case SDP_DIRECTION_RECVONLY:
      dir = SdpDirectionAttribute::kRecvonly; break;
    case SDP_DIRECTION_INACTIVE:
      dir = SdpDirectionAttribute::kInactive; break;
    default:
      errorHolder.AddParseError(0, "Bad direction attribute");
      return false;
  }
  SetAttribute(new SdpDirectionAttribute(dir));
  return true;
}

void
SipccSdpAttributeList::LoadIceAttributes(sdp_t* sdp, uint16_t level) {
    char *value;
  sdp_result_e sdpres =
      sdp_attr_get_ice_attribute(sdp, level, 0, SDP_ATTR_ICE_UFRAG, 1, &value);
  if (sdpres == SDP_SUCCESS) {
    SetAttribute(new SdpOtherAttribute(SdpAttribute::kIceUfragAttribute,
                                       "ice-ufrag", std::string(value)));
  }
  sdpres =
      sdp_attr_get_ice_attribute(sdp, level, 0, SDP_ATTR_ICE_PWD, 1, &value);
  if (sdpres == SDP_SUCCESS) {
    SetAttribute(new SdpOtherAttribute(SdpAttribute::kIcePwdAttribute,
                                       "ice-pwd", std::string(value)));
  }
}

bool
SipccSdpAttributeList::Load(sdp_t* sdp, uint16_t level,
                            SdpErrorHolder& errorHolder) {
  bool result = LoadSimpleString(sdp, level, SDP_ATTR_MID,
                                 SdpAttribute::kMidAttribute, "mid");
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "mid attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_LABEL,
                            SdpAttribute::kLabelAttribute, "label");
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "label attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_IDENTITY,
                            SdpAttribute::kIdentityAttribute, "identity");
  if (result && !AtSessionLevel()) {
    errorHolder.AddParseError(0, "identity attribute at the media level");
    return false;
  }

  if (!AtSessionLevel()) {
    if (!LoadDirection(sdp, level, errorHolder)) {
      return false;
    }
  }
  LoadIceAttributes(sdp, level);

  return true;
}

const SdpCandidateAttributeList&
SipccSdpAttributeList::GetCandidate() const {
  MOZ_CRASH();
}

const SdpConnectionAttribute&
SipccSdpAttributeList::GetConnection() const {
  MOZ_CRASH();
}

SdpDirectionAttribute::Direction
SipccSdpAttributeList::GetDirection() const {
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kDirectionAttribute);
  return static_cast<const SdpDirectionAttribute*>(attr)->mValue;
}

const SdpExtmapAttributeList&
SipccSdpAttributeList::GetExtmap() const {
  MOZ_CRASH();
}

const SdpFingerprintAttributeList&
SipccSdpAttributeList::GetFingerprint() const {
  if (!HasAttribute(SdpAttribute::kFingerprintAttribute)) {
    if (mSessionLevel) {
      return mSessionLevel->GetFingerprint();
    }
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kFingerprintAttribute);
  return *static_cast<const SdpFingerprintAttributeList*>(attr);
}

const SdpFmtpAttributeList&
SipccSdpAttributeList::GetFmtp() const {
  MOZ_CRASH();
}

const SdpGroupAttributeList&
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

const SdpSsrcAttributeList&
SipccSdpAttributeList::GetSsrc() const {
  MOZ_CRASH();
}

const SdpSsrcGroupAttributeList&
SipccSdpAttributeList::GetSsrcGroup() const {
  MOZ_CRASH();
}

void
SipccSdpAttributeList::Serialize(std::ostream& os) const {
  // Known attributes
  for (size_t i = 0; i < kMaxAttributeIndex; ++i) {
    if (mAttributes[i]) {
      os << *mAttributes[i];
    }
  }

  // Other attributes
  for (auto i = mOtherAttributes.begin(); i != mOtherAttributes.end(); ++i) {
    os << (**i);
  }
}

} // namespace mozilla
