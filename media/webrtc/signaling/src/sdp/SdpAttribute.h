/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _SDP_ATTRIBUTE_H_
#define _SDP_ATTRIBUTE_H_

#include "mozilla/UniquePtr.h"

#include "signaling/src/sdp/SdpEnum.h"

namespace mozilla {

class SdpAttribute
{
public:
  sdp::AttributeType GetType () const;
  std::string GetTypeName() const;

protected:
  virtual ~SdpAttribute();
};

class SdpAttributeList
{
public:
  bool HasAttribute(SdpAttributeType type,
                    unsigned int instance = 0);

  UniquePtr<SdpAttribute> GetAttibute(sdp::AttributeType type,
                                      unsigned int instance = 0);
}

}

#endif
