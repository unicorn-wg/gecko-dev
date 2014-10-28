/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

#include <ostream>
#include "mozilla/Assertions.h"
#include "signaling/src/sdp/SdpErrorHolder.h"

extern "C" {
#include "signaling/src/sdp/sipcc/sdp_private.h"
}

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
  // Only do fallback when the attribute can appear at both the media and
  // session level
  if (!isHere &&
      !AtSessionLevel() &&
      sessionFallback &&
      SdpAttribute::IsAllowedAtSessionLevel(type) &&
      SdpAttribute::IsAllowedAtMediaLevel(type)) {
    return mSessionLevel->HasAttribute(type, false);
  }
  return isHere;
}

const SdpAttribute*
SipccSdpAttributeList::GetAttribute(AttributeType type, bool sessionFallback) const {
  const SdpAttribute* value = mAttributes[static_cast<size_t>(type)];
  // Only do fallback when the attribute can appear at both the media and
  // session level
  if (!value &&
      !AtSessionLevel() &&
      sessionFallback &&
      SdpAttribute::IsAllowedAtSessionLevel(type) &&
      SdpAttribute::IsAllowedAtMediaLevel(type)) {
    return mSessionLevel->GetAttribute(type);
  }
  return value;
}

void SipccSdpAttributeList::RemoveAttribute(AttributeType type) {
  if (HasAttribute(type, false)) {
    delete mAttributes[static_cast<size_t>(type)];
    mAttributes[static_cast<size_t>(type)] = nullptr;
  }
}

void
SipccSdpAttributeList::Clear() {
  for (size_t i = 0; i < kMaxAttributeIndex; ++i) {
    RemoveAttribute(static_cast<AttributeType>(i));
  }

  for (auto i = mOtherAttributes.begin(); i != mOtherAttributes.end(); ++i) {
    delete *i;
  }
  mOtherAttributes.clear();
}

void
SipccSdpAttributeList::SetAttribute(SdpAttribute* attr) {
  if (attr->GetType() == SdpAttribute::kOtherAttribute) {
    mOtherAttributes.push_back(attr);
  } else {
    // TODO: Should we be willing to set attributes somewhere they are not
    // allowed? Issue 188.
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
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_MID, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "mid attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_LABEL,
                            SdpAttribute::kLabelAttribute);
  if (result && AtSessionLevel()) {
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_LABEL, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "label attribute at the session level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_IDENTITY,
                            SdpAttribute::kIdentityAttribute);
  if (result && !AtSessionLevel()) {
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_IDENTITY, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "identity attribute at the media level");
    return false;
  }
  result = LoadSimpleString(sdp, level, SDP_ATTR_MSID_SEMANTIC,
                            SdpAttribute::kMsidSemanticAttribute);
  if (result && !AtSessionLevel()) {
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_MSID_SEMANTIC, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "msid-semantic attribute at the media level");
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
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_PTIME, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "ptime attribute at the session level");
    return false;
  }
  result = LoadSimpleNumber(sdp, level, SDP_ATTR_MAXPTIME,
                            SdpAttribute::kMaxptimeAttribute);
  if (result && AtSessionLevel()) {
    uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_MAXPTIME, level, 0, 1);
    errorHolder.AddParseError(lineNumber, "maxptime attribute at the session level");
    return false;
  }
  return true;
}

void
SipccSdpAttributeList::LoadFlags(sdp_t* sdp, uint16_t level) {
  bool exists = sdp_attr_valid(sdp, SDP_ATTR_RTCP_MUX, level, 0, 1);
  if (exists && !AtSessionLevel()) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kRtcpMuxAttribute));
  }
  exists = sdp_attr_valid(sdp, SDP_ATTR_ICE_LITE, level, 0, 1);
  if (exists && AtSessionLevel()) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kIceLiteAttribute));
  }
  exists = sdp_attr_valid(sdp, SDP_ATTR_END_OF_CANDIDATES, level, 0, 1);
  if (exists) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kEndOfCandidatesAttribute));
  }
  exists = sdp_attr_valid(sdp, SDP_ATTR_BUNDLE_ONLY, level, 0, 1);
  if (exists && !AtSessionLevel()) {
    SetAttribute(new SdpFlagAttribute(SdpAttribute::kBundleOnlyAttribute));
  }
}

