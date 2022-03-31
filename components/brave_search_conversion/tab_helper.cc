/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_search_conversion/tab_helper.h"

#include "base/feature_list.h"
#include "brave/components/brave_search_conversion/features.h"

namespace brave_search_conversion {

// static
// TODO(simonhong): Get more params.
// ex) is_first_run, template_url_service to know current provider.
bool TabHelper::MaybeCreateForWebContents(content::WebContents* web_contents) {
  // TODO(simonhong): Return false forever for new user because brave search
  // will be set as a default.
  // TODO(simonhong): Return false when user already handled conversion ask.
  // TODO(simonhong): Return false when brave search is already default.
  if (!base::FeatureList::IsEnabled(features::kConversionTypeA) &&
      !base::FeatureList::IsEnabled(features::kConversionTypeB)) {
    return false;
  }

  CreateForWebContents(web_contents);
  return true;
}

TabHelper::TabHelper(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      content::WebContentsUserData<TabHelper>(*web_contents) {}

TabHelper::~TabHelper() = default;

WEB_CONTENTS_USER_DATA_KEY_IMPL(TabHelper);

}  // namespace brave_search_conversion
