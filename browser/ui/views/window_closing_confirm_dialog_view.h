/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_WINDOW_CLOSING_CONFIRM_DIALOG_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_WINDOW_CLOSING_CONFIRM_DIALOG_VIEW_H_

#include <memory>

#include "base/callback_forward.h"
#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/window/dialog_delegate.h"

class Browser;
class PrefService;

namespace views {
class Checkbox;
}  // namespace views

class WindowClosingConfirmDialogView : public views::DialogDelegateView {
 public:
  METADATA_HEADER(WindowClosingConfirmDialogView);
  static void Show(Browser* browser,
                   base::OnceCallback<void(bool)> callback,
                   base::RepeatingClosure quit_runloop);

  WindowClosingConfirmDialogView(const WindowClosingConfirmDialogView&) =
      delete;
  WindowClosingConfirmDialogView& operator=(
      const WindowClosingConfirmDialogView&) = delete;

 private:
  explicit WindowClosingConfirmDialogView(
      Browser* browser,
      base::OnceCallback<void(bool)> callback,
      base::RepeatingClosure quit_runloop);
  ~WindowClosingConfirmDialogView() override;

  void OnAccept();
  void Done();

  // views::DialogDelegate overrides:
  ui::ModalType GetModalType() const override;
  bool ShouldShowCloseButton() const override;
  bool ShouldShowWindowTitle() const override;

  base::OnceCallback<void(bool)> response_callback_;
  base::RepeatingClosure quit_runloop_;
  bool allowed_to_close_window_ = false;
  raw_ptr<Browser> browser_ = nullptr;
  raw_ptr<PrefService> prefs_ = nullptr;
  raw_ptr<views::Checkbox> dont_ask_again_checkbox_ = nullptr;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_WINDOW_CLOSING_CONFIRM_DIALOG_VIEW_H_
