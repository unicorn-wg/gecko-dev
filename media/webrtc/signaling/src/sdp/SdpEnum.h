/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDPENUM_H_
#define _SDPENUM_H_

#include <ostream>

namespace mozilla {
namespace sdp {

#ifdef CRLF
#undef CRLF
#endif
#define CRLF "\r\n"

enum NetType {
  kNetTypeNone,
  kInternet
};

inline std::ostream& operator <<(std::ostream& os, sdp::NetType t)
{
  switch (t) {
    case sdp::kInternet: os << "IN"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}


enum AddrType {
  kAddrTypeNone,
  kIPv4,
  kIPv6
};

inline std::ostream& operator <<(std::ostream& os, sdp::AddrType t)
{
  switch (t) {
    case sdp::kIPv4: os << "IP4"; break;
    case sdp::kIPv6: os << "IP6"; break;
    default: MOZ_ASSERT(false); os << "?";
  }
  return os;
}

}

}

#endif
