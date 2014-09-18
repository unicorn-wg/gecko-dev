/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPATTRIBUTE_H_
#define _SDPATTRIBUTE_H_

#include "mozilla/UniquePtr.h"
#include "mozilla/Attributes.h"

#include "signaling/src/sdp/SdpEnum.h"

namespace mozilla {

class SdpAttribute
{
public:
  enum AttributeType {
    kBundleOnlyAttribute,
    kCandidateAttribute,
    kConnectionAttribute,
    kDtlsFingerprintAttribute,
    kExtmapAttribute,
    kFingerprintAttribute,
    kFmtpAttribute,
    kGroupAttribute,
    kIceLiteAttribute,
    kIceMismatchAttribute,
    kIceOptionsAttribute,
    kIcePwdAttribute,
    kIceUfragAttribute,
    kIdentityAttribute,
    kImageattrAttribute,
    kInactiveAttribute,
    kLabelAttribute,
    kMaxprateAttribute,
    kMaxptimeAttribute,
    kMidAttribute,
    kMsidAttribute,
    kPtimeAttribute,
    kRecvonlyAttribute,
    kRemoteCandidatesAttribute,
    kRtcpAttribute,
    kRtcpFbAttribute,
    kRtcpMuxAttribute,
    kRtcpRsizeAttribute,
    kRtpmapAttribute,
    kSctpmapAttribute,
    kSendonlyAttribute,
    kSendrecvAttribute,
    kSetupAttribute,
    kSsrcAttribute,
    kSsrcGroupAttribute,
    kOtherAttribute
  };

  SdpAttribute(AttributeType type, const std::string& typeName)
      : mType(type), mTypeName(typeName) {}
  virtual ~SdpAttribute() {}

  virtual AttributeType GetType()
  {
    return mType;
  }

  virtual const std::string &GetTypeName()
  {
    return mTypeName;
  }

private:
  AttributeType mType;
  std::string mTypeName;
};


// RFC5245
// candidate-attribute   = "candidate" ":" foundation SP component-id SP
//                          transport SP
//                          priority SP
//                          connection-address SP     ;from RFC 4566
//                          port         ;port from RFC 4566
//                          SP cand-type
//                          [SP rel-addr]
//                          [SP rel-port]
//                          *(SP extension-att-name SP
//                               extension-att-value)
class SdpCandidateAttributeList : public SdpAttribute
{
public:
  SdpCandidateAttributeList() :
    SdpAttribute(kCandidateAttribute, "candidate") {}
};

// RFC4145
//         connection-attr        = "a=connection:" conn-value
//         conn-value             = "new" / "existing"
class SdpConnectionAttribute : public SdpAttribute
{
public:
  enum ConnValue {
    kNew,
    kExisting
  };

  SdpConnectionAttribute(SdpConnectionAttribute::ConnValue value) :
    SdpAttribute(kConnectionAttribute, "connection"),
    mValue(value) {}

