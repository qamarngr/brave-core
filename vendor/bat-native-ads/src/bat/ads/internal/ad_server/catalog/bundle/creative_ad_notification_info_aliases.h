/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_AD_NOTIFICATION_INFO_ALIASES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_AD_NOTIFICATION_INFO_ALIASES_H_

#include <map>
#include <string>
#include <vector>

#include "bat/ads/internal/ad_server/catalog/bundle/creative_ad_notification_info.h"

namespace ads {

using CreativeAdNotificationList = std::vector<CreativeAdNotificationInfo>;
using CreativeAdNotificationMap =
    std::map<std::string, CreativeAdNotificationInfo>;

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_CREATIVE_AD_NOTIFICATION_INFO_ALIASES_H_
