/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpMediaSection.h"

#include <ostream>
#include "signaling/src/sdp/SdpErrorHolder.h"

namespace mozilla {

unsigned int
SipccSdpMediaSection::GetPort() const {
  return mPort;
}

unsigned int
SipccSdpMediaSection::GetPortCount() const {
  return mPortCount;
}

SdpMediaSection::Protocol
SipccSdpMediaSection::GetProtocol() const {
  return mProtocol;
}

const SdpConnection&
SipccSdpMediaSection::GetConnection() const {
  return *mConnection;
}

const std::string&
SipccSdpMediaSection::GetBandwidth(const std::string& type) const {
  static std::string emptyString("");
  auto found = mBandwidths.find(type);
  if (found == mBandwidths.end()) {
    return emptyString;
  }
  return found->second;
}

const std::vector<std::string>&
SipccSdpMediaSection::GetFormats() const {
  return mFormats;
}

const SdpAttributeList&
SipccSdpMediaSection::GetAttributeList() const {
  return mAttributeList;
}

SdpAttributeList&
SipccSdpMediaSection::GetAttributeList() {
  return mAttributeList;
}

SdpDirectionAttribute
SipccSdpMediaSection::GetDirectionAttribute() const {
  return mAttributeList.GetDirection();
}

bool
SipccSdpMediaSection::Load(sdp_t* sdp, uint16_t level,
                           SdpErrorHolder& errorHolder)
{
  switch (sdp_get_media_type(sdp, level)) {
  case SDP_MEDIA_AUDIO: mMediaType = kAudio; break;
  case SDP_MEDIA_VIDEO: mMediaType = kVideo; break;
  case SDP_MEDIA_APPLICATION: mMediaType = kApplication; break;
  case SDP_MEDIA_TEXT: mMediaType = kText; break;

  default:
    errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                              "Unsupported media section type");
    return false;
  }

  mPort = sdp_get_media_portnum(sdp, level);
  int32 pc =  sdp_get_media_portcount(sdp, level);
  if (pc == SDP_INVALID_VALUE) {
    mPortCount = 0;
  } else {
    // >16bit port numbers will cause...
    mPortCount = pc;
  }
  if (!LoadProtocol(sdp, level, errorHolder)) {
    return false;
  }
  LoadFormats(sdp, level);

  if (!mAttributeList.Load(sdp, level, errorHolder)) {
    return false;
  }

  return LoadConnection(sdp, level, errorHolder);
}

bool SipccSdpMediaSection::LoadProtocol(sdp_t* sdp, uint16_t level,
                                        SdpErrorHolder& errorHolder) {
  switch (sdp_get_media_transport(sdp, level)) {
    case SDP_TRANSPORT_RTPAVP: mProtocol = kRtpAvp; break;
    case SDP_TRANSPORT_RTPSAVP: mProtocol = kRtpSavp; break;
    case SDP_TRANSPORT_RTPAVPF: mProtocol = kRtpAvpf; break;
    case SDP_TRANSPORT_RTPSAVPF: mProtocol = kRtpSavpf; break;
    case SDP_TRANSPORT_UDPTLSRTPSAVP: mProtocol = kUdpTlsRtpSavp; break;
    case SDP_TRANSPORT_UDPTLSRTPSAVPF: mProtocol = kUdpTlsRtpSavpf; break;
    case SDP_TRANSPORT_TCPTLSRTPSAVP: mProtocol = kTcpTlsRtpSavp; break;
    case SDP_TRANSPORT_TCPTLSRTPSAVPF: mProtocol = kTcpTlsRtpSavpf; break;

    default:
      errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                                "Unsupported media transport type");
      return false;
  }
  return true;
}

void
SipccSdpMediaSection::LoadFormats(sdp_t* sdp, uint16_t level) {
  uint16_t count = sdp_get_media_num_payload_types(sdp, level);
  for (uint16_t i = 0; i < count; ++i) {
    sdp_payload_ind_e indicator; // we ignore this, which might be bad
    uint32_t ptype = sdp_get_media_payload_type(sdp, level, i + 1, &indicator);

    std::ostringstream ospt;
    ospt << ((ptype & 0xff00) ? ((ptype >> 8) & 0xff) : ptype); // OMFG
    mFormats.push_back(ospt.str());
  }
}

