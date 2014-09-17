/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_ATTRIBUTE_H_
#define _SDP_ATTRIBUTE_H_

#include "mozilla/UniquePtr.h"
#include "mozilla/Attributes.h"

#include "signaling/src/sdp/SdpEnum.h"

namespace mozilla {

class SdpAttribute
{
public:
  virtual sdp::AttributeType GetType () const = 0;
  virtual std::string GetTypeName() const = 0;

protected:
  virtual ~SdpAttribute() {}
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
class SdpCandidateAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kCandidateAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "candidate";
  }
};

// RFC4145
//         connection-attr        = "a=connection:" conn-value
//         conn-value             = "new" / "existing"
class SdpConnectionAttribute : public SdpAttribute
{
public:
  SdpConnectionAttribute(ConnValue value) : mValue(value) {}

  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kConnectionAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "connection";
  }

  enum ConnValue {
    kNew,
    kExisting
  } mValue;

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
class SdpExtmapAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kExtmapAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "extmap";
  }
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
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kFingerprintAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "fingerprint";
  }
};

// RFC4566, RFC5576
//       a=fmtp:<format> <format specific parameters>
class SdpFmtpAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kFmtpAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "fmtp";
  }
};

// RFC5888
//         group-attribute     = "a=group:" semantics
//                               *(SP identification-tag)
//         semantics           = "LS" / "FID" / semantics-extension
//         semantics-extension = token
class SdpGroupAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kGroupAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "group";
  }
};

// RFC5245
// ice-options           = "ice-options" ":" ice-option-tag
//                           0*(SP ice-option-tag)
// ice-option-tag        = 1*ice-char
class SdpIceOptionsAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kGroupAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "group";
  }
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
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kIdentityAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "identity";
  }
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
class SdpImageattrAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kImageattrAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "imageattr";
  }
};

// draft-ietf-mmusic-msid
//   msid-attr = "msid:" identifier [ SP appdata ]
//   identifier = 1*64token-char ; see RFC 4566
//   appdata = 1*64token-char  ; see RFC 4566
class SdpMsidAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kMsidAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "msid";
  }
};

// RFC5245
//   remote-candidate-att = "remote-candidates" ":" remote-candidate
//                           0*(SP remote-candidate)
//   remote-candidate = component-ID SP connection-address SP port
class SdpRemoteCandidatesAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kRemoteCandidates;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "remote-candidates";
  }
};

// RFC3605
//   rtcp-attribute =  "a=rtcp:" port  [nettype space addrtype space
//                         connection-address] CRLF
class SdpRtcpAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kRtcpAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "rtcp";
  }
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
class SdpRtcpFbAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kRtcpFbAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "rtcp-fb";
  }
};

// RFC4566
// a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]
class SdpRtpmapAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kRtpmapAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "rtpmap";
  }
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
class SdpSctpmapAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kSctpmapAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "sctpmap";
  }
};

// RFC4145
//       setup-attr           =  "a=setup:" role
//       role                 =  "active" / "passive" / "actpass" / "holdconn"
class SdpSetupAttribute : public SdpAttribute
{
public:
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kSetupAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "setup";
  }
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
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kSsrcAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "ssrc";
  }
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
  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kSsrcAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return "ssrc";
  }
};

// Used for any other kind of attribute not otherwise specialized
class SdpOtherAttribute : public SdpAttribute
{
public:
  SdpOtherAttribute(std::string typeName, std::string value = "") :
    mTypeName(typeName), mValue(value) {}

  virtual sdp::AttributeType GetType() const MOZ_OVERRIDE
  {
    return sdp::kOtherAttribute;
  }

  virtual std::string GetTypeName() const MOZ_OVERRIDE
  {
    return mTypeName;
  }

  std::string const getValue()
  {
    return mValue;
  }

private:
  std::string mTypeName;
  std::string mValue;
};

class SdpAttributeList
{
public:
  virtual unsigned int CountAttributes(SdpAttributeType type) const = 0;
  virtual bool HasAttribute(SdpAttributeType type) const = 0;

  virtual UniquePtr<SdpAttribute>
    GetAttribute(sdp::AttributeType type) const = 0;

  virtual UniquePtr<SdpCandidateAttribute> GetCandidate() const = 0;
  virtual UniquePtr<SdpConnectionAttribute> GetConnection() const = 0;
  virtual UniquePtr<SdpExtmapAttribute> GetExtmap() const = 0;
  virtual UniquePtr<SdpFingerprintAttribute> GetFingerprint() const = 0;
  virtual UniquePtr<SdpFmtpAttribute> GetFmtp() const = 0;
  virtual UniquePtr<SdpGroupAttribute> GetGroup() const = 0;
  virtual UniquePtr<SdpIceOptionsAttribute> GetIceOptions() const = 0;
  virtual std::string GetIcePwd() const = 0;
  virtual std::string GetIceUfrag() const = 0;
  virtual UniquePtr<SdpIdentityAttribute> GetIdentity() const = 0;
  virtual UniquePtr<SdpImageattrAttribute> GetImageattr() const = 0;
  virtual std::string GetLabel() const = 0;
  virtual unsigned int GetMaxprate() const = 0;
  virtual unsigned int GetMaxptime() const = 0;
  virtual std::string GetMid() const = 0;
  virtual UniquePtr<SdpMsidAttribute> GetMsid() const = 0;
  virtual unsigned int GetPtime() const = 0;
  virtual UniquePtr<SdpRtcpAttribute> GetRtcp() const = 0;
  virtual UniquePtr<SdpRtcpFbAttribute> GetRtcpFb() const = 0;
  virtual UniquePtr<SdpRtcpRemoteCandidates> GetRemoteCandidates() const = 0;
  virtual UniquePtr<SdpRtpmapAttribute> GetRtpmap() const = 0;
  virtual UniquePtr<SdpSctpmapAttribute> GetSctpmap() const = 0;
  virtual UniquePtr<SdpSetupAttribute> GetSetup() const = 0;
  virtual UniquePtr<SdpSsrcAttribute> GetSsrc() const = 0;
  virtual UniquePtr<SdpSsrcGroupAttribute> GetSsrcGroup() const = 0;

  virtual SetAttribute(const SdpAttribute &) = 0;
};

}

#endif
