/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPTRANSPORT_H_
#define _JSEPTRANSPORT_H_

#include <mozilla/RefPtr.h>

#include "signaling/src/media/sdp/SdpAttribute.h"

namespace mozilla {
namespace jsep {

class JsepDtlsTransport {
 public:
  virtual SdpFingerprintAttributeList& fingerprints() const = 0;
};

class JsepIceTransport {
 public:
  virtual 
};

class JsepTransport {
 public:
  // ICE stuff.

  // DTLS stuff.
 private:

};

}  // namespace jsep
}  // namespace mozilla

#endif