bool
SipccSdpMediaSection::LoadConnection(sdp_t* sdp, uint16_t level,
                                     SdpErrorHolder& errorHolder) {
  if (!sdp_connection_valid(sdp, level)) {
    level = SDP_SESSION_LEVEL;
    if (!sdp_connection_valid(sdp, level)) {
      errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                                "Missing c= line");
      return false;
    }
  }

  sdp_nettype_e type = sdp_get_conn_nettype(sdp, level);
  if (type != SDP_NT_INTERNET) {
    errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                              "Unsupported network type");
    return false;
  }

  sdp::AddrType addrType;
  switch (sdp_get_conn_addrtype(sdp, level)) {
    case SDP_AT_IP4:
      addrType = sdp::kIPv4;
      break;
    case SDP_AT_IP6:
      addrType = sdp::kIPv6;
      break;
    default:
      errorHolder.AddParseError(sdp_get_media_line_number(sdp, level),
                                "Unsupported address type");
      return false;
  }

  std::string address = sdp_get_conn_address(sdp, level);
  int16_t ttl = static_cast<uint16_t>(sdp_get_mcast_ttl(sdp, level));
  if (ttl < 0) {
    ttl = 0;
  }
  int32_t numAddr = static_cast<uint32_t>(sdp_get_mcast_num_of_addresses(sdp, level));
  if (numAddr < 0) {
    numAddr = 0;
  }
  mConnection = MakeUnique<SdpConnection>(addrType, address, ttl, numAddr);
  return true;
}

void
SipccSdpMediaSection::AddCodec(uint8_t ptv, const std::string& name,
                               uint32_t clockrate, uint16_t channels,
                               const std::string& fmtp) {
  std::ostringstream os;
  os << static_cast<uint32_t>(ptv);
  std::string pt = os.str();

  mFormats.push_back(pt);

  SdpRtpmapAttributeList *rtpmap = new SdpRtpmapAttributeList();
  if (mAttributeList.HasAttribute(SdpAttribute::kRtpmapAttribute)) {
    const SdpRtpmapAttributeList& old = mAttributeList.GetRtpmap();
    for (auto it = old.mRtpmaps.begin(); it != old.mRtpmaps.end(); ++it) {
      rtpmap->mRtpmaps.push_back(*it);
    }
  }
  rtpmap->PushEntry(pt, name, clockrate, channels);
  mAttributeList.SetAttribute(rtpmap);

  if (!fmtp.empty()) {
    SdpFmtpAttributeList *fmtps = new SdpFmtpAttributeList();
    if (mAttributeList.HasAttribute(SdpAttribute::kFmtpAttribute)) {
      const SdpFmtpAttributeList& old = mAttributeList.GetFmtp();
      for (auto it = old.mFmtps.begin(); it != old.mFmtps.end(); ++it) {
        fmtps->mFmtps.push_back(*it);
      }
    }
    fmtps->PushEntry(pt, fmtp);
    mAttributeList.SetAttribute(fmtps);
  }
}

void SipccSdpMediaSection::AddDataChannel(uint16_t pt,
                                          const std::string& sctpmap) {
  MOZ_CRASH();
}


void
SipccSdpMediaSection::Serialize(std::ostream& os) const {
  os << "m="
     << mMediaType << " "
     << mPort;
  if (mPortCount) {
    os << "/" << mPortCount;
  }
  os << " " << mProtocol;
  for (auto i = mFormats.begin(); i != mFormats.end(); ++i) {
    os << " " << (*i);
  }
  os << CRLF;

  // We dont do i=

  if (mConnection) {
    os << *mConnection;
  }

  for (auto i = mBandwidths.begin(); i != mBandwidths.end(); ++i) {
    os << "b=" << i->first << ":" << i->second << CRLF;
  }

  // We dont do k= because they're evil

  os << mAttributeList;
}

}
