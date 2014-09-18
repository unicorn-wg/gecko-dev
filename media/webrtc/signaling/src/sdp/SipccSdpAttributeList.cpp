/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

namespace mozilla {

unsigned int
SipccSdpAttributeList::CountAttributes(AttributeType type) const {
  return 0;
}

bool
SipccSdpAttributeList::HasAttribute(AttributeType type) const {
  return false;
}

const SdpAttribute&
SipccSdpAttributeList::GetAttribute(AttributeType type, size_t index) const {
  return nullptr;
}

const SdpCandidateAttribute&
SipccSdpAttributeList::GetCandidate() const {
  return nullptr;
}

const SdpConnectionAttribute&
SipccSdpAttributeList::GetConnection() const {
  return nullptr;
}

const SdpExtmapAttribute&
SipccSdpAttributeList::GetExtmap() const {
  return nullptr;
}

const SdpFingerprintAttribute&
SipccSdpAttributeList::GetFingerprint() const {
  return nullptr;
}

const SdpFmtpAttribute&
SipccSdpAttributeList::GetFmtp() const {
  return nullptr;
}

const SdpGroupAttribute&
SipccSdpAttributeList::GetGroup() const {
  return nullptr;
}

const SdpIceOptionsAttribute&
SipccSdpAttributeList::GetIceOptions() const {
  return nullptr;
}

std::string
SipccSdpAttributeList::GetIcePwd() const {
  return "";
}

std::string
SipccSdpAttributeList::GetIceUfrag() const {
  return "";
}

const SdpIdentityAttribute&
SipccSdpAttributeList::GetIdentity() const {
  return nullptr;
}

const SdpImageattrAttribute&
SipccSdpAttributeList::GetImageattr() const {
  return nullptr;
}

std::string
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

std::string
SipccSdpAttributeList::GetMid() const {
  return "";
}

const SdpMsidAttribute&
SipccSdpAttributeList::GetMsid() const {
  return nullptr;
}

uint32_t
SipccSdpAttributeList::GetPtime() const {
  return 0;
}

const SdpRtcpAttribute&
SipccSdpAttributeList::GetRtcp() const {
  return nullptr;
}

const SdpRtcpFbAttribute&
SipccSdpAttributeList::GetRtcpFb() const {
  return nullptr;
}

//const SdpRtcpRemoteCandidates&
//SipccSdpAttributeList::GetRemoteCandidates() const {
//  return nullptr;
//}

const SdpRtpmapAttribute&
SipccSdpAttributeList::GetRtpmap() const {
  return nullptr;
}

const SdpSctpmapAttribute&
SipccSdpAttributeList::GetSctpmap() const {
  return nullptr;
}

const SdpSetupAttribute&
SipccSdpAttributeList::GetSetup() const {
  return nullptr;
}

const SdpSsrcAttribute&
SipccSdpAttributeList::GetSsrc() const {
  return nullptr;
}

const SdpSsrcGroupAttribute&
SipccSdpAttributeList::GetSsrcGroup() const {
  return nullptr;
}

void
SipccSdpAttributeList::SetAttribute(const SdpAttribute &) {
}


} // namespace mozilla
