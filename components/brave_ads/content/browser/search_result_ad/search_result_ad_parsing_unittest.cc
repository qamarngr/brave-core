/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <utility>

#include "base/no_destructor.h"
#include "base/strings/string_number_conversions.h"
#include "brave/components/brave_ads/content/browser/search_result_ad/search_result_ad_parsing.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace brave_ads {

namespace {

schema_org::mojom::ValuesPtr CreateVectorValuesPtr(std::string value) {
  return schema_org::mojom::Values::NewStringValues({value});
}

schema_org::mojom::ValuesPtr CreateVectorValuesPtr(int64_t value) {
  return schema_org::mojom::Values::NewLongValues({value});
}

class WebPageConstructor final {
 public:
  explicit WebPageConstructor(int attribute_index_to_skip = -1)
      : attribute_index_to_skip_(attribute_index_to_skip) {
    web_page_ = CreateWebPage();
  }
  ~WebPageConstructor() = default;
  WebPageConstructor(const WebPageConstructor&) = delete;
  WebPageConstructor& operator=(const WebPageConstructor&) = delete;

  blink::mojom::WebPagePtr GetWebPage() { return std::move(web_page_); }

 private:
  blink::mojom::WebPagePtr CreateWebPage() {
    blink::mojom::WebPagePtr web_page = blink::mojom::WebPage::New();
    schema_org::mojom::EntityPtr entity = schema_org::mojom::Entity::New();
    entity->type = "Product";
    schema_org::mojom::PropertyPtr property =
        schema_org::mojom::Property::New();
    property->name = "creatives";

    std::vector<schema_org::mojom::EntityPtr> entity_values;
    entity_values.push_back(CreateCreativeEntity());
    property->values =
        schema_org::mojom::Values::NewEntityValues(std::move(entity_values));

    entity->properties.push_back(std::move(property));
    web_page->entities.push_back(std::move(entity));

    return web_page;
  }

  template <typename T>
  void AddStringProperty(
      std::vector<schema_org::mojom::PropertyPtr>* properties,
      base::StringPiece name,
      T value) {
    int index = current_attribute_index_++;
    if (index == attribute_index_to_skip_) {
      return;
    }
    schema_org::mojom::PropertyPtr property =
        schema_org::mojom::Property::New();
    property->name = static_cast<std::string>(name);
    property->values = CreateVectorValuesPtr(value);

    properties->push_back(std::move(property));
  }

  schema_org::mojom::EntityPtr CreateCreativeEntity() {
    const char* kSearchResultAdStringAttributes[] = {
        "data-placement-id",
        "data-creative-instance-id",
        "data-creative-set-id",
        "data-campaign-id",
        "data-advertiser-id",
        "data-headline-text",
        "data-description",
        "data-conversion-type-value",
        "data-conversion-url-pattern-value",
        "data-conversion-advertiser-public-key-value"};

    schema_org::mojom::EntityPtr entity = schema_org::mojom::Entity::New();
    entity->type = "SearchResultAd";

    AddStringProperty<std::string>(&entity->properties, "data-landing-page",
                                   "https://target.url");
    AddStringProperty<std::string>(&entity->properties, "data-rewards-value",
                                   "0.5");
    AddStringProperty<int64_t>(&entity->properties,
                               "data-conversion-observation-window-value", 1);

    int index = 0;
    for (const auto** it = std::begin(kSearchResultAdStringAttributes);
         it != std::end(kSearchResultAdStringAttributes); ++it, ++index) {
      AddStringProperty<std::string>(
          &entity->properties, *it,
          std::string("value") + base::NumberToString(index));
    }

    return entity;
  }

