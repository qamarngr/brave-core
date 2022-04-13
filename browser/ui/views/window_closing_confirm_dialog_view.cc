/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/window_closing_confirm_dialog_view.h"

#include <utility>

#include "base/bind.h"
#include "base/callback.h"
#include "base/run_loop.h"
#include "brave/common/pref_names.h"
#include "brave/grit/brave_generated_resources.h"
#include "build/build_config.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/grit/chromium_strings.h"
#include "components/constrained_window/constrained_window_views.h"
#include "components/prefs/pref_service.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/font.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/window/dialog_delegate.h"

namespace brave {

void ShowWindowClosingConfirmDialog(
    Browser* browser,
    base::OnceCallback<void(bool)> response_callback) {
  base::RunLoop run_loop(base::RunLoop::Type::kNestableTasksAllowed);
  WindowClosingConfirmDialogView::Show(browser, std::move(response_callback),
                                       run_loop.QuitClosure());
  run_loop.Run();
}

}  // namespace brave

// static
void WindowClosingConfirmDialogView::Show(
    Browser* browser,
    base::OnceCallback<void(bool)> response_callback,
    base::RepeatingClosure quit_runloop) {
  constrained_window::CreateBrowserModalDialogViews(
      new WindowClosingConfirmDialogView(browser, std::move(response_callback),
                                         std::move(quit_runloop)),
      browser->window()->GetNativeWindow())
      ->Show();
}

WindowClosingConfirmDialogView::WindowClosingConfirmDialogView(
    Browser* browser,
    base::OnceCallback<void(bool)> response_callback,
    base::RepeatingClosure quit_runloop)
    : response_callback_(std::move(response_callback)),
      quit_runloop_(std::move(quit_runloop)),
      browser_(browser),
      prefs_(browser->profile()->GetPrefs()) {
  set_should_ignore_snapping(true);
  SetButtonLabel(ui::DIALOG_BUTTON_OK,
                 l10n_util::GetStringUTF16(
                     IDS_WINDOW_CLOSING_CONFIRM_DLG_OK_BUTTON_LABEL));
  SetButtonLabel(ui::DIALOG_BUTTON_CANCEL,
                 l10n_util::GetStringUTF16(
                     IDS_WINDOW_CLOSING_CONFIRM_DLG_CANCEL_BUTTON_LABEL));
  RegisterWindowClosingCallback(base::BindOnce(
      &WindowClosingConfirmDialogView::Done, base::Unretained(this)));
  SetAcceptCallback(base::BindOnce(&WindowClosingConfirmDialogView::OnAccept,
                                   base::Unretained(this)));

  constexpr int kChildSpacing = 16;
  constexpr int kPadding = 24;
  constexpr int kTopPadding = 20;
  constexpr int kBottomPadding = 25;

  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical,
      gfx::Insets(kTopPadding, kPadding, kBottomPadding, kPadding),
      kChildSpacing));

  constexpr int kHeaderFontSize = 16;
  int size_diff =
      kHeaderFontSize - views::Label::GetDefaultFontList().GetFontSize();
  views::Label::CustomFont header_font = {
      views::Label::GetDefaultFontList()
          .DeriveWithSizeDelta(size_diff)
          .DeriveWithWeight(gfx::Font::Weight::SEMIBOLD)};
  auto* header_label = AddChildView(std::make_unique<views::Label>(
      l10n_util::GetStringUTF16(IDS_WINDOW_CLOSING_CONFIRM_DLG_HEADER_LABEL),
      header_font));
  header_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);

  constexpr int kContentFontSize = 15;
  size_diff =
      kContentFontSize - views::Label::GetDefaultFontList().GetFontSize();
  views::Label::CustomFont contents_font = {
      views::Label::GetDefaultFontList()
          .DeriveWithSizeDelta(size_diff)
          .DeriveWithWeight(gfx::Font::Weight::NORMAL)};
  // TODO(simonhong): Use StyledLabel for tab count bold.
  auto* contents_label = AddChildView(std::make_unique<views::Label>(
      l10n_util::GetStringFUTF16Int(
          IDS_WINDOW_CLOSING_CONFIRM_DLG_CONTENTS_LABEL,
          browser_->tab_strip_model()->count()),
      contents_font));
  contents_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  contents_label->SetMultiLine(true);
  constexpr int kMaxWidth = 400;
  contents_label->SetMaximumWidth(kMaxWidth);

  dont_ask_again_checkbox_ =
      AddChildView(std::make_unique<views::Checkbox>(l10n_util::GetStringUTF16(
          IDS_WINDOW_CLOSING_CONFIRM_DLG_DONT_ASK_AGAIN_LABEL)));
}

WindowClosingConfirmDialogView::~WindowClosingConfirmDialogView() = default;

ui::ModalType WindowClosingConfirmDialogView::GetModalType() const {
  return ui::MODAL_TYPE_WINDOW;
}

bool WindowClosingConfirmDialogView::ShouldShowCloseButton() const {
  return false;
}

bool WindowClosingConfirmDialogView::ShouldShowWindowTitle() const {
  return false;
}

void WindowClosingConfirmDialogView::OnAccept() {
  allowed_to_close_window_ = true;
}

void WindowClosingConfirmDialogView::Done() {
  prefs_->SetBoolean(kEnableWindowClosingConfirm,
                     !dont_ask_again_checkbox_->GetChecked());
  std::move(response_callback_).Run(allowed_to_close_window_);
  quit_runloop_.Run();
}

BEGIN_METADATA(WindowClosingConfirmDialogView, views::DialogDelegateView)
END_METADATA
