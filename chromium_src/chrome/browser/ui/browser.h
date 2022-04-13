/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_BROWSER_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_BROWSER_H_

#define ScheduleUIUpdate virtual ScheduleUIUpdate
#define ShouldDisplayFavicon virtual ShouldDisplayFavicon
#define TryToCloseWindow virtual TryToCloseWindow
#define ShouldCloseWindow virtual ShouldCloseWindow
#define ResetTryToCloseWindow virtual ResetTryToCloseWindow
#define BRAVE_BROWSER_H              \
 private:                            \
  friend class BookmarkPrefsService; \
  friend class BraveBrowser;

#include "src/chrome/browser/ui/browser.h"

#undef BRAVE_BROWSER_H
#undef ScheduleUIUpdate
#undef ShouldDisplayFavicon
#undef TryToCloseWindow
#undef ShouldCloseWindow
#undef ResetTryToCloseWindow

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_BROWSER_H_
