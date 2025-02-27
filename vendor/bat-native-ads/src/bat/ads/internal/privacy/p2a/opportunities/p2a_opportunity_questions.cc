/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/privacy/p2a/opportunities/p2a_opportunity_questions.h"

#include "base/check.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "bat/ads/internal/base/string_util.h"
#include "bat/ads/internal/segments/segments_util.h"

namespace ads {
namespace privacy {
namespace p2a {

namespace {
constexpr char kQuestionPrefix[] = "Brave.P2A.AdOpportunitiesPerSegment.";
}  // namespace

std::vector<std::string> CreateAdOpportunityQuestions(
    const SegmentList& segments) {
  std::vector<std::string> questions;

  const std::vector<std::string> parent_segments = GetParentSegments(segments);

  for (auto& segment : parent_segments) {
    DCHECK(!segment.empty());

    std::string stripped_segment = StripNonAlphaNumericCharacters(segment);

    base::ReplaceChars(stripped_segment, " ", "", &stripped_segment);
    DCHECK(!stripped_segment.empty());

    const std::string question =
        base::StringPrintf("%s%s", kQuestionPrefix, stripped_segment.c_str());

    questions.push_back(question);
  }

  questions.push_back("Brave.P2A.TotalAdOpportunities");

  return questions;
}

}  // namespace p2a
}  // namespace privacy
}  // namespace ads
