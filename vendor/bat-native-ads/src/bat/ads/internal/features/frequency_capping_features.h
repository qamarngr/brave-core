/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FEATURES_FREQUENCY_CAPPING_FEATURES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FEATURES_FREQUENCY_CAPPING_FEATURES_H_

#include "base/feature_list.h"

namespace base {
class TimeDelta;
}  // namespace base

namespace ads {
namespace features {
namespace frequency_capping {

extern const base::Feature kFeature;

bool IsEnabled();

bool ShouldExcludeAdIfConverted();

base::TimeDelta ExcludeAdIfDismissedWithinTimeWindow();

base::TimeDelta ExcludeAdIfTransferredWithinTimeWindow();

bool ShouldOnlyServeAdsInWindowedMode();

bool ShouldOnlyServeAdsWithValidInternetConnection();

bool ShouldOnlyServeAdsIfMediaIsNotPlaying();

bool ShouldOnlyServeAdsIfBrowserIsActive();

}  // namespace frequency_capping
}  // namespace features
}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FEATURES_FREQUENCY_CAPPING_FEATURES_H_
