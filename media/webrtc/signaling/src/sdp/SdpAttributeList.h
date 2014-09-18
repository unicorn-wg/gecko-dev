/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPATTRIBUTELIST_H_
#define _SDPATTRIBUTELIST_H_

#include "mozilla/UniquePtr.h"
#include "mozilla/Attributes.h"

#include "signaling/src/sdp/SdpEnum.h"
#include "signaling/src/sdp/SdpAttribute.h"

namespace mozilla {

class SdpAttributeList
{
public:
  typedef SdpAttribute::AttributeType AttributeType;

  virtual size_t CountAttributes(AttributeType type) const = 0;
  virtual bool HasAttribute(AttributeType type) const = 0;
  virtual const SdpAttribute& GetAttribute(AttributeType type, size_t index = 0) const = 0;

  virtual const SdpConnectionAttribute& GetConnection() const = 0;
  virtual const SdpFingerprintAttribute& GetFingerprint() const = 0;
  virtual const SdpGroupAttribute& GetGroup() const = 0;
  virtual const SdpIceOptionsAttribute& GetIceOptions() const = 0;
  virtual const SdpIdentityAttribute& GetIdentity() const = 0;
  virtual const SdpMsidAttribute& GetMsid() const = 0;
  virtual const SdpRtcpAttribute& GetRtcp() const = 0;
  virtual const SdpRemoteCandidatesAttribute& GetRemoteCandidates() const = 0;
  virtual const SdpSetupAttribute& GetSetup() const = 0;
  virtual const SdpSsrcAttribute& GetSsrc() const = 0;
  virtual const SdpSsrcGroupAttribute& GetSsrcGroup() const = 0;

  // These attributes can appear multiple times, so the returned
  // classes actually represent a collection of values.
  virtual const SdpCandidateAttributeList& GetCandidate() const = 0;
  virtual const SdpExtmapAttributeList& GetExtmap() const = 0;
  virtual const SdpFmtpAttributeList& GetFmtp() const = 0;
  virtual const SdpImageattrAttributeList& GetImageattr() const = 0;
  virtual const SdpRtcpFbAttributeList& GetRtcpFb() const = 0;
  virtual const SdpRtpmapAttributeList& GetRtpmap() const = 0;
  virtual const SdpSctpmapAttributeList& GetSctpmap() const = 0;

  // These attributes are effectively simple types, so we'll make life
  // easy by just returning their value.
  virtual const std::string& GetIcePwd() const = 0;
  virtual const std::string& GetIceUfrag() const = 0;
  virtual const std::string& GetLabel() const = 0;
  virtual unsigned int GetMaxprate() const = 0;
  virtual unsigned int GetMaxptime() const = 0;
  virtual const std::string& GetMid() const = 0;
  virtual unsigned int GetPtime() const = 0;

  // The setter takes an attribute of any type, and Does The Right Thing™
  virtual void SetAttribute(const SdpAttribute &) = 0;
};

}

#endif
