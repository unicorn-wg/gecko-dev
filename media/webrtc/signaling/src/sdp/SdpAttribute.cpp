/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SdpAttribute.h"

#ifdef CRLF
#undef CRLF
#endif
#define CRLF "\r\n"

namespace mozilla {

void SdpConnectionAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << ":" << mValue << CRLF;
}

void SdpDirectionAttribute::Serialize(std::ostream& os) const {
  os << "a=" << GetTypeName() << CRLF;
}

void SdpExtmapAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mExtmaps.begin(); i != mExtmaps.end(); ++i) {
    os << "a=" << mType << ":" << i->entry;
    if (i->direction_specified) {
      os << "/" << i->direction;
    }
    os << " " << i->extensionname;
    if (i->extensionattributes.length()) {
      os << " " << i->extensionattributes;
    }
    os << CRLF;
  }
}

void SdpFingerprintAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mFingerprints.begin(); i != mFingerprints.end(); ++i) {
    os << "a=" << mType << ":" << i->hashFunc
      << " " << i->fingerprint << CRLF;
  }
}

void SdpFmtpAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mFmtps.begin(); i != mFmtps.end(); ++i) {
    os << "a=" << mType << ":" << i->format << " ";
    if (i->parameters) {
      i->parameters->Serialize(os);
    } else {
      os << i->parameters_string;
    }
    os << CRLF;
  }
}

void SdpGroupAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mGroups.begin(); i != mGroups.end(); ++i) {
    os << "a=" << mType << ":" << i->semantics;
    for (auto j = i->tags.begin(); j != i->tags.end(); ++j) {
      os << " " << (*j);
    }
    os << CRLF;
  }
}

void SdpIdentityAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << mAssertion;
  for (auto i = mExtensions.begin(); i != mExtensions.end(); i++) {
    os << (i == mExtensions.begin() ? " " : ";") << (*i);
  }
  os << CRLF;
}

void SdpImageattrAttributeList::Serialize(std::ostream& os) const
{
  MOZ_ASSERT(false, "Serializer not yet implemented");
}

void SdpMsidAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mMsids.begin(); i != mMsids.end(); ++i) {
    os << "a=" << mType << ":" << i->identifier;
    if (i->appdata.length()) {
      os << " " << i->appdata;
    }
    os << CRLF;
  }
}

void SdpRemoteCandidatesAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType;
  for (auto i = mCandidates.begin(); i != mCandidates.end(); i++) {
    os << (i == mCandidates.begin() ? ":" : " ") << i->id
      << " " << i->address
      << " " << i->port;
  }
  os << CRLF;
}

void SdpRtcpAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << ":" << mPort;
  if (mNetType != sdp::kNetTypeNone && mAddrType != sdp::kAddrTypeNone) {
    os << " " << mNetType << " " << mAddrType << " " << mAddress;
  }
  os << CRLF;
}

const char *SdpRtcpFbAttributeList::pli = "pli";
const char *SdpRtcpFbAttributeList::sli = "sli";
const char *SdpRtcpFbAttributeList::rpsi = "rpsi";
const char *SdpRtcpFbAttributeList::app = "app";

const char *SdpRtcpFbAttributeList::fir = "fir";
const char *SdpRtcpFbAttributeList::tmmbr = "tmmbr";
const char *SdpRtcpFbAttributeList::tstr = "tstr";
const char *SdpRtcpFbAttributeList::vbcm = "vbcm";

void SdpRtcpFbAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mFeedbacks.begin(); i != mFeedbacks.end(); ++i) {
    os << "a=" << mType << ":" << i->pt << " " << i->type;
    if (i->parameter.length()) {
      os << " " << i->parameter;
      if (i->extra.length()) {
        os << " " << i->extra;
      }
    }
    os << CRLF;
  }
}

static bool ShouldSerializeChannels(SdpRtpmapAttributeList::CodecType type) {
  switch (type) {
    case SdpRtpmapAttributeList::kOpus:
    case SdpRtpmapAttributeList::kG722:
      return true;
    case SdpRtpmapAttributeList::kPCMU:
    case SdpRtpmapAttributeList::kPCMA:
    case SdpRtpmapAttributeList::kVP8:
    case SdpRtpmapAttributeList::kH264:
      return false;
    case SdpRtpmapAttributeList::kOtherCodec:
      return true;
  }
}

void SdpRtpmapAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mRtpmaps.begin(); i != mRtpmaps.end(); ++i) {
    os << "a=" << mType << ":" << i->pt << " " << i->name
      << "/" << i->clock;
    if (i->channels && ShouldSerializeChannels(i->codec)) {
      os << "/" << i->channels;
    }
    os << CRLF;
  }
}

void SdpSctpmapAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mSctpmaps.begin(); i != mSctpmaps.end(); ++i) {
    os << "a=" << mType << ":" << i->pt << " " << i->name;
    if (i->streams) {
      os << " " << i->streams;
    }
    os << CRLF;
  }
}

void SdpSetupAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << ":" << mRole << CRLF;
}

void SdpSsrcAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mSsrcs.begin(); i != mSsrcs.end(); ++i) {
    os << "a=" << mType << ":" << i->ssrc << " " << i->attribute << CRLF;
  }
}

void SdpSsrcGroupAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mSsrcGroups.begin(); i != mSsrcGroups.end(); ++i) {
    os << "a=" << mType << ":" << i->semantics;
    for (auto j = i->ssrcs.begin(); j != i->ssrcs.end(); ++j) {
      os << " " << (*j);
    }
    os << CRLF;
  }
}

void SdpMultiStringAttribute::Serialize(std::ostream& os) const {
  for (auto i = mValues.begin(); i != mValues.end(); ++i) {
    os << "a=" << mType << ":" << *i << CRLF;
  }
}

void SdpOptionsAttribute::Serialize(std::ostream& os) const {
  os << "a=" << mType << ":";
  bool first = true;
  for (auto i = mValues.begin(); i != mValues.end(); ++i) {
    if (!first) {
      os << " ";
    } else {
      first = false;
    }
    os << *i;
  }
  os << CRLF;
}

void SdpOptionsAttribute::Load(const std::string& value) {
  size_t start = 0;
  size_t end = value.find(' ');
  while (end != std::string::npos) {
    PushEntry(value.substr(start, end));
    start = end + 1;
    end = value.find(' ', start);
  }
  PushEntry(value.substr(start));
}

void SdpFlagAttribute::Serialize(std::ostream& os) const {
  os << "a=" << mType << CRLF;
}

void SdpStringAttribute::Serialize(std::ostream& os) const {
    os << "a=" << mType << ":" << mValue << CRLF;
}

void SdpNumberAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << ":" << mValue << CRLF;
}

bool SdpAttribute::IsAllowedAtMediaLevel(AttributeType type) {
  switch (type) {
    case kBundleOnlyAttribute: return true;
    case kCandidateAttribute: return true;
    case kConnectionAttribute: return true;
    case kDirectionAttribute: return true;
    case kEndOfCandidatesAttribute: return true;
    case kExtmapAttribute: return true;
    case kFingerprintAttribute: return true;
    case kFmtpAttribute: return true;
    case kGroupAttribute: return false;
    case kIceLiteAttribute: return false;
    case kIceMismatchAttribute: return true;
    case kIceOptionsAttribute: return false;
    case kIcePwdAttribute: return true;
    case kIceUfragAttribute: return true;
    case kIdentityAttribute: return false;
    case kImageattrAttribute: return true;
    case kInactiveAttribute: return true;
    case kLabelAttribute: return true;
    case kMaxptimeAttribute: return true;
    case kMidAttribute: return true;
    case kMsidAttribute: return true;
    case kPtimeAttribute: return true;
    case kRecvonlyAttribute: return true;
    case kRemoteCandidatesAttribute: return true;
    case kRtcpAttribute: return true;
    case kRtcpFbAttribute: return true;
    case kRtcpMuxAttribute: return true;
    case kRtcpRsizeAttribute: return true;
    case kRtpmapAttribute: return true;
    case kSctpmapAttribute: return true;
    case kSendonlyAttribute: return true;
    case kSendrecvAttribute: return true;
    case kSetupAttribute: return true;
    case kSsrcAttribute: return true;
    case kSsrcGroupAttribute: return true;
    default: return true;
  }
}

bool SdpAttribute::IsAllowedAtSessionLevel(AttributeType type) {
  switch (type) {
    case kBundleOnlyAttribute: return false;
    case kCandidateAttribute: return false;
    case kConnectionAttribute: return true;
    case kDirectionAttribute: return true;
    case kEndOfCandidatesAttribute: return true;
    case kExtmapAttribute: return true;
    case kFingerprintAttribute: return true;
    case kFmtpAttribute: return false;
    case kGroupAttribute: return true;
    case kIceLiteAttribute: return true;
    case kIceMismatchAttribute: return false;
    case kIceOptionsAttribute: return true;
    case kIcePwdAttribute: return true;
    case kIceUfragAttribute: return true;
    case kIdentityAttribute: return true;
    case kImageattrAttribute: return false;
    case kInactiveAttribute: return true;
    case kLabelAttribute: return false;
    case kMaxptimeAttribute: return false;
    case kMidAttribute: return false;
    case kMsidAttribute: return false;
    case kPtimeAttribute: return false;
    case kRecvonlyAttribute: return true;
    case kRemoteCandidatesAttribute: return false;
    case kRtcpAttribute: return false;
    case kRtcpFbAttribute: return false;
    case kRtcpMuxAttribute: return false;
    case kRtcpRsizeAttribute: return false;
    case kRtpmapAttribute: return false;
    case kSctpmapAttribute: return false;
    case kSendonlyAttribute: return true;
    case kSendrecvAttribute: return true;
    case kSetupAttribute: return true;
    case kSsrcAttribute: return false;
    case kSsrcGroupAttribute: return false;
    default: return true;
  }
}


} // namespace mozilla
