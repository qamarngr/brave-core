/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_PARSE_WEB_PAGE_ENTITIES_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_PARSE_WEB_PAGE_ENTITIES_H_

#include "brave/components/brave_ads/browser/search_result_ad/search_result_ad_aliases.h"
#include "third_party/blink/public/mojom/document_metadata/document_metadata.mojom.h"

namespace brave_ads {

SearchResultAdsList ParseWebPageEntities(blink::mojom::WebPagePtr web_page);

}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_PARSE_WEB_PAGE_ENTITIES_H_