static bool
ConvertDirection(sdp_direction_e sipcc_direction,
                 SdpDirectionAttribute::Direction *dir_outparam) {
  switch(sipcc_direction) {
    case SDP_DIRECTION_SENDRECV:
      *dir_outparam = SdpDirectionAttribute::kSendrecv; break;
    case SDP_DIRECTION_SENDONLY:
      *dir_outparam = SdpDirectionAttribute::kSendonly; break;
    case SDP_DIRECTION_RECVONLY:
      *dir_outparam = SdpDirectionAttribute::kRecvonly; break;
    case SDP_DIRECTION_INACTIVE:
      *dir_outparam = SdpDirectionAttribute::kInactive; break;
    default:
      return false;
  }

  return true;
}

bool
SipccSdpAttributeList::LoadDirection(sdp_t* sdp, uint16_t level,
                                     SdpErrorHolder& errorHolder) {
  SdpDirectionAttribute::Direction dir;
  if (!ConvertDirection(sdp_get_media_direction(sdp, level, 0),
                        &dir)) {
      errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                                "Internal error determining media direction");
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

  if (AtSessionLevel()) {
    const char* iceOptVal = sdp_attr_get_simple_string(sdp,
                                                       SDP_ATTR_ICE_OPTIONS,
                                                       level,
                                                       0,
                                                       1);
    if (iceOptVal) {
      auto* iceOptions =
          new SdpOptionsAttribute(SdpAttribute::kIceOptionsAttribute);
      iceOptions->Load(iceOptVal);
      SetAttribute(iceOptions);
    }
  }
}

