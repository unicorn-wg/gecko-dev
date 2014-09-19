/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

#include <ostream>
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
SipccSdpAttributeList::HasAttribute(AttributeType type, bool sessionFallback) const {
  bool isHere = mAttributes[static_cast<size_t>(type)] != nullptr;
  if (!isHere && !AtSessionLevel() && sessionFallback) {
    return mSessionLevel->HasAttribute(type, false);
  }
  return isHere;
}

const SdpAttribute*
SipccSdpAttributeList::GetAttribute(AttributeType type, bool sessionFallback) const {
  const SdpAttribute* value = mAttributes[static_cast<size_t>(type)];
  if (!value && !AtSessionLevel() && sessionFallback) {
    return mSessionLevel->GetAttribute(type);
  }
  return value;
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
                                        AttributeType targetType) {
  const char* value = sdp_attr_get_simple_string(sdp, attr, level, 0, 1);
  if (value) {
    SetAttribute(new SdpStringAttribute(targetType, std::string(value)));
  }
  return value != nullptr;
}

bool SipccSdpAttributeList::LoadSimpleStrings(sdp_t* sdp, uint16_t level,
                                              SdpErrorHolder& errorHolder) {
  bool result = LoadSimpleString(sdp, level, SDP_ATTR_MID,
                                 SdpAttribute::kMidAttribute);
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "mid attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_LABEL,
                            SdpAttribute::kLabelAttribute);
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "label attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_IDENTITY,
                            SdpAttribute::kIdentityAttribute);
  if (result && !AtSessionLevel()) {
    errorHolder.AddParseError(0, "identity attribute at the media level");
    return false;
  }
  return true;
}

bool
SipccSdpAttributeList::LoadSimpleNumber(sdp_t* sdp, uint16_t level, sdp_attr_e attr,
                                             AttributeType targetType) {
  bool exists = sdp_attr_valid(sdp, attr, level, 0, 1);
  if (exists) {
    uint32_t value = sdp_attr_get_simple_u32(sdp, attr, level, 0, 1);
    SetAttribute(new SdpNumberAttribute(targetType, value));
  }
  return exists;
}

bool SipccSdpAttributeList::LoadSimpleNumbers(sdp_t* sdp, uint16_t level,
                                              SdpErrorHolder& errorHolder) {
  bool result = LoadSimpleNumber(sdp, level, SDP_ATTR_PTIME,
                            SdpAttribute::kPtimeAttribute);
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "ptime attribute at the session level");
    return false;
  }
  result = LoadSimpleNumber(sdp, level, SDP_ATTR_MAXPTIME,
                            SdpAttribute::kMaxptimeAttribute);
  if (result && AtSessionLevel()) {
    errorHolder.AddParseError(0, "maxptime attribute at the session level");
    return false;
  }
  return true;
}

void
SipccSdpAttributeList::LoadFlags(sdp_t* sdp, uint16_t level) {
  bool exists = sdp_attr_valid(sdp, SDP_ATTR_RTCP_MUX, level, 0, 1);
  if (exists) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kRtcpMuxAttribute));
  }
  exists = sdp_attr_valid(sdp, SDP_ATTR_ICE_LITE, level, 0, 1);
  if (exists) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kIceLiteAttribute));
  }
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
    SetAttribute(new SdpStringAttribute(SdpAttribute::kIceUfragAttribute,
                                        std::string(value)));
  }
  sdpres =
      sdp_attr_get_ice_attribute(sdp, level, 0, SDP_ATTR_ICE_PWD, 1, &value);
  if (sdpres == SDP_SUCCESS) {
    SetAttribute(new SdpStringAttribute(SdpAttribute::kIcePwdAttribute,
                                        std::string(value)));
  }

}

