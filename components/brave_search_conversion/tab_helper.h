/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_TAB_HELPER_H_
#define BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_TAB_HELPER_H_

#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}  // namespace content

namespace brave_search_conversion {

class TabHelper : public content::WebContentsObserver,
                  public content::WebContentsUserData<TabHelper> {
 public:
  static bool MaybeCreateForWebContents(content::WebContents* web_contents);

  TabHelper(const TabHelper&) = delete;
  TabHelper& operator=(const TabHelper&) = delete;

  ~TabHelper() override;

 private:
  friend class content::WebContentsUserData<TabHelper>;

  explicit TabHelper(content::WebContents* web_contents);

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace brave_search_conversion

#endif  // BRAVE_COMPONENTS_BRAVE_SEARCH_CONVERSION_TAB_HELPER_H_