void
SipccSdpAttributeList::LoadFingerprint(sdp_t* sdp, uint16_t level) {
  char *value;
  std::vector<std::string> fingerprints;
  for (uint16_t i = 1; i < UINT16_MAX; ++i) {
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

      std::string fpString(fingerprints[0].substr(c - start));
      std::vector<uint8_t> fingerprint =
          SdpFingerprintAttributeList::ParseFingerprint(fpString);
      if (fingerprint.size() == 0) {
        continue; // skip the bad ones
      }

      fingerprint_attrs->PushEntry(algorithm_str, fingerprint);
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
  for (uint16_t i = 1; i < UINT16_MAX; ++i) {
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
SipccSdpAttributeList::LoadSctpmap(sdp_t* sdp, uint16_t level,
                                   SdpErrorHolder& errorHolder) {
  SdpSctpmapAttributeList* sctpmap = new SdpSctpmapAttributeList();
  for (uint16_t i = 0; i < UINT16_MAX; ++i) {
    sdp_attr_t* attr = sdp_find_attr(sdp, level, 0, SDP_ATTR_SCTPMAP, i + 1);

    if (!attr) {
      break;
    }

    // Yeah, this is broken.
    uint16_t pt = attr->attr.sctpmap.port;
    uint16_t streams = attr->attr.sctpmap.streams;
    const char* name = attr->attr.sctpmap.protocol;

    std::ostringstream ospt;
    ospt << pt;
    sctpmap->PushEntry(ospt.str(), name, streams);
  }

  if (sctpmap->mSctpmaps.empty()) {
    delete sctpmap;
  } else {
    SetAttribute(sctpmap);
  }

  return true;
}

SdpRtpmapAttributeList::CodecType
SipccSdpAttributeList::GetCodecType(rtp_ptype type) {
  switch (type) {
    case RTP_PCMU:
      return SdpRtpmapAttributeList::kPCMU;
    case RTP_PCMA:
      return SdpRtpmapAttributeList::kPCMA;
    case RTP_G722:
      return SdpRtpmapAttributeList::kG722;
    case RTP_H264_P0:
    case RTP_H264_P1:
      return SdpRtpmapAttributeList::kH264;
    case RTP_OPUS:
      return SdpRtpmapAttributeList::kOpus;
    case RTP_VP8:
      return SdpRtpmapAttributeList::kVP8;
    case RTP_NONE:
      // Happens when sipcc doesn't know how to translate to the enum
    case RTP_CELP:
    case RTP_G726:
    case RTP_GSM:
    case RTP_G723:
    case RTP_DVI4:
    case RTP_DVI4_II:
    case RTP_LPC:
    case RTP_G728:
    case RTP_G729:
    case RTP_JPEG:
    case RTP_NV:
    case RTP_H261:
    case RTP_AVT:
    case RTP_L16:
    case RTP_H263:
    case RTP_ILBC:
    case RTP_I420:
      return SdpRtpmapAttributeList::kOtherCodec;
  }
  MOZ_CRASH();
}

bool
SipccSdpAttributeList::LoadRtpmap(sdp_t* sdp, uint16_t level,
                                  SdpErrorHolder& errorHolder) {
  SdpRtpmapAttributeList* rtpmap = new SdpRtpmapAttributeList();
  uint16_t count;
  sdp_result_e result = sdp_attr_num_instances(sdp, level, 0, SDP_ATTR_RTPMAP,
                                               &count);
  if (result != SDP_SUCCESS) {
    errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                              "Unable to get rtpmap size");
    return false;
  }
  for (uint16_t i = 0; i < count; ++i) {
    uint16_t pt = sdp_attr_get_rtpmap_payload_type(sdp, level, 0, i + 1);
    const char* ccName = sdp_attr_get_rtpmap_encname(sdp, level, 0, i + 1);
    std::string name;
    if (ccName) {
      name = ccName;
    } else {
      // Probably no rtpmap attribute for a pt in an m-line
      errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                                "No rtpmap attribute for payload type");
      continue;
    }
    SdpRtpmapAttributeList::CodecType codec =
      GetCodecType(sdp_get_known_payload_type(sdp, level, pt));

    uint32_t clock = sdp_attr_get_rtpmap_clockrate(sdp, level, 0, i + 1);
    uint16_t channels = 0;

    // sipcc gives us a channels value of "1" for video, which is broken
    if (sdp_get_media_type(sdp, level) == SDP_MEDIA_AUDIO) {
      channels = sdp_attr_get_rtpmap_num_chan(sdp, level, 0, i + 1);
    }

    std::ostringstream ospt;
    ospt << pt;
    rtpmap->PushEntry(ospt.str(), codec, name, clock, channels);
  }

  if (rtpmap->mRtpmaps.empty()) {
    delete rtpmap;
  } else {
    SetAttribute(rtpmap);
  }

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

  if (groups->mGroups.empty()) {
    delete groups;
  } else {
    SetAttribute(groups);
  }

  return true;
}

void
SipccSdpAttributeList::LoadFmtp(sdp_t* sdp, uint16_t level) {
  auto *fmtps = new SdpFmtpAttributeList;
  for (uint16_t i = 1; i < UINT16_MAX; ++i) {
    sdp_attr_t *attr = sdp_find_attr(sdp, level, 0, SDP_ATTR_FMTP, i);

    if (attr) {
      sdp_fmtp_t *fmtp = &(attr->attr.fmtp);
      flex_string fs;
      flex_string_init(&fs);

      std::stringstream os;
      // payload_num is the number in the fmtp attribute, verbatim
      os << fmtp->payload_num;

      rtp_ptype codec = sdp_get_known_payload_type(sdp,
                                                   level,
                                                   fmtp->payload_num);
      UniquePtr<SdpFmtpAttributeList::Parameters> parameters;

      switch (codec) {
        case RTP_H264_P0:
        case RTP_H264_P1:
          {
            SdpFmtpAttributeList::H264Parameters* h264_parameters(
                new SdpFmtpAttributeList::H264Parameters);

            strncpy(h264_parameters->sprop_parameter_sets,
                    fmtp->parameter_sets,
                    sizeof(h264_parameters->sprop_parameter_sets));

            h264_parameters->level_asymmetry_allowed =
              !!(fmtp->level_asymmetry_allowed);

            h264_parameters->packetization_mode = fmtp->packetization_mode;
            // Copied from VcmSIPCCBinding
#ifdef _WIN32
            sscanf_s(fmtp->profile_level_id,
                     "%x",
                     &h264_parameters->profile_level_id,
                     sizeof(unsigned*));
#else
            sscanf(fmtp->profile_level_id,
                   "%xu",
                   &h264_parameters->profile_level_id);
#endif
            h264_parameters->max_mbps = fmtp->max_mbps;
            h264_parameters->max_fs = fmtp->max_fs;
            h264_parameters->max_cpb = fmtp->max_cpb;
            h264_parameters->max_dpb = fmtp->max_dpb;
            h264_parameters->max_br = fmtp->max_br;

            parameters.reset(h264_parameters);
          }
          break;
        case RTP_VP8:
          {
            SdpFmtpAttributeList::VP8Parameters* vp8_parameters(
                new SdpFmtpAttributeList::VP8Parameters);

            vp8_parameters->max_fs = fmtp->max_fs;
            vp8_parameters->max_fr = fmtp->max_fr;

            parameters.reset(vp8_parameters);
          }
          break;
        default:
          {}
      }

      // Very lame, but we need direct access so we can get the serialized form
      sdp_result_e sdpres = sdp_build_attr_fmtp_params(sdp, fmtp, &fs);

      if (sdpres == SDP_SUCCESS) {
        fmtps->PushEntry(os.str(),
                         std::string(fs.buffer),
                         Move(parameters));
      }

      flex_string_free(&fs);
    } else {
      break;
    }
  }

  // Do we let missing fmtps slide?

  if (fmtps->mFmtps.empty()) {
    delete fmtps;
  } else {
    SetAttribute(fmtps);
  }
}

void
SipccSdpAttributeList::LoadMsids(sdp_t* sdp, uint16_t level,
                                 SdpErrorHolder& errorHolder) {
  uint16_t attrCount = 0;
  if(sdp_attr_num_instances(sdp, level, 0, SDP_ATTR_MSID, &attrCount)
     != SDP_SUCCESS) {
    errorHolder.AddParseError(0, "a=msid instances aren't correct");
    return;
  }
  auto* msids = new SdpMsidAttributeList;
  for (uint16_t i = 1; i <= attrCount; ++i) {
    const char* identifier = sdp_attr_get_msid_identifier(sdp, level, 0, i);
    if (!identifier) {
      uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_MSID, level, 0, i);
      errorHolder.AddParseError(lineNumber, "msid attribute with bad identity");
      continue;
    }
    const char* appdata = sdp_attr_get_msid_appdata(sdp, level, 0, i);
    if (!appdata) {
      uint32_t lineNumber = sdp_attr_line_number(sdp, SDP_ATTR_MSID, level, 0, i);
      errorHolder.AddParseError(lineNumber, "msid attribute with bad appdata");
      continue;
    }
    msids->PushEntry(identifier, appdata);
  }

  if (msids->mMsids.empty()) {
    delete msids;
  } else {
    SetAttribute(msids);
  }
}