void
SipccSdpAttributeList::LoadFingerprint(sdp_t* sdp, uint16_t level) {
  char *value;
  std::vector<std::string> fingerprints;
  for (uint16_t i = 1; ; ++i) {
    sdp_result_e result = sdp_attr_get_dtls_fingerprint_attribute(
          sdp,
          level,
          0,
          SDP_ATTR_DTLS_FINGERPRINT,
          i,
          &value);

    if (result == SDP_SUCCESS) {
      fingerprints.push_back(value);
    } else {
      break;
    }
  }

  if (!fingerprints.empty()) {
    SdpFingerprintAttributeList *fingerprint_attrs =
      new SdpFingerprintAttributeList();

    for (auto i = fingerprints.begin(); i != fingerprints.end(); ++i) {
      // sipcc does not expose parse code for this
      const char * const start = fingerprints[0].c_str();
      const char *c = start;
      const size_t size = fingerprints[0].size();

      while (*c != '\0') {
        if (*c == ' ' || *c == '\t') {
          break;
        }
        ++c;
      }

      std::string algorithm_str(fingerprints[0].substr(0, c - start));

      while (*c != '\0') {
        if (*c != ' ' && *c != '\t') {
          break;
        }
        ++c;
      }

      std::string fingerprint(fingerprints[0].substr(c - start, size));

      SdpFingerprintAttributeList::HashAlgorithm algorithm =
        SdpFingerprintAttributeList::kUnknownAlgorithm;

      if (algorithm_str == "sha-1") {
        algorithm = SdpFingerprintAttributeList::kSha1;
      } else if (algorithm_str == "sha-224") {
        algorithm = SdpFingerprintAttributeList::kSha224;
      } else if (algorithm_str == "sha-256") {
        algorithm = SdpFingerprintAttributeList::kSha256;
      } else if (algorithm_str == "sha-384") {
        algorithm = SdpFingerprintAttributeList::kSha384;
      } else if (algorithm_str == "sha-512") {
        algorithm = SdpFingerprintAttributeList::kSha512;
      } else if (algorithm_str == "md5") {
        algorithm = SdpFingerprintAttributeList::kMd5;
      } else if (algorithm_str == "md2") {
        algorithm = SdpFingerprintAttributeList::kMd2;
      }

      if (algorithm != SdpFingerprintAttributeList::kUnknownAlgorithm &&
          !fingerprint.empty()) {
        fingerprint_attrs->PushEntry(algorithm, fingerprint);
      }
    }

    if (!fingerprint_attrs->mFingerprints.empty()) {
      SetAttribute(fingerprint_attrs);
    } else {
      // Can happen if we only find unknown algorithms
      delete fingerprint_attrs;
    }
  }
}

void
SipccSdpAttributeList::LoadCandidate(sdp_t* sdp, uint16_t level) {
  char *value;
  auto candidates = new SdpMultiStringAttribute(
      SdpAttribute::kCandidateAttribute);
  for (uint16_t i = 1; ; ++i) {
    sdp_result_e result = sdp_attr_get_ice_attribute(
          sdp,
          level,
          0,
          SDP_ATTR_ICE_CANDIDATE,
          i,
          &value);

    if (result == SDP_SUCCESS) {
      candidates->mValues.push_back(value);
    } else {
      break;
    }
  }

  if (candidates->mValues.empty()) {
    delete candidates;
  } else {
    SetAttribute(candidates);
  }
}

bool
SipccSdpAttributeList::LoadRtpmap(sdp_t* sdp, uint16_t level,
                                  SdpErrorHolder& errorHolder) {
  SdpRtpmapAttributeList* rtpmap = new SdpRtpmapAttributeList();
  uint16_t count = sdp_get_media_num_payload_types(sdp, level);
  for (uint16_t i = 0; i < count; ++i) {
    uint16_t pt = sdp_attr_get_rtpmap_payload_type(sdp, level, 0, i + 1);
    const char* ccName = sdp_attr_get_rtpmap_encname(sdp, level, 0, i + 1);
    std::string name;
    if (ccName) {
      name = ccName;
    }
    uint32_t clock = sdp_attr_get_rtpmap_clockrate(sdp, level, 0, i + 1);
    uint16_t channels = sdp_attr_get_rtpmap_num_chan(sdp, level, 0, i + 1);
    std::ostringstream ospt;
    ospt << pt;
    rtpmap->PushEntry(ospt.str(), name, clock, channels);
  }
  SetAttribute(rtpmap);
  return true;
}

