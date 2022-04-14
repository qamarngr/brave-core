/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/browser/search_result_ad/search_result_ad_handler.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "base/callback.h"
#include "base/feature_list.h"
#include "brave/components/brave_ads/browser/ads_service.h"
#include "brave/components/brave_ads/browser/search_result_ad/parse_web_page_entities.h"
#include "brave/components/brave_ads/common/features.h"
#include "brave/components/brave_search/common/brave_search_utils.h"
#include "content/public/browser/render_frame_host.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/service_manager/public/cpp/interface_provider.h"

namespace brave_ads {

SearchResultAdHandler::SearchResultAdHandler(AdsService* ads_service)
    : ads_service_(ads_service) {}

SearchResultAdHandler::~SearchResultAdHandler() = default;

void SearchResultAdHandler::MaybeRetrieveSearchResultAd(
    content::RenderFrameHost* render_frame_host) {
  DCHECK(ads_service_);
  DCHECK(render_frame_host);

  if (!ads_service_->IsEnabled() ||
      !base::FeatureList::IsEnabled(
          features::kSupportBraveSearchResultAdConfirmationEvents) ||
      !brave_search::IsAllowedHost(render_frame_host->GetLastCommittedURL())) {
    if (metadata_request_finished_callback_for_testing_) {
      std::move(metadata_request_finished_callback_for_testing_).Run();
    }
    return;
  }

  mojo::Remote<blink::mojom::DocumentMetadata> remote;
  render_frame_host->GetRemoteInterfaces()->GetInterface(
      remote.BindNewPipeAndPassReceiver());
  DCHECK(remote.is_bound());

  mojo::RemoteSetElementId id =
      document_metadata_remote_set_.Add(std::move(remote));
  blink::mojom::DocumentMetadata* document_metadata =
      document_metadata_remote_set_.Get(id);
  DCHECK(document_metadata);
  document_metadata->GetEntities(
      base::BindOnce(&SearchResultAdHandler::OnRetrieveSearchResultAdEntities,
                     base::Unretained(this), id));
}

void SearchResultAdHandler::SetMetadataRequestFinishedCallbackForTesting(
    base::OnceClosure callback) {
  metadata_request_finished_callback_for_testing_ = std::move(callback);
}

void SearchResultAdHandler::OnRetrieveSearchResultAdEntities(
    mojo::RemoteSetElementId id,
    blink::mojom::WebPagePtr web_page) {
  if (metadata_request_finished_callback_for_testing_) {
    std::move(metadata_request_finished_callback_for_testing_).Run();
  }

  document_metadata_remote_set_.Remove(id);

  if (!web_page) {
    return;
  }

  SearchResultAdsList search_result_ads =
      ParseWebPageEntities(std::move(web_page));

  std::reverse(search_result_ads.begin(), search_result_ads.end());
  TriggerSearchResultAdViewedEvents(std::move(search_result_ads));
}

void SearchResultAdHandler::TriggerSearchResultAdViewedEvents(
    SearchResultAdsList search_result_ads) {
  DCHECK(ads_service_);
  if (search_result_ads.empty()) {
    return;
  }

  ads::mojom::SearchResultAdPtr search_result_ad =
      std::move(search_result_ads.back());
  search_result_ads.pop_back();

  ads_service_->TriggerSearchResultAdEvent(
      std::move(search_result_ad), ads::mojom::SearchResultAdEventType::kViewed,
      base::BindOnce(
          &SearchResultAdHandler::OnTriggerSearchResultAdViewedEvents,
          weak_factory_.GetWeakPtr(), std::move(search_result_ads)));
}

void SearchResultAdHandler::OnTriggerSearchResultAdViewedEvents(
    SearchResultAdsList search_result_ads,
    const bool success,
    const std::string& placement_id,
    ads::mojom::SearchResultAdEventType ad_event_type) {
  DCHECK_EQ(ad_event_type, ads::mojom::SearchResultAdEventType::kViewed);
  if (!success) {
    VLOG(1) << "Failed to trigger search result ad viewed event for "
            << placement_id;
    return;
  }

  TriggerSearchResultAdViewedEvents(std::move(search_result_ads));
}

}  // namespace brave_ads
