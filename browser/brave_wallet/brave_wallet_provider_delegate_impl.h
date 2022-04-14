/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_WALLET_BRAVE_WALLET_PROVIDER_DELEGATE_IMPL_H_
#define BRAVE_BROWSER_BRAVE_WALLET_BRAVE_WALLET_PROVIDER_DELEGATE_IMPL_H_

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/brave_wallet/browser/brave_wallet_provider_delegate.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/web_contents_observer.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace content {
class RenderFrameHost;
class WebContents;
}  // namespace content

namespace brave_wallet {

class KeyringService;

class BraveWalletProviderDelegateImpl : public BraveWalletProviderDelegate,
                                        public content::WebContentsObserver {
 public:
  explicit BraveWalletProviderDelegateImpl(
      content::WebContents* web_contents,
      content::RenderFrameHost* const render_frame_host);
  BraveWalletProviderDelegateImpl(const BraveWalletProviderDelegateImpl&) =
      delete;
  BraveWalletProviderDelegateImpl& operator=(
      const BraveWalletProviderDelegateImpl&) = delete;
  ~BraveWalletProviderDelegateImpl() override;

  void ShowPanel() override;
  url::Origin GetOrigin() const override;
  void RequestEthereumPermissions(
      RequestEthereumPermissionsCallback callback) override;
  void GetAllowedAccounts(mojom::CoinType type,
                          bool include_accounts_when_locked,
                          GetAllowedAccountsCallback callback) override;
  void RequestSolanaPermission(
      RequestSolanaPermissionCallback callback) override;
  void IsSelectedAccountAllowed(
      mojom::CoinType type,
      IsSelectedAccountAllowedCallback callback) override;

 private:
  void ContinueRequestEthereumPermissions(
      RequestEthereumPermissionsCallback callback,
      const std::vector<std::string>& allowed_accounts,
      mojom::ProviderError error,
      const std::string& error_message);
  void ContinueRequestEthereumPermissionsKeyringInfo(
      RequestEthereumPermissionsCallback callback,
      brave_wallet::mojom::KeyringInfoPtr keyring_info);
  void ContinueRequestEthereumPermissionsSelectedAccount(
      RequestEthereumPermissionsCallback callback,
      const std::vector<std::string>& request_accounts,
      const absl::optional<std::string>& selected_account);
  void ContinueGetAllowedAccounts(
      bool include_accounts_when_locked,
      GetAllowedAccountsCallback callback,
      const absl::optional<std::string>& selected_account);
  void ContinueRequestSolanaPermission(
      RequestSolanaPermissionCallback callback,
      const absl::optional<std::string>& selected_account,
      bool is_selected_account_allowed);

  void ContinueIsSelectedAccountAllowed(
      IsSelectedAccountAllowedCallback callback,
      const absl::optional<std::string>& selected_account);

  // content::WebContentsObserver overrides
  void WebContentsDestroyed() override;

  mojo::Remote<mojom::KeyringService> keyring_service_;
  raw_ptr<content::WebContents> web_contents_ = nullptr;
  const content::GlobalRenderFrameHostId host_id_;
  base::WeakPtrFactory<BraveWalletProviderDelegateImpl> weak_ptr_factory_;
};

}  // namespace brave_wallet

#endif  // BRAVE_BROWSER_BRAVE_WALLET_BRAVE_WALLET_PROVIDER_DELEGATE_IMPL_H_