void
SipccSdpAttributeList::LoadExtmap(sdp_t* sdp,
                                  uint16_t level,
                                  SdpErrorHolder& errorHolder) {
  auto *extmaps = new SdpExtmapAttributeList;
  for (uint16_t i = 1; i < UINT16_MAX; ++i) {
    sdp_attr_t *attr = sdp_find_attr(sdp, level, 0, SDP_ATTR_EXTMAP, i);

    if (attr) {
      sdp_extmap_t *extmap = &(attr->attr.extmap);

      SdpDirectionAttribute::Direction dir = SdpDirectionAttribute::kSendrecv;

      if (extmap->media_direction_specified &&
          !ConvertDirection(extmap->media_direction, &dir)) {
        uint32_t lineNumber =
          sdp_attr_line_number(sdp, SDP_ATTR_EXTMAP, level, 0, i);
        errorHolder.AddParseError(lineNumber,
            "Could not determine direction for extmap attribute.");
        continue;
      }

      extmaps->PushEntry(extmap->id,
                         dir,
                         extmap->media_direction_specified,
                         extmap->uri,
                         extmap->extension_attributes);
    } else {
      break;
    }
  }

  if (extmaps->mExtmaps.empty()) {
    delete extmaps;
  } else {
    if (!AtSessionLevel() &&
        mSessionLevel->HasAttribute(SdpAttribute::kExtmapAttribute)) {
      uint32_t lineNumber =
        sdp_attr_line_number(sdp, SDP_ATTR_EXTMAP, level, 0, 1);
      errorHolder.AddParseError(lineNumber,
          "extmap attributes in both session and media level");
    }
    SetAttribute(extmaps);
  }
}

