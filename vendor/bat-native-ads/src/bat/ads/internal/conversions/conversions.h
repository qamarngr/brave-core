/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CONVERSIONS_CONVERSIONS_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CONVERSIONS_CONVERSIONS_H_

#include <string>
#include <vector>

#include "base/observer_list.h"
#include "bat/ads/ads_client_aliases.h"
#include "bat/ads/internal/base/timer.h"
#include "bat/ads/internal/conversions/conversion_info_aliases.h"
#include "bat/ads/internal/conversions/conversions_observer.h"
#include "bat/ads/internal/resources/behavioral/conversions/conversion_id_pattern_info_aliases.h"

class GURL;

namespace ads {

struct AdEventInfo;
struct ConversionQueueItemInfo;
struct VerifiableConversionInfo;

class Conversions final {
 public:
  Conversions();
  ~Conversions();

  void AddObserver(ConversionsObserver* observer);
  void RemoveObserver(ConversionsObserver* observer);

  bool ShouldAllow() const;

  void MaybeConvert(const std::vector<GURL>& redirect_chain,
                    const std::string& html,
                    const ConversionIdPatternMap& conversion_id_patterns);

  void StartTimerIfReady();

 private:
  void CheckRedirectChain(const std::vector<GURL>& redirect_chain,
                          const std::string& html,
                          const ConversionIdPatternMap& conversion_id_patterns);

  void Convert(const AdEventInfo& ad_event,
               const VerifiableConversionInfo& verifiable_conversion);

  ConversionList FilterConversions(const std::vector<GURL>& redirect_chain,
                                   const ConversionList& conversions);
  ConversionList SortConversions(const ConversionList& conversions);

  void AddItemToQueue(const AdEventInfo& ad_event,
                      const VerifiableConversionInfo& verifiable_conversion);

  void ProcessQueueItem(const ConversionQueueItemInfo& queue_item);
  void ProcessQueue();

  void RemoveInvalidQueueItem(
      const ConversionQueueItemInfo& conversion_queue_item,
      ResultCallback callback);
  void MarkQueueItemAsProcessed(
      const ConversionQueueItemInfo& conversion_queue_item,
      ResultCallback callback);
  void FailedToConvertQueueItem(
      const ConversionQueueItemInfo& conversion_queue_item);
  void ConvertedQueueItem(const ConversionQueueItemInfo& conversion_queue_item);

  void StartTimer(const ConversionQueueItemInfo& queue_item);

  void NotifyConversion(
      const ConversionQueueItemInfo& conversion_queue_item) const;
  void NotifyConversionFailed(
      const ConversionQueueItemInfo& conversion_queue_item) const;

  base::ObserverList<ConversionsObserver> observers_;

  Timer timer_;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_CONVERSIONS_CONVERSIONS_H_
