/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_ALIASES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_ALIASES_H_

#include <functional>
#include <string>
#include <vector>

#include "bat/ads/internal/ad_server/catalog/bundle/creative_new_tab_page_ad_info_aliases.h"

namespace ads {

using GetCreativeNewTabPageAdCallback =
    std::function<void(const bool,
                       const std::string& creative_instance_id,
                       const CreativeNewTabPageAdInfo&)>;

using GetCreativeNewTabPageAdsCallback =
    std::function<void(const bool,
                       const std::vector<std::string>&,
                       const CreativeNewTabPageAdList&)>;

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_NEW_TAB_PAGE_ADS_DATABASE_TABLE_ALIASES_H_
