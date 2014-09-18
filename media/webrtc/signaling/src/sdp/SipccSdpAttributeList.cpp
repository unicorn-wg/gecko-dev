/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpAttributeList.h"

namespace mozilla {

unsigned int
SipccSdpAttributeList::CountAttributes(sdp::AttributeType type) const {
  return 0;
}

bool
SipccSdpAttributeList::HasAttribute(sdp::AttributeType type) const {
  return false;
}

UniquePtr<SdpAttribute>
SipccSdpAttributeList::GetAttribute(sdp::AttributeType type) const {
  return nullptr;
}

UniquePtr<SdpCandidateAttribute>
SipccSdpAttributeList::GetCandidate() const {
  return nullptr;
}

UniquePtr<SdpConnectionAttribute>
SipccSdpAttributeList::GetConnection() const {
  return nullptr;
}

UniquePtr<SdpExtmapAttribute>
SipccSdpAttributeList::GetExtmap() const {
  return nullptr;
}

UniquePtr<SdpFingerprintAttribute>
SipccSdpAttributeList::GetFingerprint() const {
  return nullptr;
}

UniquePtr<SdpFmtpAttribute>
SipccSdpAttributeList::GetFmtp() const {
  return nullptr;
}

UniquePtr<SdpGroupAttribute>
SipccSdpAttributeList::GetGroup() const {
  return nullptr;
}

UniquePtr<SdpIceOptionsAttribute>
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

UniquePtr<SdpIdentityAttribute>
SipccSdpAttributeList::GetIdentity() const {
  return nullptr;
}

UniquePtr<SdpImageattrAttribute>
SipccSdpAttributeList::GetImageattr() const {
  return nullptr;
}

std::string
SipccSdpAttributeList::GetLabel() const {
  return "";
}

unsigned int
SipccSdpAttributeList::GetMaxprate() const {
  return 0;
}

unsigned int
SipccSdpAttributeList::GetMaxptime() const {
  return 0;
}

std::string
SipccSdpAttributeList::GetMid() const {
  return "";
}

UniquePtr<SdpMsidAttribute>
SipccSdpAttributeList::GetMsid() const {
  return nullptr;
}

unsigned int
SipccSdpAttributeList::GetPtime() const {
  return 0;
}

UniquePtr<SdpRtcpAttribute>
SipccSdpAttributeList::GetRtcp() const {
  return nullptr;
}

UniquePtr<SdpRtcpFbAttribute>
SipccSdpAttributeList::GetRtcpFb() const {
  return nullptr;
}

//UniquePtr<SdpRtcpRemoteCandidates>
//SipccSdpAttributeList::GetRemoteCandidates() const {
//  return nullptr;
//}

UniquePtr<SdpRtpmapAttribute>
SipccSdpAttributeList::GetRtpmap() const {
  return nullptr;
}

UniquePtr<SdpSctpmapAttribute>
SipccSdpAttributeList::GetSctpmap() const {
  return nullptr;
}

UniquePtr<SdpSetupAttribute>
SipccSdpAttributeList::GetSetup() const {
  return nullptr;
}

UniquePtr<SdpSsrcAttribute>
SipccSdpAttributeList::GetSsrc() const {
  return nullptr;
}

UniquePtr<SdpSsrcGroupAttribute>
SipccSdpAttributeList::GetSsrcGroup() const {
  return nullptr;
}

void
SipccSdpAttributeList::SetAttribute(const SdpAttribute &) {
}


} // namespace mozilla