void
SipccSdpAttributeList::LoadSetup(sdp_t* sdp, uint16_t level) {
  sdp_setup_type_e setup_type;
  auto sdpres = sdp_attr_get_setup_attribute(sdp, level, 0, 1, &setup_type);

  if (sdpres != SDP_SUCCESS) {
    return;
  }

  SdpSetupAttribute::Role role;
  switch (setup_type) {
    case SDP_SETUP_ACTIVE: role = SdpSetupAttribute::kActive; break;
    case SDP_SETUP_PASSIVE: role = SdpSetupAttribute::kPassive; break;
    case SDP_SETUP_ACTPASS: role = SdpSetupAttribute::kActpass; break;
    case SDP_SETUP_HOLDCONN: role = SdpSetupAttribute::kHoldconn; break;
    default:
      return;
  }

  SetAttribute(new SdpSetupAttribute(role));
}

bool
SipccSdpAttributeList::LoadGroups(sdp_t* sdp, uint16_t level,
                                  SdpErrorHolder& errorHolder) {
  uint16_t attrCount = 0;
  if(sdp_attr_num_instances(sdp, level, 0, SDP_ATTR_GROUP, &attrCount)
     != SDP_SUCCESS) {
    errorHolder.AddParseError(0, "a=group instances aren't correct");
    return false;
  }

  SdpGroupAttributeList* groups = new SdpGroupAttributeList();
  for (uint16_t attr = 1; attr <= attrCount; ++attr) {
    SdpGroupAttributeList::Semantics semantics;
    std::vector<std::string> tags;

    switch (sdp_get_group_attr(sdp, level, 0, attr)) {
      case SDP_GROUP_ATTR_FID:
        semantics = SdpGroupAttributeList::kFid; break;
      case SDP_GROUP_ATTR_LS:
        semantics = SdpGroupAttributeList::kLs; break;
      case SDP_GROUP_ATTR_ANAT:
        semantics = SdpGroupAttributeList::kAnat; break;
      case SDP_GROUP_ATTR_BUNDLE:
        semantics = SdpGroupAttributeList::kBundle; break;
      default: continue;
    }

    uint16_t idCount = sdp_get_group_num_id(sdp, level, 0, attr);
    for (uint16_t id = 1; id <= idCount; ++id) {
      const char* idStr = sdp_get_group_id(sdp, level, 0, attr, id);
      if (!idStr) {
        std::ostringstream os;
        os << "bad a=group identifier at " << (attr - 1) << ", " << (id - 1);
        errorHolder.AddParseError(0, os.str());
        delete groups;
        return false;
      }
      tags.push_back(std::string(idStr));
    }
    groups->PushEntry(semantics, tags);
  }
  SetAttribute(groups);
  return true;
}

bool
SipccSdpAttributeList::Load(sdp_t* sdp, uint16_t level,
                            SdpErrorHolder& errorHolder) {

  if (!LoadSimpleStrings(sdp, level, errorHolder) ||
      !LoadSimpleNumbers(sdp, level, errorHolder)) {
    return false;
  }
  LoadFlags(sdp, level);

  if (AtSessionLevel()) {
    if (!LoadGroups(sdp, level, errorHolder)) {
      return false;
    }
  } else {
    if (!LoadDirection(sdp, level, errorHolder)) {
      return false;
    }
    if (!LoadRtpmap(sdp, level, errorHolder)) {
      return false;
    }
  }
  LoadIceAttributes(sdp, level);
  LoadCandidate(sdp, level);
  LoadFingerprint(sdp, level);
  LoadSetup(sdp, level);

  return true;
}

const std::vector<std::string>&
SipccSdpAttributeList::GetCandidate() const {
  if (AtSessionLevel()) {
    MOZ_CRASH("This is media-level only foo!");
  }

  if (!HasAttribute(SdpAttribute::kCandidateAttribute)) {
    MOZ_CRASH();
  }

  return static_cast<const SdpMultiStringAttribute*>(GetAttribute(
        SdpAttribute::kCandidateAttribute))->mValues;
}