void
SipccSdpAttributeList::LoadRtcpFb(sdp_t* sdp,
                                  uint16_t level,
                                  SdpErrorHolder& errorHolder) {
  auto *rtcpfbs = new SdpRtcpFbAttributeList;
  for (uint16_t i = 1; i < UINT16_MAX; ++i) {
    sdp_attr_t *attr = sdp_find_attr(sdp, level, 0, SDP_ATTR_RTCP_FB, i);

    if (!attr) {
      break;
    }

    sdp_fmtp_fb_t *rtcpfb  = &attr->attr.rtcp_fb;

    std::stringstream os;
    os << rtcpfb->payload_num;

    std::string pt(os.str());
    SdpRtcpFbAttributeList::Type type;
    std::string parameter;
    std::string extra(rtcpfb->extra);

    switch (rtcpfb->feedback_type) {
      case SDP_RTCP_FB_ACK:
        type = SdpRtcpFbAttributeList::kAck;
        switch (rtcpfb->param.ack) {
          // TODO: sipcc doesn't seem to support ack with no following token. Issue 189.
          case SDP_RTCP_FB_ACK_RPSI:
            parameter = SdpRtcpFbAttributeList::rpsi;
            break;
          case SDP_RTCP_FB_ACK_APP:
            parameter = SdpRtcpFbAttributeList::app;
            break;
          default:
            // Unknown type, ignore.
            continue;
        }
        break;
      case SDP_RTCP_FB_CCM:
        type = SdpRtcpFbAttributeList::kCcm;
        switch (rtcpfb->param.ccm) {
          case SDP_RTCP_FB_CCM_FIR:
            parameter = SdpRtcpFbAttributeList::fir;
            break;
          case SDP_RTCP_FB_CCM_TMMBR:
            parameter = SdpRtcpFbAttributeList::tmmbr;
            break;
          case SDP_RTCP_FB_CCM_TSTR:
            parameter = SdpRtcpFbAttributeList::tstr;
            break;
          case SDP_RTCP_FB_CCM_VBCM:
            parameter = SdpRtcpFbAttributeList::vbcm;
            break;
          default:
            // Unknown type, ignore
            continue;
        }
        break;
      case SDP_RTCP_FB_NACK:
        type = SdpRtcpFbAttributeList::kNack;
        switch (rtcpfb->param.nack) {
          case SDP_RTCP_FB_NACK_BASIC:
            break;
          case SDP_RTCP_FB_NACK_SLI:
            parameter = SdpRtcpFbAttributeList::sli;
            break;
          case SDP_RTCP_FB_NACK_PLI:
            parameter = SdpRtcpFbAttributeList::pli;
            break;
          case SDP_RTCP_FB_NACK_RPSI:
            parameter = SdpRtcpFbAttributeList::rpsi;
            break;
          case SDP_RTCP_FB_NACK_APP:
            parameter = SdpRtcpFbAttributeList::app;
            break;
          default:
            // Unknown type, ignore
            continue;
        }
        break;
      case SDP_RTCP_FB_TRR_INT:
        {
          type = SdpRtcpFbAttributeList::kTrrInt;
          std::ostringstream os;
          os << rtcpfb->param.trr_int;
          parameter = os.str();
        }
        break;
      default:
        // Unknown type, ignore
        continue;
    }

    rtcpfbs->PushEntry(pt, type, parameter, extra);
  }

  if (rtcpfbs->mFeedbacks.empty()) {
    delete rtcpfbs;
  } else {
    SetAttribute(rtcpfbs);
  }
}

