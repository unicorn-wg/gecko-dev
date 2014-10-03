/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _JSEPCODECDESCRIPTION_H_
#define _JSEPCODECDESCRIPTION_H_

#include <iostream>
#include <string>
#include "signaling/src/sdp/SdpMediaSection.h"

namespace mozilla {
namespace jsep {

#define JSEP_CODEC_CLONE(T) \
  virtual JsepCodecDescription* Clone() const MOZ_OVERRIDE { \
return new T(*this); }

// A single entry in our list of known codecs.
struct JsepCodecDescription {
  JsepCodecDescription(mozilla::SdpMediaSection::MediaType type,
                       const std::string& default_pt,
                       const std::string& name,
                       uint32_t clock,
                       uint32_t channels,
                       bool enabled) :
      mType(type),
      mDefaultPt(default_pt),
      mName(name),
      mClock(clock),
      mChannels(channels),
      mEnabled(enabled) {}
  virtual ~JsepCodecDescription() {}

  virtual JsepCodecDescription* Clone() const = 0;
  virtual void AddFmtps(SdpFmtpAttributeList& fmtp) const = 0;
  virtual void AddRtcpFbs(SdpRtcpFbAttributeList& rtcpfb) const = 0;
  virtual bool LoadFmtps(
      const SdpFmtpAttributeList::Parameters& params) = 0;
  virtual bool LoadRtcpFb(
      const SdpRtcpFbAttributeList::Feedback& feedback) = 0;


  virtual JsepCodecDescription* MakeNegotiatedCodec(
      const mozilla::SdpMediaSection& m_section,
      const mozilla::SdpRtpmapAttributeList::Rtpmap& rtpmap) const {
    JsepCodecDescription* negotiated = Clone();
    negotiated->mDefaultPt = rtpmap.pt;

    const SdpAttributeList& attrs = m_section.GetAttributeList();
    if (attrs.HasAttribute(SdpAttribute::kFmtpAttribute)) {
      const SdpFmtpAttributeList& fmtps = attrs.GetFmtp();
      for (auto i = fmtps.mFmtps.begin(); i != fmtps.mFmtps.end(); ++i) {
        if (i->format == negotiated->mDefaultPt) {
          if (i->parameters) {
            if (!negotiated->LoadFmtps(*i->parameters)) {
              // Remote parameters were invalid
              delete negotiated;
              return nullptr;
            }
          } else {
            // TODO: Fmtp that we don't understand. What should we do here?
          }
        }
      }
    }

    if (attrs.HasAttribute(SdpAttribute::kRtcpFbAttribute)) {
      auto& rtcpfbs = attrs.GetRtcpFb().mFeedbacks;
      for (auto i = rtcpfbs.begin(); i != rtcpfbs.end(); ++i) {
        if (i->pt == negotiated->mDefaultPt) {
          if (!negotiated->LoadRtcpFb(*i)) {
              // Remote parameters were invalid
              delete negotiated;
              return nullptr;
          }
        }
      }
    }

    return negotiated;
  }

  virtual void AddToMediaSection(SdpMediaSection& m_section) const {
    if (mEnabled && m_section.GetMediaType() == mType) {
      m_section.AddCodec(mDefaultPt, mName, mClock, mChannels);
      AddFmtps(m_section);
      AddRtcpFbs(m_section);
    }
  }

  virtual void AddFmtps(SdpMediaSection& m_section) const {
    SdpAttributeList& attrs = m_section.GetAttributeList();

    SdpFmtpAttributeList* fmtps;

    if (attrs.HasAttribute(SdpAttribute::kFmtpAttribute)) {
      fmtps = new SdpFmtpAttributeList(attrs.GetFmtp());
    } else {
      fmtps = new SdpFmtpAttributeList;
    }

    AddFmtps(*fmtps);

    if (fmtps->mFmtps.empty()) {
      delete fmtps;
    } else {
      attrs.SetAttribute(fmtps);
    }
  }

  virtual void AddRtcpFbs(SdpMediaSection& m_section) const {
    SdpAttributeList& attrs = m_section.GetAttributeList();

    SdpRtcpFbAttributeList* rtcpfbs;

    if (attrs.HasAttribute(SdpAttribute::kRtcpFbAttribute)) {
      rtcpfbs = new SdpRtcpFbAttributeList(attrs.GetRtcpFb());
    } else {
      rtcpfbs = new SdpRtcpFbAttributeList;
    }

    AddRtcpFbs(*rtcpfbs);

    attrs.SetAttribute(rtcpfbs);
  }

  mozilla::SdpMediaSection::MediaType mType;
  std::string mDefaultPt;
  std::string mName;
  uint32_t mClock;
  uint32_t mChannels;
  bool mEnabled;
};


struct JsepAudioCodecDescription : public JsepCodecDescription {
  JsepAudioCodecDescription(const std::string& default_pt,
                            const std::string& name,
                            uint32_t clock,
                            uint32_t channels,
                            uint32_t packet_size = 0, // TODO(ekr@rtfm.com): Remove when I have reasonable defaults.
                            uint32_t bit_rate = 0,
                            bool enabled = true) :
      JsepCodecDescription(mozilla::SdpMediaSection::kAudio,
                           default_pt, name, clock, channels, enabled),
      mPacketSize(packet_size),
      mBitrate(bit_rate) {}

  virtual void AddFmtps(SdpFmtpAttributeList& fmtp) const MOZ_OVERRIDE {
    // TODO
  }

