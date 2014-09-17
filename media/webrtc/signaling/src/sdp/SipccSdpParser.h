/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SIPCCSDPPARSER_H_
#define _SIPCCSDPPARSER_H_

#include <vector>
#include <string>

#include "mozilla/UniquePtr.h"

#include "signaling/src/sdp/Sdp.h"

namespace mozilla {

class SipccSdpParser
{
public:
  SipccSdpParser() {}
  ~SipccSdpParser() {}

  /**
   * This parses the provided text into an SDP object.
   * This returns a nullptr-valued pointer if things go poorly.
   */
  UniquePtr<Sdp> Parse(const std::string& sdpText);

  void AddParseError(uint32_t line, const std::string& message);

  /**
   * Returns a reference to the list of parse errors.
   * This gets cleared out when you call Parse.
   */
  const std::vector<std::pair<uint32_t, const std::string> >&
  GetParseErrors() const { return mErrors; }

private:
  std::vector<std::pair<uint32_t, const std::string> > mErrors;
};

} // namespace mozilla

#endif
