/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/path_service.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/test/scoped_feature_list.h"
#include "base/time/time.h"
#include "brave/common/brave_paths.h"
#include "brave/components/brave_ads/browser/mock_ads_service.h"
#include "brave/components/brave_ads/browser/search_result_ad/search_result_ad_handler.h"
#include "brave/components/brave_ads/common/features.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/content_mock_cert_verifier.h"
#include "net/dns/mock_host_resolver.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_browser_tests --filter=SearchResultAdTest*

using testing::_;
using testing::Mock;
using testing::Return;

namespace {

constexpr char kAllowedDomain[] = "search.brave.com";
constexpr char kNotAllowedDomain[] = "brave.com";

void CheckSampleSearchAdMetadata(
    const ads::mojom::SearchResultAdPtr& search_result_ad,
    size_t ad_index) {
  const std::string index = base::StrCat({"-", base::NumberToString(ad_index)});
  EXPECT_EQ(search_result_ad->placement_id,
            base::StrCat({"data-placement-id", index}));
  EXPECT_EQ(search_result_ad->creative_instance_id,
            base::StrCat({"data-creative-instance-id", index}));
  EXPECT_EQ(search_result_ad->creative_set_id,
            base::StrCat({"data-creative-set-id", index}));
  EXPECT_EQ(search_result_ad->campaign_id,
            base::StrCat({"data-campaign-id", index}));
  EXPECT_EQ(search_result_ad->advertiser_id,
            base::StrCat({"data-advertiser-id", index}));
  EXPECT_EQ(search_result_ad->target_url,
            base::StrCat({"data-landing-page", index}));
  EXPECT_EQ(search_result_ad->headline_text,
            base::StrCat({"data-headline-text", index}));
  EXPECT_EQ(search_result_ad->description,
            base::StrCat({"data-description", index}));
  EXPECT_EQ(search_result_ad->value, 0.5 + ad_index);

  EXPECT_EQ(search_result_ad->conversion->type,
            base::StrCat({"data-conversion-type-value", index}));
  EXPECT_EQ(search_result_ad->conversion->url_pattern,
            base::StrCat({"data-conversion-url-pattern-value", index}));
  EXPECT_EQ(
      search_result_ad->conversion->advertiser_public_key,
      base::StrCat({"data-conversion-advertiser-public-key-value", index}));
  EXPECT_EQ(
      static_cast<size_t>(search_result_ad->conversion->observation_window),
      ad_index);
}

}  // namespace

class SearchResultAdTest : public InProcessBrowserTest {
 public:
  SearchResultAdTest() {
    feature_list_.InitAndEnableFeature(
        brave_ads::features::kSupportBraveSearchResultAdConfirmationEvents);
  }

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();
    mock_cert_verifier_.mock_cert_verifier()->set_default_result(net::OK);
    host_resolver()->AddRule("*", "127.0.0.1");

    https_server_.reset(new net::EmbeddedTestServer(
        net::test_server::EmbeddedTestServer::TYPE_HTTPS));