bool
SipccSdpAttributeList::Load(sdp_t* sdp, uint16_t level,
                            SdpErrorHolder& errorHolder) {

  if (!LoadSimpleStrings(sdp, level, errorHolder) ||
      !LoadSimpleNumbers(sdp, level, errorHolder)) {
    return false;
  }
  LoadFlags(sdp, level);

  if (!LoadDirection(sdp, level, errorHolder)) {
    return false;
  }

  if (AtSessionLevel()) {
    if (!LoadGroups(sdp, level, errorHolder)) {
      return false;
    }
  } else {
    sdp_media_e mtype = sdp_get_media_type(sdp, level);
    if (mtype == SDP_MEDIA_APPLICATION) {
      if (!LoadSctpmap(sdp, level, errorHolder)) {
        return false;
      }
    } else {
      if (!LoadRtpmap(sdp, level, errorHolder)) {
        return false;
      }
    }
    LoadCandidate(sdp, level);
    LoadFmtp(sdp, level);
    LoadMsids(sdp, level, errorHolder);
    LoadRtcpFb(sdp, level, errorHolder);
  }

  LoadIceAttributes(sdp, level);
  LoadFingerprint(sdp, level);
  LoadSetup(sdp, level);
  LoadExtmap(sdp, level, errorHolder);

  return true;
}

const std::vector<std::string>&
SipccSdpAttributeList::GetCandidate() const {
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
  if (!HasAttribute(SdpAttribute::kFmtpAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpFmtpAttributeList*>(GetAttribute(
      SdpAttribute::kFmtpAttribute));
}

const SdpGroupAttributeList&
SipccSdpAttributeList::GetGroup() const {
  if (!HasAttribute(SdpAttribute::kGroupAttribute)) {
    MOZ_CRASH();
  }

  return *static_cast<const SdpGroupAttributeList*>(GetAttribute(
        SdpAttribute::kGroupAttribute));
}

const SdpOptionsAttribute&
SipccSdpAttributeList::GetIceOptions() const {
  if (!HasAttribute(SdpAttribute::kIceOptionsAttribute)) {
    MOZ_CRASH();
  }

  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIceOptionsAttribute);
  return *static_cast<const SdpOptionsAttribute*>(attr);
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
  if (!HasAttribute(SdpAttribute::kIdentityAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kIdentityAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const SdpImageattrAttributeList&
SipccSdpAttributeList::GetImageattr() const {
  MOZ_CRASH("Not yet implemented.");
}

const std::string&
SipccSdpAttributeList::GetLabel() const {
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
  if (!HasAttribute(SdpAttribute::kMidAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMidAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
}

const SdpMsidAttributeList&
SipccSdpAttributeList::GetMsid() const {
  if (!HasAttribute(SdpAttribute::kMsidAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMsidAttribute);
  return *static_cast<const SdpMsidAttributeList*>(attr);
}

const std::string&
SipccSdpAttributeList::GetMsidSemantic() const {
  if (!HasAttribute(SdpAttribute::kMsidSemanticAttribute)) {
    return sEmptyString;
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kMsidSemanticAttribute);
  return static_cast<const SdpStringAttribute*>(attr)->GetValue();
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
  MOZ_CRASH("Not yet implemented");
}

const SdpRtcpFbAttributeList&
SipccSdpAttributeList::GetRtcpFb() const {
  if (!HasAttribute(SdpAttribute::kRtcpFbAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kRtcpFbAttribute);
  return *static_cast<const SdpRtcpFbAttributeList*>(attr);
}

const SdpRemoteCandidatesAttribute&
SipccSdpAttributeList::GetRemoteCandidates() const {
  MOZ_CRASH("Not yet implemented");
}

const SdpRtpmapAttributeList&
SipccSdpAttributeList::GetRtpmap() const {
  if (!HasAttribute(SdpAttribute::kRtpmapAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kRtpmapAttribute);
  return *static_cast<const SdpRtpmapAttributeList*>(attr);
}

const SdpSctpmapAttributeList&
SipccSdpAttributeList::GetSctpmap() const {
  if (!HasAttribute(SdpAttribute::kSctpmapAttribute)) {
    MOZ_CRASH();
  }
  const SdpAttribute* attr = GetAttribute(SdpAttribute::kSctpmapAttribute);
  return *static_cast<const SdpSctpmapAttributeList*>(attr);
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
  MOZ_CRASH("Not yet implemented");
}

const SdpSsrcGroupAttributeList&
SipccSdpAttributeList::GetSsrcGroup() const {
  MOZ_CRASH("Not yet implemented");
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
