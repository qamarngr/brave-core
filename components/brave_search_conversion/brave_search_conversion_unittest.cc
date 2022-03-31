/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/feature_list.h"
#include "brave/components/brave_search_conversion/features.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace brave_search_conversion {

TEST(BraveSearchConversionTest, FeatureTest) {
  // All disabled by default. They'll be controlled via griffin.
  EXPECT_FALSE(base::FeatureList::IsEnabled(features::kConversionTypeA));
  EXPECT_FALSE(base::FeatureList::IsEnabled(features::kConversionTypeB));
}

}  // namespace brave_search_conversion