const SdpConnectionAttribute&
SipccSdpAttributeList::GetConnection() const {
  if (!HasAttribute(SdpAttribute::kConnectionAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpConnectionAttribute*>(GetAttribute(
        SdpAttribute::kConnectionAttribute));
}

SdpDirectionAttribute::Direction
SipccSdpAttributeList::GetDirection() const {
  // Do we need to implement fallthrough/default, or does sipcc do that
  // correctly for us?
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kDirectionAttribute);
  return static_cast<const SdpDirectionAttribute*>(attr)->mValue;
}

const SdpExtmapAttributeList&
SipccSdpAttributeList::GetExtmap() const {
  if (!HasAttribute(SdpAttribute::kExtmapAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpExtmapAttributeList*>(GetAttribute(
        SdpAttribute::kExtmapAttribute));
}

const SdpFingerprintAttributeList&
SipccSdpAttributeList::GetFingerprint() const {
  if (!HasAttribute(SdpAttribute::kFingerprintAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kFingerprintAttribute);
  return *static_cast<const SdpFingerprintAttributeList*>(attr);
}

const SdpFmtpAttributeList&
SipccSdpAttributeList::GetFmtp() const {
  if (AtSessionLevel()) {
    MOZ_CRASH("This is media-level only foo!");
  }

  if (!HasAttribute(SdpAttribute::kFmtpAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpFmtpAttributeList*>(GetAttribute(
      SdpAttribute::kFmtpAttribute));
}

const SdpGroupAttributeList&
SipccSdpAttributeList::GetGroup() const {
  if (!AtSessionLevel()) {
    MOZ_CRASH("This is session-level only foo!");
  }

  if (!HasAttribute(SdpAttribute::kGroupAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpGroupAttributeList*>(GetAttribute(
        SdpAttribute::kGroupAttribute));
}

const SdpIceOptionsAttribute&
SipccSdpAttributeList::GetIceOptions() const {
  MOZ_CRASH();
}

const std::string&
SipccSdpAttributeList::GetIcePwd() const {
  if (!HasAttribute(SdpAttribute::kIcePwdAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIcePwdAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const std::string&
SipccSdpAttributeList::GetIceUfrag() const {
  if (!HasAttribute(SdpAttribute::kIceUfragAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIceUfragAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const std::string&
SipccSdpAttributeList::GetIdentity() const {
  if (!AtSessionLevel()) {
    MOZ_CRASH("This is session-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kIdentityAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIdentityAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const SdpImageattrAttributeList&
SipccSdpAttributeList::GetImageattr() const {
  MOZ_CRASH();
}

const std::string&
SipccSdpAttributeList::GetLabel() const {
  if (AtSessionLevel()) {
    MOZ_CRASH("This is media-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kLabelAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kLabelAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

uint32_t
SipccSdpAttributeList::GetMaxptime() const {
  if (!HasAttribute(SdpAttribute::kMaxptimeAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMaxptimeAttribute);
  return static_cast<const SdpNumberAttribute*>(attr)->GetValue();
}

const std::string&
SipccSdpAttributeList::GetMid() const {
  if (AtSessionLevel()) {
    MOZ_CRASH("This is media-level only foo!");
  }
  if (!HasAttribute(SdpAttribute::kMidAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMidAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const SdpMsidAttributeList&
SipccSdpAttributeList::GetMsid() const {
  MOZ_CRASH();
}

uint32_t
SipccSdpAttributeList::GetPtime() const {
  if (!HasAttribute(SdpAttribute::kPtimeAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kPtimeAttribute);
  return static_cast<const SdpNumberAttribute*>(attr)->GetValue();
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
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kRtpmapAttribute);
  return *static_cast<const SdpRtpmapAttributeList*>(attr);
}

const SdpSctpmapAttributeList&
SipccSdpAttributeList::GetSctpmap() const {
  MOZ_CRASH();
}

const SdpSetupAttribute&
SipccSdpAttributeList::GetSetup() const {
  if (!HasAttribute(SdpAttribute::kSetupAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kSetupAttribute);
  return *static_cast<const SdpSetupAttribute*>(attr);
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
