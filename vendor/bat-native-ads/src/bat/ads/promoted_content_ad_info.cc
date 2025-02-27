/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/promoted_content_ad_info.h"

#include "bat/ads/internal/base/logging_util.h"
#include "bat/ads/internal/deprecated/json_helper.h"

namespace ads {

PromotedContentAdInfo::PromotedContentAdInfo() = default;

PromotedContentAdInfo::PromotedContentAdInfo(
    const PromotedContentAdInfo& info) = default;

PromotedContentAdInfo::~PromotedContentAdInfo() = default;

bool PromotedContentAdInfo::IsValid() const {
  if (!AdInfo::IsValid()) {
    return false;
  }

  if (title.empty() || description.empty()) {
    return false;
  }

  return true;
}

std::string PromotedContentAdInfo::ToJson() const {
  std::string json;
  SaveToJson(*this, &json);
  return json;
}

bool PromotedContentAdInfo::FromJson(const std::string& json) {
  rapidjson::Document document;
  document.Parse(json.c_str());

  if (document.HasParseError()) {
    BLOG(1, helper::JSON::GetLastError(&document));
    return false;
  }

  if (document.HasMember("type")) {
    type = AdType(document["type"].GetString());
  }

  if (document.HasMember("uuid")) {
    placement_id = document["uuid"].GetString();
  }

  if (document.HasMember("creative_instance_id")) {
    creative_instance_id = document["creative_instance_id"].GetString();
  }

  if (document.HasMember("creative_set_id")) {
    creative_set_id = document["creative_set_id"].GetString();
  }

  if (document.HasMember("campaign_id")) {
    campaign_id = document["campaign_id"].GetString();
  }

  if (document.HasMember("advertiser_id")) {
    advertiser_id = document["advertiser_id"].GetString();
  }

  if (document.HasMember("segment")) {
    segment = document["segment"].GetString();
  }

  if (document.HasMember("title")) {
    title = document["title"].GetString();
  }

  if (document.HasMember("description")) {
    description = document["description"].GetString();
  }

  if (document.HasMember("target_url")) {
    target_url = GURL(document["target_url"].GetString());
  }

  return true;
}

void SaveToJson(JsonWriter* writer, const PromotedContentAdInfo& info) {
  writer->StartObject();

  writer->String("type");
  writer->String(info.type.ToString().c_str());

  writer->String("uuid");
  writer->String(info.placement_id.c_str());

  writer->String("creative_instance_id");
  writer->String(info.creative_instance_id.c_str());

  writer->String("creative_set_id");
  writer->String(info.creative_set_id.c_str());

  writer->String("campaign_id");
  writer->String(info.campaign_id.c_str());

  writer->String("advertiser_id");
  writer->String(info.advertiser_id.c_str());

  writer->String("segment");
  writer->String(info.segment.c_str());

  writer->String("title");
  writer->String(info.title.c_str());

  writer->String("description");
  writer->String(info.description.c_str());

  writer->String("target_url");
  writer->String(info.target_url.spec().c_str());

  writer->EndObject();
}

}  // namespace ads
