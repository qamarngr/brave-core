/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TARGETING_DATA_TYPES_CONTEXTUAL_TEXT_CLASSIFICATION_TEXT_CLASSIFICATION_ALIASES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TARGETING_DATA_TYPES_CONTEXTUAL_TEXT_CLASSIFICATION_TEXT_CLASSIFICATION_ALIASES_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/containers/circular_deque.h"

namespace ads {
namespace targeting {

using TextClassificationProbabilitiesMap = std::map<std::string, double>;
using TextClassificationProbabilitiesList =
    base::circular_deque<TextClassificationProbabilitiesMap>;

using SegmentProbabilityPair = std::pair<std::string, double>;
using SegmentProbabilitiesList = std::vector<SegmentProbabilityPair>;
using SegmentProbabilitiesMap = std::map<std::string, double>;

}  // namespace targeting
}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_TARGETING_DATA_TYPES_CONTEXTUAL_TEXT_CLASSIFICATION_TEXT_CLASSIFICATION_ALIASES_H_
