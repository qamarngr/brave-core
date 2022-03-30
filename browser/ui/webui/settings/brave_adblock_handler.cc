/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_adblock_handler.h"

#include <string>

#include "base/bind.h"
#include "base/values.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/components/brave_adblock/resources/grit/brave_adblock_generated_map.h"
#include "brave/components/brave_shields/browser/ad_block_custom_filters_provider.h"
#include "brave/components/brave_shields/browser/ad_block_regional_service_manager.h"
#include "brave/components/brave_shields/browser/ad_block_service.h"
#include "brave/components/brave_shields/browser/ad_block_service_helper.h"
#include "brave/components/brave_shields/browser/ad_block_subscription_service_manager.h"
#include "brave/components/brave_shields/browser/ad_block_subscription_service_manager_observer.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/web_ui.h"

void BraveAdBlockHandler::RegisterMessages() {
  profile_ = Profile::FromWebUI(web_ui());
  web_ui()->RegisterMessageCallback(
      "brave_adblock.getRegionalLists",
      base::BindRepeating(&BraveAdBlockHandler::GetRegionalLists,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "brave_adblock.enableFilterList",
      base::BindRepeating(&BraveAdBlockHandler::EnableFilterList,
                          base::Unretained(this)));
}

void BraveAdBlockHandler::GetRegionalLists(base::Value::ConstListView args) {
  AllowJavascript();
  auto callback_id = args[0].GetString();

  std::unique_ptr<base::ListValue> regional_lists =
      g_brave_browser_process->ad_block_service()
          ->regional_service_manager()
          ->GetRegionalLists();

  ResolveJavascriptCallback(base::Value(callback_id), regional_lists->Clone());
}

void BraveAdBlockHandler::EnableFilterList(base::Value::ConstListView args) {
  DCHECK_EQ(args.size(), 2U);

  if (!args[0].is_string() || !args[1].is_bool())
    return;

  std::string uuid = args[0].GetString();
  bool enabled = args[1].GetBool();

  g_brave_browser_process->ad_block_service()
      ->regional_service_manager()
      ->EnableFilterList(uuid, enabled);
}
