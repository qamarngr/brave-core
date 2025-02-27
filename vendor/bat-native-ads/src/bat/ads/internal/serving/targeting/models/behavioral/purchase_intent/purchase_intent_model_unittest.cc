/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/serving/targeting/models/behavioral/purchase_intent/purchase_intent_model.h"

#include "bat/ads/internal/base/unittest_base.h"
#include "bat/ads/internal/base/unittest_util.h"
#include "bat/ads/internal/resources/behavioral/purchase_intent/purchase_intent_resource.h"
#include "bat/ads/internal/targeting/processors/behavioral/purchase_intent/purchase_intent_processor.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {
namespace targeting {
namespace model {

class BatAdsPurchaseIntentModelTest : public UnitTestBase {
 protected:
  BatAdsPurchaseIntentModelTest() = default;

  ~BatAdsPurchaseIntentModelTest() override = default;
};

TEST_F(BatAdsPurchaseIntentModelTest, DoNotGetSegmentsForUnitializedResource) {
  // Arrange
  resource::PurchaseIntent resource;
  processor::PurchaseIntent processor(&resource);

  const GURL url = GURL("https://www.brave.com/test?foo=bar");
  processor.Process(url);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest, DoNotGetSegmentsForExpiredSignals) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  processor::PurchaseIntent processor(&resource);

  const GURL url_1 = GURL("https://www.brave.com/test?foo=bar");
  processor.Process(url_1);

  FastForwardClockBy(base::Days(1));

  const GURL url_2 = GURL("https://www.basicattentiontoken.org/test?bar=foo");
  processor.Process(url_2);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest, DoNotGetSegmentsIfNeverProcessed) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest,
       DoNotGetSegmentsIfNeverMatchedFunnelSites) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  processor::PurchaseIntent processor(&resource);

  const GURL url = GURL("https://duckduckgo.com/?q=segment+keyword+1");
  processor.Process(url);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest, GetSegmentsForPreviouslyMatchedSite) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  processor::PurchaseIntent processor(&resource);

  const GURL url_1 = GURL("https://www.brave.com/test?foo=bar");
  processor.Process(url_1);

  const GURL url_2 = GURL("https://www.basicattentiontoken.org/test?bar=foo");
  processor.Process(url_2);

  processor.Process(url_1);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {"segment 3", "segment 2"};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest,
       GetSegmentsForPreviouslyMatchedSegmentKeywords) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  processor::PurchaseIntent processor(&resource);

  const GURL url = GURL("https://duckduckgo.com/?q=segment+keyword+1&foo=bar");
  processor.Process(url);
  processor.Process(url);
  processor.Process(url);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {"segment 1"};

  EXPECT_EQ(expected_segments, segments);
}

TEST_F(BatAdsPurchaseIntentModelTest,
       GetSegmentsForPreviouslyMatchedFunnelKeywords) {
  // Arrange
  resource::PurchaseIntent resource;
  resource.Load();
  task_environment()->RunUntilIdle();

  processor::PurchaseIntent processor(&resource);

  const GURL url =
      GURL("https://duckduckgo.com/?q=segment+keyword+1+funnel+keyword+2");
  processor.Process(url);

  // Act
  PurchaseIntent model;
  const SegmentList segments = model.GetSegments();

  // Assert
  const SegmentList expected_segments = {"segment 1"};

  EXPECT_EQ(expected_segments, segments);
}

}  // namespace model
}  // namespace targeting
}  // namespace ads
