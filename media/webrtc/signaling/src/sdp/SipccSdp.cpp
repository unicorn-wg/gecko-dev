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

  SdpConnection::AddrType addrType;
  switch (sdp_get_owner_address_type(sdp)) {
    case SDP_AT_IP4:
      addrType = SdpConnection::kIPv4;
      break;
    case SDP_AT_IP6:
      addrType = SdpConnection::kIPv6;
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
  os << "THIS IS SPARTA\r\n";
  MOZ_CRASH();
}

} // namespace mozilla
