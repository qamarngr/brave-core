/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_INFO_ALIASES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_INFO_ALIASES_H_

#include <map>
#include <string>
#include <vector>

#include "bat/ads/internal/ad_server/catalog/bundle/creative_promoted_content_ad_info.h"

namespace ads {

using CreativePromotedContentAdList =
    std::vector<CreativePromotedContentAdInfo>;
using CreativePromotedContentAdMap =
    std::map<std::string, CreativePromotedContentAdInfo>;

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_PROMOTED_CONTENT_AD_INFO_ALIASES_H_
