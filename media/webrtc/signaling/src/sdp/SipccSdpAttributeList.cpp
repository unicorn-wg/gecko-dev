/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

#include "mozilla/Assertions.h"

namespace mozilla {

size_t
SipccSdpAttributeList::CountAttributes(AttributeType type) const {
  return 0;
}

bool
SipccSdpAttributeList::HasAttribute(AttributeType type) const {
  return false;
}

const SdpAttribute&
SipccSdpAttributeList::GetAttribute(AttributeType type, size_t index) const {
  return Nothing();
}

const SdpCandidateAttribute&
SipccSdpAttributeList::GetCandidate() const {
  MOZ_CRASH();
}

const SdpConnectionAttribute&
SipccSdpAttributeList::GetConnection() const {
  MOZ_CRASH();;
}

const SdpExtmapAttribute&
SipccSdpAttributeList::GetExtmap() const {
  MOZ_CRASH();
}

const SdpFingerprintAttribute&
SipccSdpAttributeList::GetFingerprint() const {
  MOZ_CRASH();
}

const SdpFmtpAttribute&
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
  return "";
}

const std::string&
SipccSdpAttributeList::GetIceUfrag() const {
  return "";
}

const SdpIdentityAttribute&
SipccSdpAttributeList::GetIdentity() const {
  MOZ_CRASH();;
}

const SdpImageattrAttribute&
SipccSdpAttributeList::GetImageattr() const {
  MOZ_CRASH();;
}

const std::string&
SipccSdpAttributeList::GetLabel() const {
  return "";
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
  return "";
}

const SdpMsidAttribute&
SipccSdpAttributeList::GetMsid() const {
  MOZ_CRASH();;
}

uint32_t
SipccSdpAttributeList::GetPtime() const {
  return 0;
}

const SdpRtcpAttribute&
SipccSdpAttributeList::GetRtcp() const {
  MOZ_CRASH();;
}

const SdpRtcpFbAttribute&
SipccSdpAttributeList::GetRtcpFb() const {
  MOZ_CRASH();;
}

const SdpRemoteCandidatesAttribute&
SipccSdpAttributeList::GetRemoteCandidates() const {
  MOZ_CRASH();;
}

const SdpRtpmapAttribute&
SipccSdpAttributeList::GetRtpmap() const {
  MOZ_CRASH();;
}

const SdpSctpmapAttribute&
SipccSdpAttributeList::GetSctpmap() const {
  MOZ_CRASH();;
}

const SdpSetupAttribute&
SipccSdpAttributeList::GetSetup() const {
  MOZ_CRASH();;
}

const SdpSsrcAttribute&
SipccSdpAttributeList::GetSsrc() const {
  MOZ_CRASH();;
}

const SdpSsrcGroupAttribute&
SipccSdpAttributeList::GetSsrcGroup() const {
  MOZ_CRASH();;
}

void
SipccSdpAttributeList::SetAttribute(const SdpAttribute &) {
}


} // namespace mozilla
