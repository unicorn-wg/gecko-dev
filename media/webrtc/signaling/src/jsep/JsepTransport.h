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
  virtual ~JsepDtlsTransport() {}

  virtual const SdpFingerprintAttributeList& fingerprints() const = 0;
};

class JsepIceTransport {
 public:
  virtual ~JsepIceTransport() {}

  virtual const std::string& ufrag() const = 0;
  virtual const std::string& password() const = 0;
  virtual const std::vector<std::string>& candidates() const = 0;
   //  virtual const SdpIceOptionsAttribute& options() const = 0;
 };

 class JsepTransport {
  public:
   JsepTransport(const std::string& id,
                 UniquePtr<JsepIceTransport> ice,
                 UniquePtr<JsepDtlsTransport> dtls) :
       mTransportId(id),
       mIce(Move(ice)),
       mDtls(Move(dtls)) {}

 // Unique identifier for this transport within this call. Group?
  std::string mTransportId;

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
