/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_BUNDLE_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_BUNDLE_H_

#include "bat/ads/internal/ad_server/catalog/bundle/creative_ad_notification_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/bundle/creative_inline_content_ad_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/bundle/creative_new_tab_page_ad_info_aliases.h"
#include "bat/ads/internal/ad_server/catalog/bundle/creative_promoted_content_ad_info_aliases.h"
#include "bat/ads/internal/conversions/conversion_info_aliases.h"

namespace ads {

class Catalog;
struct BundleInfo;

class Bundle final {
 public:
  Bundle();
  ~Bundle();

  void BuildFromCatalog(const Catalog& catalog);

 private:
  BundleInfo FromCatalog(const Catalog& catalog) const;

  void DeleteDatabaseTables();

  void DeleteCampaigns();
  void DeleteSegments();
  void DeleteCreativeAds();
  void DeleteCreativeNewTabPageAdWallpapers();
  void DeleteDayparts();
  void DeleteGeoTargets();

  void DeleteCreativeAdNotifications();
  void SaveCreativeAdNotifications(
      const CreativeAdNotificationList& creative_ad_notifications);

  void DeleteCreativeInlineContentAds();
  void SaveCreativeInlineContentAds(
      const CreativeInlineContentAdList& creative_inline_content_ads);

  void DeleteCreativeNewTabPageAds();
  void SaveCreativeNewTabPageAds(
      const CreativeNewTabPageAdList& creative_new_tab_page_ads);

  void DeleteCreativePromotedContentAds();
  void SaveCreativePromotedContentAds(
      const CreativePromotedContentAdList& creative_promoted_content_ads);

  void PurgeExpiredDeposits();

  void PurgeExpiredConversions();
  void SaveConversions(const ConversionList& conversions);
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_BUNDLE_BUNDLE_H_
