/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_events/ad_notifications/ad_notification_event_clicked.h"

#include "bat/ads/confirmation_type.h"
#include "bat/ads/internal/ad_events/ad_events.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/base/logging_util.h"
#include "bat/ads/internal/creatives/ad_notifications/ad_notifications.h"
#include "bat/ads/internal/history/history.h"

namespace ads {
namespace ad_notifications {

AdEventClicked::AdEventClicked() = default;

AdEventClicked::~AdEventClicked() = default;

void AdEventClicked::FireEvent(const AdNotificationInfo& ad) {
  BLOG(3, "Clicked ad notification with placement id "
              << ad.placement_id << " and creative instance id "
              << ad.creative_instance_id);

  AdNotifications::Get()->Remove(ad.placement_id);

  AdsClientHelper::Get()->CloseNotification(ad.placement_id);

  LogAdEvent(ad, ConfirmationType::kClicked, [](const bool success) {
    if (!success) {
      BLOG(1, "Failed to log ad notification clicked event");
      return;
    }

    BLOG(1, "Successfully logged ad notification clicked event");
  });

  history::AddAdNotification(ad, ConfirmationType::kClicked);
}

}  // namespace ad_notifications
}  // namespace ads