  blink::mojom::WebPagePtr web_page_;
  int current_attribute_index_ = 0;
  int attribute_index_to_skip_ = -1;
};

TEST(SearchResultAdParsingTest, ValidWebPage) {
  WebPageConstructor constructor;
  blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
  SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
  ASSERT_EQ(ads.size(), 1u);
  ads::mojom::SearchResultAdPtr& search_result_ad = ads[0];

  EXPECT_EQ(search_result_ad->target_url, GURL("https://target.url"));
  EXPECT_EQ(search_result_ad->value, 0.5);
  EXPECT_EQ(
      static_cast<size_t>(search_result_ad->conversion->observation_window),
      1u);

  EXPECT_EQ(search_result_ad->placement_id, "value0");
  EXPECT_EQ(search_result_ad->creative_instance_id, "value1");
  EXPECT_EQ(search_result_ad->creative_set_id, "value2");
  EXPECT_EQ(search_result_ad->campaign_id, "value3");
  EXPECT_EQ(search_result_ad->advertiser_id, "value4");
  EXPECT_EQ(search_result_ad->headline_text, "value5");
  EXPECT_EQ(search_result_ad->description, "value6");
  EXPECT_EQ(search_result_ad->conversion->type, "value7");
  EXPECT_EQ(search_result_ad->conversion->url_pattern, "value8");
  EXPECT_EQ(search_result_ad->conversion->advertiser_public_key, "value9");
}

TEST(SearchResultAdParsingTest, NotValidWebPage) {
  {
    blink::mojom::WebPagePtr web_page = blink::mojom::WebPage::New();
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    web_page->entities[0]->type = "Not-Product";
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    web_page->entities[0]->properties.clear();
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    property->name = "not-creatives";
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    property->values = schema_org::mojom::Values::NewEntityValues({});
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    property->values = schema_org::mojom::Values::NewStringValues({"creative"});
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }
}

TEST(SearchResultAdParsingTest, NotValidAdEntityExtraProperty) {
  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];
    ad_entity->type = "Not-SearchResultAd";
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    WebPageConstructor constructor;
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];

    schema_org::mojom::PropertyPtr extra_property =
        schema_org::mojom::Property::New();
    extra_property->name = "extra-name";
    extra_property->values =
        schema_org::mojom::Values::NewStringValues({"extra-value"});
    ad_entity->properties.push_back(std::move(extra_property));

    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }
}

TEST(SearchResultAdParsingTest, NotValidAdEntityPropertySkipped) {
  constexpr int kSearchResultAdAttributesCount = 12;
  for (int index = 0; index < kSearchResultAdAttributesCount; ++index) {
    WebPageConstructor constructor(index);
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }
}

TEST(SearchResultAdParsingTest, NotValidAdEntityWrongPropertyType) {
  {
    // Skip "data-landing-page".
    WebPageConstructor constructor(0);
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];

    schema_org::mojom::PropertyPtr extra_property =
        schema_org::mojom::Property::New();
    extra_property->name = "data-landing-page";
    extra_property->values =
        schema_org::mojom::Values::NewStringValues({"http://target.url"});
    ad_entity->properties.push_back(std::move(extra_property));

    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    // Skip "data-rewards-value".
    WebPageConstructor constructor(1);
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];

    schema_org::mojom::PropertyPtr extra_property =
        schema_org::mojom::Property::New();
    extra_property->name = "data-rewards-value";
    extra_property->values =
        schema_org::mojom::Values::NewStringValues({"0-5"});
    ad_entity->properties.push_back(std::move(extra_property));

    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    // Skip "data-conversion-observation-window-value".
    WebPageConstructor constructor(2);
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];

    schema_org::mojom::PropertyPtr extra_property =
        schema_org::mojom::Property::New();
    extra_property->name = "data-conversion-observation-window-value";
    extra_property->values = schema_org::mojom::Values::NewStringValues({"1"});
    ad_entity->properties.push_back(std::move(extra_property));

    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }

  {
    // Skip "data-placement-id".
    WebPageConstructor constructor(3);
    blink::mojom::WebPagePtr web_page = constructor.GetWebPage();
    auto& property = web_page->entities[0]->properties[0];
    auto& ad_entity = property->values->get_entity_values()[0];

    schema_org::mojom::PropertyPtr extra_property =
        schema_org::mojom::Property::New();
    extra_property->name = "data-placement-id";
    extra_property->values = schema_org::mojom::Values::NewLongValues({101});
    ad_entity->properties.push_back(std::move(extra_property));

    SearchResultAdsList ads = ParseWebPageEntities(std::move(web_page));
    EXPECT_TRUE(ads.empty());
  }
}

}  // namespace

}  // namespace brave_ads
