/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdp.h"

#include <cstdlib>
#include "mozilla/Assertions.h"
#include "signaling/src/sdp/SdpErrorHolder.h"

namespace mozilla {

SipccSdp::~SipccSdp() {
  for (auto i = mMediaSections.begin(); i != mMediaSections.end(); ++i) {
    delete *i;
  }
}

const SdpOrigin& SipccSdp::GetOrigin() const {
  return *mOrigin;
}

const std::string&
SipccSdp::GetBandwidth(const std::string& type) const {
  static std::string emptyString("");
  auto found = mBandwidths.find(type);
  if (found == mBandwidths.end()) {
    return emptyString;
  }
  return found->second;
}

const SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level) const
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return *mMediaSections[level];
}

SdpMediaSection&
SipccSdp::GetMediaSection(uint16_t level)
{
  if (level > mMediaSections.size()) {
    MOZ_CRASH();
  }
  return *mMediaSections[level];
}

SdpMediaSection &
SipccSdp::AddMediaSection(SdpMediaSection::MediaType mediaType, uint16_t port,
                          SdpMediaSection::Protocol protocol,
                          sdp::AddrType addrType, const std::string &addr,
                          SdpDirectionAttribute::Direction dir) {
  SipccSdpMediaSection *media = new SipccSdpMediaSection(&mAttributeList);
  media->mMediaType = mediaType;
  media->mPort = port;
  media->mPortCount = 0;
  media->mProtocol = protocol;
  media->mConnection = MakeUnique<SdpConnection>(addrType, addr);
  media->GetAttributeList().SetAttribute(new SdpDirectionAttribute(dir));
  mMediaSections.push_back(media);
  return *media;
}

bool
SipccSdp::LoadOrigin(sdp_t* sdp, SdpErrorHolder& errorHolder) {
  std::string username = sdp_get_owner_username(sdp);
  uint64_t sessId = strtoul(sdp_get_owner_sessionid(sdp), nullptr, 10);
  uint64_t sessVer = strtoul(sdp_get_owner_version(sdp), nullptr, 10);

  sdp_nettype_e type = sdp_get_owner_network_type(sdp);
  if (type != SDP_NT_INTERNET) {
    errorHolder.AddParseError(0, "Unsupported network type");
    return false;
  }

  sdp::AddrType addrType;
  switch (sdp_get_owner_address_type(sdp)) {
    case SDP_AT_IP4:
      addrType = sdp::kIPv4;
      break;
    case SDP_AT_IP6:
      addrType = sdp::kIPv6;
      break;
    default:
      errorHolder.AddParseError(0, "Unsupported address type");
      return false;
  }

  std::string address = sdp_get_owner_address(sdp);
  mOrigin = MakeUnique<SdpOrigin>(username, sessId, sessVer,
                                  addrType, address);
  return true;
}

bool
SipccSdp::Load(sdp_t* sdp, SdpErrorHolder& errorHolder) {
  // Believe it or not, SDP_SESSION_LEVEL is 0xFFFF
  if (!mAttributeList.Load(sdp, SDP_SESSION_LEVEL, errorHolder)) {
    return false;
  }

  if (!LoadOrigin(sdp, errorHolder)) {
    return false;
  }

  for (int i = 0; i < sdp_get_num_media_lines(sdp); ++i) {
    // note that we pass a "level" here that is one higher
    // sipcc counts media sections from 1, using 0 as the "session"
    SipccSdpMediaSection* section = new SipccSdpMediaSection(&mAttributeList);
    if (!section->Load(sdp, i + 1, errorHolder)) {
      return false;
    }
    mMediaSections.push_back(section);
  }
  return true;
}

void
SipccSdp::Serialize(std::ostream& os) const {
  os << "v=0" << CRLF
     << mOrigin << CRLF
     << "s=-" << CRLF;

  // We don't support creating i=, u=, e=, p=
  // We don't generate c= at the session level (only in media)

  for (auto i = mBandwidths.begin(); i != mBandwidths.end(); ++i) {
    os << "b=" << i->first << ":" << i->second << CRLF;
  }
  os << "t=0 0" << CRLF;

  // We don't support r= or z=

  // attributes
  os << mAttributeList;

  // media sections
  for (auto i = mMediaSections.begin(); i != mMediaSections.end(); ++i) {
    os << (*i);
  }

}

} // namespace mozilla
