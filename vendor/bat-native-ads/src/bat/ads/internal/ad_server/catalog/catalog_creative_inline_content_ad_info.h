/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_INLINE_CONTENT_AD_INFO_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_INLINE_CONTENT_AD_INFO_H_

#include "bat/ads/internal/ad_server/catalog/catalog_creative_info.h"
#include "bat/ads/internal/ad_server/catalog/catalog_inline_content_ad_payload_info.h"

namespace ads {

struct CatalogCreativeInlineContentAdInfo final : CatalogCreativeInfo {
  CatalogCreativeInlineContentAdInfo();
  CatalogCreativeInlineContentAdInfo(
      const CatalogCreativeInlineContentAdInfo& info);
  ~CatalogCreativeInlineContentAdInfo();

  bool operator==(const CatalogCreativeInlineContentAdInfo& rhs) const;
  bool operator!=(const CatalogCreativeInlineContentAdInfo& rhs) const;

  CatalogInlineContentAdPayloadInfo payload;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_AD_SERVER_CATALOG_CATALOG_CREATIVE_INLINE_CONTENT_AD_INFO_H_