  enum ConnValue mValue;
};

// RFC5285
//        extmap = mapentry SP extensionname [SP extensionattributes]
//
//        extensionname = URI
//
//        direction = "sendonly" / "recvonly" / "sendrecv" / "inactive"
//
//        mapentry = "extmap:" 1*5DIGIT ["/" direction]
//
//        extensionattributes = byte-string
//
//        URI = <Defined in RFC 3986>
//
//        byte-string = <Defined in RFC 4566>
//
//        SP = <Defined in RFC 5234>
//
//        DIGIT = <Defined in RFC 5234>
class SdpExtmapAttributeList : public SdpAttribute
{
public:
  SdpExtmapAttributeList() :
    SdpAttribute(kExtmapAttribute, "extmap") {}
};


// RFC4572
//   fingerprint-attribute  =  "fingerprint" ":" hash-func SP fingerprint
//
//   hash-func              =  "sha-1" / "sha-224" / "sha-256" /
//                             "sha-384" / "sha-512" /
//                             "md5" / "md2" / token
//                             ; Additional hash functions can only come
//                             ; from updates to RFC 3279
//
//   fingerprint            =  2UHEX *(":" 2UHEX)
//                             ; Each byte in upper-case hex, separated
//                             ; by colons.
//
//   UHEX                   =  DIGIT / %x41-46 ; A-F uppercase
class SdpFingerprintAttribute : public SdpAttribute
{
public:
  SdpFingerprintAttribute() :
    SdpAttribute(kFingerprintAttribute, "fingerprint") {}
};

// RFC4566, RFC5576
//       a=fmtp:<format> <format specific parameters>
class SdpFmtpAttributeList : public SdpAttribute
{
public:
  SdpFmtpAttributeList() :
    SdpAttribute(kFmtpAttribute, "fmtp") {}
};

// RFC5888
//         group-attribute     = "a=group:" semantics
//                               *(SP identification-tag)
//         semantics           = "LS" / "FID" / semantics-extension
//         semantics-extension = token
class SdpGroupAttribute : public SdpAttribute
{
public:
  SdpGroupAttribute() :
    SdpAttribute(kGroupAttribute, "group") {}
};

// RFC5245
// ice-options           = "ice-options" ":" ice-option-tag
//                           0*(SP ice-option-tag)
// ice-option-tag        = 1*ice-char
class SdpIceOptionsAttribute : public SdpAttribute
{
public:
  SdpIceOptionsAttribute() :
    SdpAttribute(kIceOptionsAttribute, "ice-options") {}
};

// draft-ietf-rtcweb-security-arch
//   identity-attribute  = "identity:" identity-assertion
//                         [ SP identity-extension
//                           *(";" [ SP ] identity-extension) ]
//   identity-assertion  = base64
//   base64              = 1*(ALPHA / DIGIT / "+" / "/" / "=" )
//   identity-extension  = extension-att-name [ "=" extension-att-value ]
//   extension-att-name  = token
//   extension-att-value = 1*(%x01-09 / %x0b-0c / %x0e-3a / %x3c-ff)
//                         ; byte-string from [RFC4566] omitting ";"
class SdpIdentityAttribute : public SdpAttribute
{
public:
  SdpIdentityAttribute() :
    SdpAttribute(kIdentityAttribute, "identity") {}
};

// RFC6236
//     image-attr = "imageattr:" PT 1*2( 1*WSP ( "send" / "recv" )
//                                       1*WSP attr-list )
//     PT = 1*DIGIT / "*"
//     attr-list = ( set *(1*WSP set) ) / "*"
//       ;  WSP and DIGIT defined in [RFC5234]
//
//     set= "[" "x=" xyrange "," "y=" xyrange *( "," key-value ) "]"
//                ; x is the horizontal image size range (pixel count)
//                ; y is the vertical image size range (pixel count)
//
//     key-value = ( "sar=" srange )
//               / ( "par=" prange )
//               / ( "q=" qvalue )
//                ; Key-value MAY be extended with other keyword
//                ;  parameters.
//                ; At most, one instance each of sar, par, or q
//                ;  is allowed in a set.
//                ;
//                ; sar (sample aspect ratio) is the sample aspect ratio
//                ;  associated with the set (optional, MAY be ignored)
//                ; par (picture aspect ratio) is the allowed
//                ;  ratio between the display's x and y physical
//                ;  size (optional)
//                ; q (optional, range [0.0..1.0], default value 0.5)
//                ;  is the preference for the given set,
//                ;  a higher value means a higher preference
//
//     onetonine = "1" / "2" / "3" / "4" / "5" / "6" / "7" / "8" / "9"
//                ; Digit between 1 and 9
//     xyvalue = onetonine *5DIGIT
//                ; Digit between 1 and 9 that is
//                ; followed by 0 to 5 other digits
//     step = xyvalue
//     xyrange = ( "[" xyvalue ":" [ step ":" ] xyvalue "]" )
//                ; Range between a lower and an upper value
//                ; with an optional step, default step = 1
//                ; The rightmost occurrence of xyvalue MUST have a
//                ; higher value than the leftmost occurrence.
//             / ( "[" xyvalue 1*( "," xyvalue ) "]" )
//                ; Discrete values separated by ','
//             / ( xyvalue )
//                ; A single value
//     spvalue = ( "0" "." onetonine *3DIGIT )
//                ; Values between 0.1000 and 0.9999
//             / ( onetonine "." 1*4DIGIT )
//                ; Values between 1.0000 and 9.9999
//     srange =  ( "[" spvalue 1*( "," spvalue ) "]" )
//                ; Discrete values separated by ','.
//                ; Each occurrence of spvalue MUST be
//                ; greater than the previous occurrence.
//             / ( "[" spvalue "-" spvalue "]" )
//                ; Range between a lower and an upper level (inclusive)
//                ; The second occurrence of spvalue MUST have a higher
//                ; value than the first
//             / ( spvalue )
//                ; A single value
//
//     prange =  ( "[" spvalue "-" spvalue "]" )
//                ; Range between a lower and an upper level (inclusive)
//                ; The second occurrence of spvalue MUST have a higher
//                ; value than the first
//
//     qvalue  = ( "0" "." 1*2DIGIT )
//             / ( "1" "." 1*2("0") )
//                ; Values between 0.00 and 1.00
class SdpImageattrAttributeList : public SdpAttribute
{
public:
  SdpImageattrAttributeList() :
    SdpAttribute(kImageattrAttribute, "imageattr") {}
};

// draft-ietf-mmusic-msid
//   msid-attr = "msid:" identifier [ SP appdata ]
//   identifier = 1*64token-char ; see RFC 4566
//   appdata = 1*64token-char  ; see RFC 4566
class SdpMsidAttributeList : public SdpAttribute
{
public:
  SdpMsidAttributeList() :
    SdpAttribute(kMsidAttribute, "msid") {}
};

// RFC5245
//   remote-candidate-att = "remote-candidates" ":" remote-candidate
//                           0*(SP remote-candidate)
//   remote-candidate = component-ID SP connection-address SP port
class SdpRemoteCandidatesAttribute : public SdpAttribute
{
public:
  SdpRemoteCandidatesAttribute() :
    SdpAttribute(kRemoteCandidatesAttribute, "remote-candidates") {}
};

// RFC3605
//   rtcp-attribute =  "a=rtcp:" port  [nettype space addrtype space
//                         connection-address] CRLF
class SdpRtcpAttribute : public SdpAttribute
{
public:
  SdpRtcpAttribute() :
    SdpAttribute(kRtcpAttribute, "rtcp") {}
};

// RFC4585
//    rtcp-fb-syntax = "a=rtcp-fb:" rtcp-fb-pt SP rtcp-fb-val CRLF
//
//    rtcp-fb-pt         = "*"   ; wildcard: applies to all formats
//                       / fmt   ; as defined in SDP spec
//
//    rtcp-fb-val        = "ack" rtcp-fb-ack-param
//                       / "nack" rtcp-fb-nack-param
//                       / "trr-int" SP 1*DIGIT
//                       / rtcp-fb-id rtcp-fb-param
//
//    rtcp-fb-id         = 1*(alpha-numeric / "-" / "_")
//
//    rtcp-fb-param      = SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
//
//    rtcp-fb-ack-param  = SP "rpsi"
//                       / SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
//
//    rtcp-fb-nack-param = SP "pli"
//                       / SP "sli"
//                       / SP "rpsi"
//                       / SP "app" [SP byte-string]
//                       / SP token [SP byte-string]
//                       / ; empty
class SdpRtcpFbAttributeList : public SdpAttribute
{
public:
  SdpRtcpFbAttributeList() :
    SdpAttribute(kRtcpFbAttribute, "rtcp-fb") {}
};

// RFC4566
// a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]
class SdpRtpmapAttributeList : public SdpAttribute
{
public:
  SdpRtpmapAttributeList() :
    SdpAttribute(kRtpmapAttribute, "rtpmap") {}
};

// draft-ietf-mmusic-sctp-sdp-06
//         sctpmap-attr        =  "a=sctpmap:" sctpmap-number
//                                 app [max-message-size] [streams]
//         sctpmap-number      =  1*DIGIT
//         app                 =  token
//         max-message-size    =  "max-message-size" EQUALS 1*DIGIT
//         streams             =  "streams" EQUALS 1*DIGIT"
// (draft-07 appears to have done something really funky here, but I
// don't beleive it).
class SdpSctpmapAttributeList : public SdpAttribute
{
public:
  SdpSctpmapAttributeList() :
    SdpAttribute(kSctpmapAttribute, "sctpmap") {}
};

// RFC4145
//       setup-attr           =  "a=setup:" role
//       role                 =  "active" / "passive" / "actpass" / "holdconn"
class SdpSetupAttribute : public SdpAttribute
{
public:
  SdpSetupAttribute() :
    SdpAttribute(kSetupAttribute, "setup") {}
};

// RFC5576
// ssrc-attr = "ssrc:" ssrc-id SP attribute
// ; The base definition of "attribute" is in RFC 4566.
// ; (It is the content of "a=" lines.)
//
// ssrc-id = integer ; 0 .. 2**32 - 1
class SdpSsrcAttribute : public SdpAttribute
{
public:
  SdpSsrcAttribute() :
    SdpAttribute(kSsrcAttribute, "ssrc") {}
};

// RFC5576
// ssrc-group-attr = "ssrc-group:" semantics *(SP ssrc-id)
//
// semantics       = "FEC" / "FID" / token
//
// ssrc-id = integer ; 0 .. 2**32 - 1
class SdpSsrcGroupAttribute : public SdpAttribute
{
public:
  SdpSsrcGroupAttribute() :
    SdpAttribute(kSsrcGroupAttribute, "ssrc-group") {}
};

// Used for any other kind of attribute not otherwise specialized
class SdpOtherAttribute : public SdpAttribute
{
public:
  SdpOtherAttribute(std::string typeName, std::string value = "") :
    SdpAttribute(kOtherAttribute, typeName),
    mValue(value) {}

  std::string const getValue()
  {
    return mValue;
  }

private:
  std::string mTypeName;
  std::string mValue;
};

}

#endif