    brave::RegisterPathProvider();
    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    https_server_->ServeFilesFromDirectory(test_data_dir);
    ASSERT_TRUE(https_server_->Start());
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    InProcessBrowserTest::SetUpCommandLine(command_line);
    mock_cert_verifier_.SetUpCommandLine(command_line);
  }

  void SetUpInProcessBrowserTestFixture() override {
    InProcessBrowserTest::SetUpInProcessBrowserTestFixture();
    mock_cert_verifier_.SetUpInProcessBrowserTestFixture();
  }

  void TearDownInProcessBrowserTestFixture() override {
    mock_cert_verifier_.TearDownInProcessBrowserTestFixture();
    InProcessBrowserTest::TearDownInProcessBrowserTestFixture();
  }

  net::EmbeddedTestServer* https_server() { return https_server_.get(); }

  content::WebContents* LoadTestDataUrl(base::StringPiece domain,
                                        const std::string& relative_path) {
    GURL url = https_server()->GetURL(domain, relative_path);

    content::WebContents* web_contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    EXPECT_TRUE(content::NavigateToURL(web_contents, url));
    EXPECT_EQ(url, web_contents->GetVisibleURL());

    return web_contents;
  }

 private:
  base::test::ScopedFeatureList feature_list_;
  content::ContentMockCertVerifier mock_cert_verifier_;
  std::unique_ptr<net::EmbeddedTestServer> https_server_;
};

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, SampleSearchAdMetadata) {
  content::WebContents* web_contents = LoadTestDataUrl(
      kAllowedDomain, "/brave_ads/search_result_ad_sample.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  brave_ads::TriggerSearchResultAdEventCallback trigger_callback;

  auto run_loop = std::make_unique<base::RunLoop>();
  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(true));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _))
      .WillOnce([&run_loop, &trigger_callback](
                    ads::mojom::SearchResultAdPtr ad_mojom,
                    const ads::mojom::SearchResultAdEventType event_type,
                    brave_ads::TriggerSearchResultAdEventCallback callback) {
        CheckSampleSearchAdMetadata(ad_mojom, 1);
        trigger_callback = std::move(callback);
        run_loop->Quit();
      });

  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop->Run();
  Mock::VerifyAndClearExpectations(&ads_service);

  run_loop = std::make_unique<base::RunLoop>();
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _))
      .WillOnce([&run_loop, &trigger_callback](
                    ads::mojom::SearchResultAdPtr ad_mojom,
                    const ads::mojom::SearchResultAdEventType event_type,
                    brave_ads::TriggerSearchResultAdEventCallback callback) {
        CheckSampleSearchAdMetadata(ad_mojom, 2);
        trigger_callback = std::move(callback);
        run_loop->Quit();
      });

  std::move(trigger_callback)
      .Run(true, "placement-id-1",
           ads::mojom::SearchResultAdEventType::kViewed);
  run_loop->Run();
  Mock::VerifyAndClearExpectations(&ads_service);

  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);
  std::move(trigger_callback)
      .Run(true, "placement-id-2",
           ads::mojom::SearchResultAdEventType::kViewed);
}

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, FailedToTriggerSearchResultAdEvent) {
  content::WebContents* web_contents = LoadTestDataUrl(
      kAllowedDomain, "/brave_ads/search_result_ad_sample.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  brave_ads::TriggerSearchResultAdEventCallback trigger_callback;

  auto run_loop = std::make_unique<base::RunLoop>();
  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(true));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _))
      .WillOnce([&run_loop, &trigger_callback](
                    ads::mojom::SearchResultAdPtr ad_mojom,
                    const ads::mojom::SearchResultAdEventType event_type,
                    brave_ads::TriggerSearchResultAdEventCallback callback) {
        CheckSampleSearchAdMetadata(ad_mojom, 1);
        trigger_callback = std::move(callback);
        run_loop->Quit();
      });

  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop->Run();
  Mock::VerifyAndClearExpectations(&ads_service);

  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);
  std::move(trigger_callback)
      .Run(false, "placement-id-1",
           ads::mojom::SearchResultAdEventType::kViewed);
}

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, AdsDisabled) {
  content::WebContents* web_contents = LoadTestDataUrl(
      kAllowedDomain, "/brave_ads/search_result_ad_sample.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(false));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);

  base::RunLoop run_loop;
  search_result_ad_metadata_handler
      .SetMetadataRequestFinishedCallbackForTesting(base::BindOnce(
          [](base::OnceClosure run_loop_closure) {
            std::move(run_loop_closure).Run();
          },
          run_loop.QuitClosure()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, NotAllowedDomain) {
  content::WebContents* web_contents = LoadTestDataUrl(
      kNotAllowedDomain, "/brave_ads/search_result_ad_sample.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(true));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);

  base::RunLoop run_loop;
  search_result_ad_metadata_handler
      .SetMetadataRequestFinishedCallbackForTesting(base::BindOnce(
          [](base::OnceClosure run_loop_closure) {
            std::move(run_loop_closure).Run();
          },
          run_loop.QuitClosure()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, NoSearchAdMetadata) {
  content::WebContents* web_contents =
      LoadTestDataUrl(kAllowedDomain, "/simple.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(true));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);

  base::RunLoop run_loop;
  search_result_ad_metadata_handler
      .SetMetadataRequestFinishedCallbackForTesting(base::BindOnce(
          [](base::OnceClosure run_loop_closure) {
            std::move(run_loop_closure).Run();
          },
          run_loop.QuitClosure()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(SearchResultAdTest, BrokenSearchAdMetadata) {
  content::WebContents* web_contents = LoadTestDataUrl(
      kAllowedDomain, "/brave_ads/search_result_ad_broken.html");

  brave_ads::MockAdsService ads_service;
  brave_ads::SearchResultAdHandler search_result_ad_metadata_handler(
      &ads_service);

  EXPECT_CALL(ads_service, IsEnabled()).WillOnce(Return(true));
  EXPECT_CALL(ads_service, TriggerSearchResultAdEvent(_, _, _)).Times(0);

  base::RunLoop run_loop;
  search_result_ad_metadata_handler
      .SetMetadataRequestFinishedCallbackForTesting(base::BindOnce(
          [](base::OnceClosure run_loop_closure) {
            std::move(run_loop_closure).Run();
          },
          run_loop.QuitClosure()));

  content::RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  search_result_ad_metadata_handler.MaybeRetrieveSearchResultAd(
      render_frame_host);

  run_loop.Run();
}
