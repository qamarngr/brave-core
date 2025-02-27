/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/creatives/promoted_content_ads/promoted_content_ad_builder.h"

#include "base/guid.h"
#include "bat/ads/internal/ad_server/catalog/bundle/creative_promoted_content_ad_info.h"
#include "bat/ads/promoted_content_ad_info.h"

namespace ads {

PromotedContentAdInfo BuildPromotedContentAd(
    const CreativePromotedContentAdInfo& creative_promoted_content_ad) {
  const std::string placement_id =
      base::GUID::GenerateRandomV4().AsLowercaseString();
  return BuildPromotedContentAd(creative_promoted_content_ad, placement_id);
}

PromotedContentAdInfo BuildPromotedContentAd(
    const CreativePromotedContentAdInfo& creative_promoted_content_ad,
    const std::string& placement_id) {
  PromotedContentAdInfo promoted_content_ad;

  promoted_content_ad.type = AdType::kPromotedContentAd;
  promoted_content_ad.placement_id = placement_id;
  promoted_content_ad.creative_instance_id =
      creative_promoted_content_ad.creative_instance_id;
  promoted_content_ad.creative_set_id =
      creative_promoted_content_ad.creative_set_id;
  promoted_content_ad.campaign_id = creative_promoted_content_ad.campaign_id;
  promoted_content_ad.advertiser_id =
      creative_promoted_content_ad.advertiser_id;
  promoted_content_ad.segment = creative_promoted_content_ad.segment;
  promoted_content_ad.target_url = creative_promoted_content_ad.target_url;
  promoted_content_ad.title = creative_promoted_content_ad.title;
  promoted_content_ad.description = creative_promoted_content_ad.description;

  return promoted_content_ad;
}

}  // namespace ads