  virtual void AddRtcpFbs(SdpRtcpFbAttributeList& rtcpfb) const MOZ_OVERRIDE {
    // TODO: Do we want to add anything?
  }

  virtual bool LoadFmtps(
      const SdpFmtpAttributeList::Parameters& params) MOZ_OVERRIDE {
    // TODO
    return true;
  }

  virtual bool LoadRtcpFb(
      const SdpRtcpFbAttributeList::Feedback& feedback) MOZ_OVERRIDE {
    // Nothing to do
    return true;
  }

  JSEP_CODEC_CLONE(JsepAudioCodecDescription)

  uint32_t mPacketSize;
  uint32_t mBitrate;
};


struct JsepVideoCodecDescription : public JsepCodecDescription {
  JsepVideoCodecDescription(const std::string& default_pt,
                            const std::string& name,
                            uint32_t clock,
                            bool enabled = true) :
      JsepCodecDescription(mozilla::SdpMediaSection::kVideo,
                           default_pt, name, clock, 0, enabled),
      mMaxFs(0),
      mMaxFr(0),
      mPacketizationMode(0),
      mMaxMbps(0),
      mMaxCpb(0),
      mMaxDpb(0),
      mMaxBr(0) {}

  virtual void AddFmtps(SdpFmtpAttributeList& fmtp) const MOZ_OVERRIDE {
    if (mName == "H264") {
      UniquePtr<SdpFmtpAttributeList::H264Parameters> params =
        MakeUnique<SdpFmtpAttributeList::H264Parameters>();

      params->packetization_mode = mPacketizationMode;
      // Hard-coded, may need to change someday?
      params->level_asymmetry_allowed = true;
      params->profile_level_id = mProfileLevelId;
      params->max_mbps = mMaxMbps;
      params->max_fs = mMaxFs;
      params->max_cpb = mMaxCpb;
      params->max_dpb = mMaxDpb;
      params->max_br = mMaxBr;
      strncpy(params->sprop_parameter_sets,
              mSpropParameterSets.c_str(),
              sizeof(params->sprop_parameter_sets));
      fmtp.PushEntry(mDefaultPt, "", mozilla::Move(params));
    } else if (mName == "VP8") {
      // TODO
    }
  }

  virtual void AddRtcpFbs(SdpRtcpFbAttributeList& rtcpfb) const MOZ_OVERRIDE {
    // Just hard code for now
    rtcpfb.PushEntry(mDefaultPt,
                     SdpRtcpFbAttributeList::kNack);
    rtcpfb.PushEntry(mDefaultPt,
                     SdpRtcpFbAttributeList::kNack,
                     SdpRtcpFbAttributeList::pli);
    rtcpfb.PushEntry(mDefaultPt,
                     SdpRtcpFbAttributeList::kCcm,
                     SdpRtcpFbAttributeList::fir);
  }

  virtual bool LoadFmtps(
      const SdpFmtpAttributeList::Parameters& params) MOZ_OVERRIDE {
    switch (params.codec_type) {
      case SdpRtpmapAttributeList::kH264:
        LoadH264Parameters(params);
        break;
      case SdpRtpmapAttributeList::kVP8:
        LoadVP8Parameters(params);
        break;
      default:
        ;
    }
    return true;
  }

  virtual bool LoadRtcpFb(
      const SdpRtcpFbAttributeList::Feedback& feedback) MOZ_OVERRIDE {
    switch (feedback.type) {
      case SdpRtcpFbAttributeList::kAck:
        mAckFbTypes.push_back(feedback.parameter);
        break;
      case SdpRtcpFbAttributeList::kCcm:
        mCcmFbTypes.push_back(feedback.parameter);
        break;
      case SdpRtcpFbAttributeList::kNack:
        mNackFbTypes.push_back(feedback.parameter);
        break;
      case SdpRtcpFbAttributeList::kApp:
      case SdpRtcpFbAttributeList::kTrrInt:
        // We don't support these, ignore.
        {}
    }
    return true;
  }

  void LoadH264Parameters(const SdpFmtpAttributeList::Parameters& params) {
    const SdpFmtpAttributeList::H264Parameters& h264_params =
      static_cast<const SdpFmtpAttributeList::H264Parameters&>(params);

    mMaxFs = h264_params.max_fs;
    mProfileLevelId = h264_params.profile_level_id;
    mPacketizationMode = h264_params.packetization_mode;
    mMaxMbps = h264_params.max_mbps;
    mMaxCpb = h264_params.max_cpb;
    mMaxDpb = h264_params.max_dpb;
    mMaxBr = h264_params.max_br;
    mSpropParameterSets = h264_params.sprop_parameter_sets;
  }

  void LoadVP8Parameters(const SdpFmtpAttributeList::Parameters& params) {
    // TODO: Implement
  }

  JSEP_CODEC_CLONE(JsepVideoCodecDescription)

  std::vector<std::string> mAckFbTypes;
  std::vector<std::string> mNackFbTypes;
  std::vector<std::string> mCcmFbTypes;

  uint32_t mMaxFs;

  // H264-specific stuff
  uint32_t mProfileLevelId;
  uint32_t mMaxFr;
  uint32_t mPacketizationMode;
  uint32_t mMaxMbps;
  uint32_t mMaxCpb;
  uint32_t mMaxDpb;
  uint32_t mMaxBr;
  std::string mSpropParameterSets;
};

}  // namespace jsep
}  // namespace mozilla

#endif
