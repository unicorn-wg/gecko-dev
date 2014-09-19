/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SdpAttribute.h"

#include <iomanip>

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
    if (i->direction != kNotSpecified) {
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
       << " " << FormatFingerprint(i->fingerprint) << CRLF;
  }
}

// Format the fingerprint in RFC 4572 Section 5 attribute format
std::string
SdpFingerprintAttributeList::FormatFingerprint(const std::vector<uint8_t>& fp) {
  std::ostringstream os;
  for (auto i = fp.begin(); i != fp.end(); ++i) {
    os << ":"
       << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
       << static_cast<uint32_t>(*i);
  }
  return os.str().substr(1);
}

// Parse the fingerprint from RFC 4572 Section 5 attribute format
std::vector<uint8_t>
SdpFingerprintAttributeList::ParseFingerprint(const std::string str) {
  size_t targetSize = (str.length() + 1) / 3;
  std::vector<uint8_t> fp(targetSize);
  bool top_half = true;
  uint8_t val = 0;
  size_t fpIndex = 0;

  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == ':') {
      if (!top_half) {
        fp.clear(); // error
        return fp;
      }
      continue;
    }

    if ((str[i] >= '0') && (str[i] <= '9')) {
      val |= str[i] - '0';
    } else if ((str[i] >= 'A') && (str[i] <= 'F')) {
      val |= str[i] - 'A' + 10;
    } else if ((str[i] >= 'a') && (str[i] <= 'f')) {
      val |= str[i] - 'a' + 10;
    }

    if (top_half) {
      val <<= 4;
      top_half = false;
    } else {
      fp[fpIndex++] = val;
      top_half = true;
      val = 0;
    }
  }

  if (targetSize != fpIndex) {
    fp.clear();
  }

  return fp;
}


void SdpFmtpAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mFmtps.begin(); i != mFmtps.end(); ++i) {
    os << "a=" << mType << ":" << i->format
      << " " << i->parameters << CRLF;
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

void SdpIceOptionsAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType;
  for (auto i = mOptions.begin(); i != mOptions.end(); i++) {
    os << (i == mOptions.begin() ? ":" : " ") << (*i);
  }
  os << CRLF;
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
  os << "a=" << mType << ":" << mIdentifier;
  if (mAppdata.length()) {
    os << " " << mAppdata;
  }
  os << CRLF;
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

void SdpRtcpFbAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mFeedback.begin(); i != mFeedback.end(); ++i) {
    os << "a=" << mType << ":" << i->pt << " " << i->type;
    if (i->parameters.length()) {
      os << " " << i->parameters;
    }
    os << CRLF;
  }
}

void SdpRtpmapAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mRtpmaps.begin(); i != mRtpmaps.end(); ++i) {
    os << "a=" << mType << ":" << i->pt << " " << i->name
      << "/" << i->clock;
    if (i->channels) {
      os << "/" << i->channels;
    }
    os << CRLF;
  }
}

void SdpSctpmapAttributeList::Serialize(std::ostream& os) const
{
  for (auto i = mSctpmaps.begin(); i != mSctpmaps.end(); ++i) {
    os << "a=" << mType << ":" << i->number << " " << i->app;
    if (i->maxMessageSize) {
      os << " max-message-size=" << i->maxMessageSize;
    }
    if (i->streams) {
      os << " streams=" << i->streams;
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

void SdpStringAttribute::Serialize(std::ostream& os) const {
    os << "a=" << mType << ":" << mValue << CRLF;
}

void SdpNumberAttribute::Serialize(std::ostream& os) const
{
  os << "a=" << mType << ":" << mValue << CRLF;
}

} // namespace mozilla
