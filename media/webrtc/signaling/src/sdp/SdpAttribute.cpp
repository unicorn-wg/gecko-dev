/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SdpAttribute.h"

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
      << " " << i->fingerprint << CRLF;
  }
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
