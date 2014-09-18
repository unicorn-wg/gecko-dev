/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "signaling/src/sdp/SipccSdpParser.h"

#include <utility>
extern "C" {
#include "signaling/src/sdp/sipcc/sdp.h"
}

namespace mozilla {

UniquePtr<Sdp>
SipccSdpParser::Parse(const std::string& sdpText)
{
  sdp_conf_options_t *sipcc_config = sdp_init_config();
  if (!sipcc_config) {
    return nullptr;
  }

  // XXX - capture errors properly
  // sdp_config_set_error_handler(sipcc_sdp_parser_error_handler, this);

  sdp_t *sdp = sdp_init_description(sipcc_config);
  if (sdp) {
    const char* rawString = sdpText.c_str();
    sdp_result_e result = sdp_parse(sdp, rawString, sdpText.length());
    if (result == SDP_SUCCESS) {
      SipccSdp* sipccSdp = new SipccSdp(sdp);
      sipccSdp->Load();
      return UniquePtr<Sdp>(sipccSdp);
    }

    sdp_free_description(sdp);
  }

  SDP_FREE(sipcc_config); // FIXME: need a proper release for this
  return nullptr;
}

void
SipccSdpParser::AddParseError(uint32_t line, const std::string& message)
{
  mErrors.push_back(std::make_pair(line, message));
}

extern "C" {

void
sipcc_sdp_parser_error_handler(void *context, uint32_t line, const char *message)
{
  SipccSdpParser* parser = static_cast<SipccSdpParser*>(context);
  std::string err(message);
  parser->AddParseError(line, err);
}

}

} // namespace mozilla

