/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/serving/new_tab_page_ads/new_tab_page_ad_serving.h"

#include "base/check.h"
#include "base/rand_util.h"
#include "bat/ads/ad_type.h"
#include "bat/ads/internal/ad_server/catalog/bundle/creative_new_tab_page_ad_info.h"
#include "bat/ads/internal/base/logging_util.h"
#include "bat/ads/internal/creatives/new_tab_page_ads/new_tab_page_ad_builder.h"
#include "bat/ads/internal/creatives/new_tab_page_ads/new_tab_page_ad_permission_rules.h"
#include "bat/ads/internal/eligible_ads/new_tab_page_ads/eligible_new_tab_page_ads_base.h"
#include "bat/ads/internal/eligible_ads/new_tab_page_ads/eligible_new_tab_page_ads_factory.h"
#include "bat/ads/internal/resources/behavioral/anti_targeting/anti_targeting_resource.h"
#include "bat/ads/internal/serving/new_tab_page_ads/new_tab_page_ads_features.h"
#include "bat/ads/internal/serving/serving_features.h"
#include "bat/ads/internal/serving/targeting/geographic/subdivision/subdivision_targeting.h"
#include "bat/ads/internal/targeting/targeting_user_model_builder.h"
#include "bat/ads/internal/targeting/targeting_user_model_info.h"
#include "bat/ads/new_tab_page_ad_info.h"

namespace ads {
namespace new_tab_page_ads {

Serving::Serving(
    targeting::geographic::SubdivisionTargeting* subdivision_targeting,
    resource::AntiTargeting* anti_targeting_resource) {
  DCHECK(subdivision_targeting);
  DCHECK(anti_targeting_resource);

  const int version = ::ads::features::GetServingVersion();
  eligible_ads_ = EligibleAdsFactory::Build(version, subdivision_targeting,
                                            anti_targeting_resource);
}

Serving::~Serving() = default;

void Serving::AddObserver(NewTabPageServingObserver* observer) {
  DCHECK(observer);
  observers_.AddObserver(observer);
}

void Serving::RemoveObserver(NewTabPageServingObserver* observer) {
  DCHECK(observer);
  observers_.RemoveObserver(observer);
}

void Serving::MaybeServeAd(GetNewTabPageAdCallback callback) {
  if (!features::IsEnabled()) {
    BLOG(1, "New tab page ad not served: Feature is disabled");
    FailedToServeAd(callback);
    return;
  }

  if (!IsSupported()) {
    BLOG(1, "New tab page ad not served: Unsupported version");
    FailedToServeAd(callback);
    return;
  }

  frequency_capping::PermissionRules permission_rules;
  if (!permission_rules.HasPermission()) {
    BLOG(1, "New tab page ad not served: Not allowed due to permission rules");
    FailedToServeAd(callback);
    return;
  }

  const targeting::UserModelInfo& user_model = targeting::BuildUserModel();

  DCHECK(eligible_ads_);
  eligible_ads_->GetForUserModel(
      user_model, [=](const bool had_opportunity,
                      const CreativeNewTabPageAdList& creative_ads) {
        if (creative_ads.empty()) {
          BLOG(1, "New tab page ad not served: No eligible ads found");
          FailedToServeAd(callback);
          return;
        }

        BLOG(1, "Found " << creative_ads.size() << " eligible ads");

        const int rand = base::RandInt(0, creative_ads.size() - 1);
        const CreativeNewTabPageAdInfo& creative_ad = creative_ads.at(rand);

        const NewTabPageAdInfo& ad = BuildNewTabPageAd(creative_ad);
        if (!ServeAd(ad, callback)) {
          BLOG(1, "Failed to serve new tab page ad");
          FailedToServeAd(callback);
          return;
        }

        BLOG(1, "Served new tab page ad");
        ServedAd(ad);
      });
}

///////////////////////////////////////////////////////////////////////////////

bool Serving::IsSupported() const {
  if (!eligible_ads_) {
    return false;
  }

  return true;
}

bool Serving::ServeAd(const NewTabPageAdInfo& ad,
                      GetNewTabPageAdCallback callback) const {
  DCHECK(ad.IsValid());

  // TODO(https://github.com/brave/brave-browser/issues/14015): Add logging for
  // wallpapers
  BLOG(1, "Serving new tab page ad:\n"
              << "  placementId: " << ad.placement_id << "\n"
              << "  creativeInstanceId: " << ad.creative_instance_id << "\n"
              << "  creativeSetId: " << ad.creative_set_id << "\n"
              << "  campaignId: " << ad.campaign_id << "\n"
              << "  advertiserId: " << ad.advertiser_id << "\n"
              << "  segment: " << ad.segment << "\n"
              << "  companyName: " << ad.company_name << "\n"
              << "  imageUrl: " << ad.image_url << "\n"
              << "  alt: " << ad.alt << "\n"
              << "  targetUrl: " << ad.target_url);

  callback(/* success */ true, ad);

  NotifyDidServeNewTabPageAd(ad);

  return true;
}

void Serving::FailedToServeAd(GetNewTabPageAdCallback callback) {
  callback(/* success */ false, {});

  NotifyFailedToServeNewTabPageAd();
}

void Serving::ServedAd(const NewTabPageAdInfo& ad) {
  DCHECK(eligible_ads_);
  eligible_ads_->set_last_served_ad(ad);
}

void Serving::NotifyDidServeNewTabPageAd(const NewTabPageAdInfo& ad) const {
  for (NewTabPageServingObserver& observer : observers_) {
    observer.OnDidServeNewTabPageAd(ad);
  }
}

void Serving::NotifyFailedToServeNewTabPageAd() const {
  for (NewTabPageServingObserver& observer : observers_) {
    observer.OnFailedToServeNewTabPageAd();
  }
}

}  // namespace new_tab_page_ads
}  // namespace ads
