/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_UNITTEST_UTIL_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_UNITTEST_UTIL_H_

#include "bat/ads/internal/ad_server/catalog/bundle/creative_promoted_content_ad_info_aliases.h"

namespace ads {

struct CreativePromotedContentAdInfo;

void SaveCreativeAds(const CreativePromotedContentAdList& creative_ads);

CreativePromotedContentAdList BuildCreativePromotedContentAds(const int count);
CreativePromotedContentAdInfo BuildCreativePromotedContentAd();

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_UNITTEST_UTIL_H_
