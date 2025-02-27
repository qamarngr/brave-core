/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_SET_INFO_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_SET_INFO_H_

#include <string>

#include "bat/ads/internal/ad_server/catalog/catalog_creative_ad_notification_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/catalog_creative_inline_content_ad_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/catalog_creative_new_tab_page_ad_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/catalog_creative_promoted_content_ad_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/catalog_os_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/catalog_segment_info_aliases.h"
#include "bat/ads/internal/conversions/conversion_info_aliases.h"

namespace ads {

struct CatalogCreativeSetInfo final {
  CatalogCreativeSetInfo();
  CatalogCreativeSetInfo(const CatalogCreativeSetInfo& info);
  ~CatalogCreativeSetInfo();

  bool operator==(const CatalogCreativeSetInfo& rhs) const;
  bool operator!=(const CatalogCreativeSetInfo& rhs) const;

  std::string creative_set_id;
  unsigned int per_day = 0;
  unsigned int per_week = 0;
  unsigned int per_month = 0;
  unsigned int total_max = 0;
  double value = 0.0;
  std::string split_test_group;
  CatalogSegmentList segments;
  CatalogOsList oses;
  CatalogCreativeAdNotificationList creative_ad_notifications;
  CatalogCreativeInlineContentAdList creative_inline_content_ads;
  CatalogCreativeNewTabPageAdList creative_new_tab_page_ads;
  CatalogCreativePromotedContentAdList creative_promoted_content_ads;
  ConversionList conversions;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_SET_INFO_H_
