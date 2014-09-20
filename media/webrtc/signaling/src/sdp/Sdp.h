/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

/*

         ,-----.                  ,--.  ,--.
        '  .--./ ,--,--.,--.,--.,-'  '-.`--' ,---. ,--,--,
        |  |    ' ,-.  ||  ||  |'-.  .-',--.| .-. ||      `
        '  '--'\\ '-'  |'  ''  '  |  |  |  |' '-' '|  ||  |
         `-----' `--`--' `----'   `--'  `--' `---' `--''--'

                               .MMM+                 ,MMO
                               ZMMMM.               .MMMM~
                             .ZMMDMMM.              .DMMM
                             ,MMM  MMM.              ON?.
                            .MMM.  .MMM.            .MMMMM .
                           .MMM.     MMM,.          NMMMMMM:
                          .MMM        MMM,         .MMMMM$MM,
                         .MMM,         MMM.        $MMMMMM:MM
                        .MMM~  7M.     .MMM. ....:7MMMMMMM.MM.
                        MMM+   MMM     ..+NMMMMMMMMMMMM.MM..MO.
                       IMM$    :MM.  IMMMMMMMMMMMMMMMMM.MM..MM.
                      IMMZ      MMD   MMNZ~.~I.    .MMM.:$.
                     ,MMN      ,MMD        ,MMM.   .MMM.
                    ,MMM.+MD. ,MMM          ,MMM.  .MMM.
                   .MMM 7MMMM.MMN.           ~MMM.  MMM:
                  .MMM  .MMMIZDMM      . +?   ,MMM. ZMMM.
                  MMM      .8MMMMMM. .?MMMMM   .MMM .MMM:
                 8N8.      :8NM8MNNMM8MDDMDN=   7MMM..MMD
                MOM.       ~DM88NMMMMMNZ. 8NM    ?MMM MDD,
               8MM~        7NN  ZMMMMN    .MMM    ?MMM MM.
             .$MMN         $N$.   IM88M~   ,MM     ?MM8.
             OMMM          .DMM$    +MMMN.          ~MMN
            IMMM              NMM8    $MMI           =MMN
           .MMM                .I      MMM.           DMMD
          .MMM                          MM~            8MMM.
         .MMM.                          +MM             OMMN.
        .MMM                             ,,              8MMI
        MMMNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNMMM7
       ,MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

            ,--------.,--.     ,--.           ,--.
            '--.  .--'|  ,---. `--' ,---.     |  | ,---.
               |  |   |  .-.  |,--.(  .-'     |  |(  .-'
               |  |   |  | |  ||  |.-'  `)    |  |.-'  `)
               `--'   `--' `--'`--'`----'     `--'`----'
                                                                ,--.
       ,---.  ,------.  ,------.                  ,--.          |  |
      '   .-' |  .-.  \ |  .--. ' ,--,--.,--.--.,-'  '-. ,--,--.|  |
      `.  `-. |  |  \  :|  '--' |' ,-.  ||  .--''-.  .-'' ,-.  ||  |
      .-'    ||  '--'  /|  | --' \ '-'  ||  |     |  |  \ '-'  |`--'
      `-----' `-------' `--'      `--`--'`--'     `--'   `--`--'.--.
                                                                '__'
*/

#ifndef _SDP_H_
#define _SDP_H_

#include <ostream>
#include <vector>
#include <sstream>
#include "mozilla/UniquePtr.h"
#include "mozilla/Maybe.h"
#include "signaling/src/sdp/SdpMediaSection.h"
#include "signaling/src/sdp/SdpAttributeList.h"
#include "signaling/src/sdp/SdpEnum.h"

namespace mozilla {

class SdpOrigin;
class SdpEncryptionKey;

class SdpMediaSection;

class Sdp
{
public:
  Sdp() {};

  virtual const SdpOrigin& GetOrigin() const = 0;
  // Note: connection information is always retrieved from media sections
  virtual uint32_t GetBandwidth(const std::string& type) const = 0;

  virtual const SdpAttributeList &GetAttributeList() const = 0;
  virtual SdpAttributeList &GetAttributeList() = 0;

  virtual uint16_t GetMediaSectionCount() const = 0;
  virtual const SdpMediaSection &GetMediaSection(uint16_t level) const = 0;
  virtual SdpMediaSection &GetMediaSection(uint16_t level) = 0;

  virtual SdpMediaSection &AddMediaSection(
      SdpMediaSection::MediaType media = SdpMediaSection::kAudio,
      SdpDirectionAttribute::Direction dir = SdpDirectionAttribute::kSendrecv,
      uint16_t port = 9,
      SdpMediaSection::Protocol proto = SdpMediaSection::kUdpTlsRtpSavpf,
      sdp::AddrType addrType = sdp::kIPv4,
      const std::string &addr = "0.0.0.0") = 0;

  virtual void Serialize(std::ostream&) const = 0;

  std::string toString();

  virtual ~Sdp() {};
};

inline std::ostream& operator <<(std::ostream& os, const Sdp &sdp)
{
  sdp.Serialize(os);
  return os;
}

inline std::string Sdp::toString() {
  std::stringstream s;
  s << *this;
  return s.str();
}


class SdpOrigin
{
public:
  SdpOrigin(const std::string& username, uint64_t sessId, uint64_t sessVer,
            sdp::AddrType addrType, const std::string& addr) :
      mUsername(username),
      mSessionId(sessId),
      mSessionVersion(sessVer),
      mAddrType(addrType),
      mAddress(addr)
  {}

  const std::string& GetUsername() const {
    return mUsername;
  }

  uint64_t GetSessionId() const {
    return mSessionId;
  }

  uint64_t GetSessionVersion() const {
    return mSessionVersion;
  }

  const sdp::AddrType GetAddrType() const {
    return mAddrType;
  }

  const std::string& GetAddress() const {
    return mAddress;
  }

  void Serialize(std::ostream& os) const {
    sdp::NetType netType = sdp::kInternet;
    os << "o="
       << mUsername << " "
       << mSessionId << " "
       << mSessionVersion << " "
       << netType << " "
       << mAddrType << " "
       << mAddress << CRLF;
  }

private:
  std::string mUsername;
  uint64_t mSessionId;
  uint64_t mSessionVersion;
  sdp::AddrType mAddrType;
  std::string mAddress;
};

inline std::ostream& operator <<(std::ostream& os, const SdpOrigin &origin)
{
  origin.Serialize(os);
  return os;
}

} // namespace mozilla

#endif
