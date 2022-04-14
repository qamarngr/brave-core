/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/brave_ads/browser/search_result_ad/search_result_ad_aliases.h"
#include "brave/vendor/bat-native-ads/include/bat/ads/public/interfaces/ads.mojom.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "third_party/blink/public/mojom/document_metadata/document_metadata.mojom.h"

namespace content {
class RenderFrameHost;
}

namespace brave_ads {

class AdsService;

class SearchResultAdHandler final {
 public:
  explicit SearchResultAdHandler(AdsService* ads_service);
  virtual ~SearchResultAdHandler();

  SearchResultAdHandler(const SearchResultAdHandler&) = delete;
  SearchResultAdHandler& operator=(const SearchResultAdHandler&) = delete;

  void MaybeRetrieveSearchResultAd(content::RenderFrameHost* render_frame_host);

  void SetMetadataRequestFinishedCallbackForTesting(base::OnceClosure callback);

 private:
  void OnRetrieveSearchResultAdEntities(mojo::RemoteSetElementId id,
                                        blink::mojom::WebPagePtr web_page);

  void TriggerSearchResultAdViewedEvents(SearchResultAdsList search_result_ads);

  void OnTriggerSearchResultAdViewedEvents(
      SearchResultAdsList search_result_ads,
      bool success,
      const std::string& placement_id,
      ads::mojom::SearchResultAdEventType ad_event_type);

  mojo::RemoteSet<blink::mojom::DocumentMetadata> document_metadata_remote_set_;

  raw_ptr<AdsService> ads_service_ = nullptr;

  base::OnceClosure metadata_request_finished_callback_for_testing_;

  base::WeakPtrFactory<SearchResultAdHandler> weak_factory_{this};
};

}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_SEARCH_RESULT_AD_SEARCH_RESULT_AD_HANDLER_H_
