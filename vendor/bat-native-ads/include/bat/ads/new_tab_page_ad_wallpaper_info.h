/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_INCLUDE_BAT_ADS_NEW_TAB_PAGE_AD_WALLPAPER_INFO_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_INCLUDE_BAT_ADS_NEW_TAB_PAGE_AD_WALLPAPER_INFO_H_

#include "bat/ads/export.h"
#include "bat/ads/new_tab_page_ad_wallpaper_focal_point_info.h"
#include "url/gurl.h"

namespace ads {

struct ADS_EXPORT NewTabPageAdWallpaperInfo final {
  NewTabPageAdWallpaperInfo();
  NewTabPageAdWallpaperInfo(const NewTabPageAdWallpaperInfo& info);
  ~NewTabPageAdWallpaperInfo();

  GURL image_url;
  NewTabPageAdWallpaperFocalPointInfo focal_point;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_INCLUDE_BAT_ADS_NEW_TAB_PAGE_AD_WALLPAPER_INFO_H_
