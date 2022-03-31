/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_FEATURES_H_
#define BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_FEATURES_H_

namespace base {
struct Feature;
}  // namespace base

namespace brave_search_conversion {

namespace features {

// We'll test A and B types separately and find more effective approach.
extern const base::Feature kConversionTypeA;
extern const base::Feature kConversionTypeB;

}  // namespace features

}  // namespace brave_search_conversion

#endif  // BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_FEATURES_H_
