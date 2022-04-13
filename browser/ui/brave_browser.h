/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_BROWSER_H_
#define BRAVE_BROWSER_UI_BRAVE_BROWSER_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "brave/components/sidebar/buildflags/buildflags.h"
#include "chrome/browser/ui/browser.h"

#if BUILDFLAG(ENABLE_SIDEBAR)
namespace sidebar {
class SidebarController;
}  // namespace sidebar

class BraveBrowserWindow;
#endif

namespace content {
class WebContents;
}  // namespace content

class BraveBrowser : public Browser {
 public:
  explicit BraveBrowser(const CreateParams& params);
  ~BraveBrowser() override;

  BraveBrowser(const BraveBrowser&) = delete;
  BraveBrowser& operator=(const BraveBrowser&) = delete;

  // Browser overrides:
  void ScheduleUIUpdate(content::WebContents* source,
                        unsigned changed_flags) override;
  bool ShouldDisplayFavicon(content::WebContents* web_contents) const override;
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  bool ShouldCloseWindow() override;
  bool TryToCloseWindow(
      bool skip_beforeunload,
      const base::RepeatingCallback<void(bool)>& on_close_confirmed) override;
  void ResetTryToCloseWindow() override;

#if BUILDFLAG(ENABLE_SIDEBAR)
  sidebar::SidebarController* sidebar_controller() {
    return sidebar_controller_.get();
  }
#endif

  BraveBrowserWindow* brave_window();

 private:
  bool ShouldShowWindowClosingConfirmDialog() const;
  void OnShowWindowClosingConfirmDialog(bool allowed_to_close_window);

#if BUILDFLAG(ENABLE_SIDEBAR)
  std::unique_ptr<sidebar::SidebarController> sidebar_controller_;
#endif

  bool show_window_closing_confirm_dialog_ = true;
  bool allowed_to_close_window_ = true;

  base::WeakPtrFactory<BraveBrowser> weak_factory_{this};
};

#endif  // BRAVE_BROWSER_UI_BRAVE_BROWSER_H_
