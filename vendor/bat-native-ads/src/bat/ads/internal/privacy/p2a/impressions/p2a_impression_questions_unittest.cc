/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/privacy/p2a/impressions/p2a_impression_questions.h"

#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {
namespace privacy {
namespace p2a {

TEST(BatAdsP2AAdImpressionQuestionTest, CreateAdImpressionQuestions) {
  // Arrange
  const std::string segment = "technology & computing-software";

  // Act
  const std::vector<std::string> questions =
      CreateAdImpressionQuestions(segment);

  // Assert
  const std::vector<std::string> expected_questions = {
      "Brave.P2A.AdImpressionsPerSegment.technologycomputing",
      "Brave.P2A.TotalAdImpressions"};

  EXPECT_EQ(expected_questions, questions);
}

}  // namespace p2a
}  // namespace privacy
}  // namespace ads
