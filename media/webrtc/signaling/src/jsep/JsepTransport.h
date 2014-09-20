/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPTRANSPORT_H_
#define _JSEPTRANSPORT_H_

#include <mozilla/RefPtr.h>

#include "signaling/src/sdp/SdpAttribute.h"

namespace mozilla {
namespace jsep {

class JsepDtlsTransport {
 public:
  virtual SdpFingerprintAttributeList& fingerprints() const = 0;
};

class JsepIceTransport {
 public:
  const std::string& ufrag() const = 0;
  const std::string& password() const = 0;
  const std::vector<const std::string>& candidates() = 0;
  const SdpIceOptionsAttribute& options() = 0;
};

class JsepTransport {
 public:
  // ICE stuff.
  UniquePtr<JsepIceTransport> mIce;
  UniquePtr<JsepDtlsTransport> mDtls;

  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(JsepTransport);

 protected:
  ~JsepTransport() {}
};

}  // namespace jsep
}  // namespace mozilla

#endif
