/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/fil_tx_manager.h"
#include <stdint.h>
#include <string>

#include <memory>

#include "base/notreached.h"
#include "brave/components/brave_wallet/browser/fil_block_tracker.h"
#include "brave/components/brave_wallet/browser/fil_tx_state_manager.h"
#include "brave/components/brave_wallet/browser/fil_nonce_tracker.h"
#include "brave/components/brave_wallet/browser/fil_pending_tx_tracker.h"
#include "brave/components/brave_wallet/browser/fil_transaction.h"
#include "brave/components/brave_wallet/browser/fil_tx_meta.h"
#include "brave/components/brave_wallet/browser/fil_tx_state_manager.h"
#include "brave/components/brave_wallet/browser/json_rpc_service.h"
#include "brave/components/brave_wallet/browser/keyring_service.h"
#include "brave/components/brave_wallet/browser/tx_service.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "brave/components/brave_wallet/common/fil_address.h"
#include "brave/components/brave_wallet/common/hex_utils.h"
#include "components/grit/brave_components_strings.h"
#include "ui/base/l10n/l10n_util.h"

namespace brave_wallet {

// static
bool FilTxManager::ValidateTxData(const mojom::FilTxDataPtr& tx_data,
                                  std::string* error) {
  CHECK(error);
  // 'to' cannot be empty
  if (tx_data->to.empty()) {
    *error = l10n_util::GetStringUTF8(IDS_WALLET_FIL_SEND_TRANSACTION_TO);
    return false;
  }

  if (!tx_data->value.empty() && !IsValidHexString(tx_data->value)) {
    *error =
        l10n_util::GetStringUTF8(IDS_WALLET_SEND_TRANSACTION_VALUE_INVALID);
    return false;
  }
  if (!tx_data->to.empty() && !FilAddress::IsValidAddress(tx_data->to)) {
    *error = l10n_util::GetStringUTF8(IDS_WALLET_SEND_TRANSACTION_TO_INVALID);
    return false;
  }
  return true;
}

FilTxManager::FilTxManager(TxService* tx_service,
                           JsonRpcService* json_rpc_service,
                           KeyringService* keyring_service,
                           PrefService* prefs)
    : TxManager(std::make_unique<FilTxStateManager>(prefs, json_rpc_service),
                std::make_unique<FilBlockTracker>(json_rpc_service),
                tx_service,
                json_rpc_service,
                keyring_service,
                prefs),
      tx_state_manager_(
          std::make_unique<FilTxStateManager>(prefs, json_rpc_service)),
      nonce_tracker_(std::make_unique<FilNonceTracker>(tx_state_manager_.get(),
                                                       json_rpc_service)),
      pending_tx_tracker_(
          std::make_unique<FilPendingTxTracker>(tx_state_manager_.get(),
                                                json_rpc_service,
                                                nonce_tracker_.get())) {
  tx_state_manager_->AddObserver(this);
}

FilTxManager::~FilTxManager() {
  tx_state_manager_->RemoveObserver(this);
}

void FilTxManager::AddUnapprovedTransaction(
    mojom::FilTxDataPtr tx_data,
    const std::string& from,
    AddUnapprovedTransactionCallback callback) {
  if (from.empty()) {
    std::move(callback).Run(
        false, "",
        l10n_util::GetStringUTF8(IDS_WALLET_SEND_TRANSACTION_FROM_EMPTY));
    return;
  }

  std::string error;
  if (!FilTxManager::ValidateTxData(tx_data, &error)) {
    std::move(callback).Run(false, "", error);
    return;
  }
  auto tx = FilTransaction::FromTxData(tx_data);
  if (!tx) {
    std::move(callback).Run(
        false, "",
        l10n_util::GetStringUTF8(
            IDS_WALLET_ETH_SEND_TRANSACTION_CONVERT_TX_DATA));
    return;
  }
  auto tx_ptr = std::make_unique<FilTransaction>(*tx);
  const std::string gas_fee_cap = tx->gas_fee_cap();
  const std::string gas_premium = tx->gas_premium();
  auto gas_limit = tx->gas_limit();
  const std::string cid = tx->cid();

  if (!gas_limit || gas_fee_cap.empty() || gas_premium.empty() || cid.empty()) {
    GetEstimatedGas(from, std::move(tx_ptr), std::move(callback));
  } else {
    ContinueAddUnapprovedTransaction(
        from, std::move(tx_ptr), std::move(callback), gas_premium, gas_fee_cap,
        gas_limit, cid, mojom::ProviderError::kSuccess, "");
  }
}

void FilTxManager::GetEstimatedGas(const std::string& from,
                                   std::unique_ptr<FilTransaction> tx,
                                   AddUnapprovedTransactionCallback callback) {
  const std::string gas_premium = tx->gas_premium();
  const std::string gas_fee_cap = tx->gas_fee_cap();
  auto gas_limit = tx->gas_limit();
  uint64_t nonce = tx->nonce().value();
  const std::string value = tx->value();
  // TODO(spylogsster): add code to get max fee
  const std::string max_fee = "30000000000000";
  auto to = tx->to().ToChecksumAddress();
  json_rpc_service_->GetFilEstimateGas(
      from, to, gas_premium, gas_fee_cap, gas_limit, nonce, max_fee, value,
      base::BindOnce(&FilTxManager::ContinueAddUnapprovedTransaction,
                     weak_factory_.GetWeakPtr(), from, std::move(tx),
                     std::move(callback)));
}

void FilTxManager::ContinueAddUnapprovedTransaction(
    const std::string& from,
    std::unique_ptr<FilTransaction> tx,
    AddUnapprovedTransactionCallback callback,
    const std::string& gas_premium,
    const std::string& gas_fee_cap,
    uint64_t gas_limit,
    const std::string& cid,
    mojom::ProviderError error,
    const std::string& error_message) {
  DLOG(INFO) << gas_premium;
  DLOG(INFO) << gas_fee_cap;
  DLOG(INFO) << gas_limit;
  tx->set_gas_premium(gas_premium);
  tx->set_fee_cap(gas_fee_cap);
  tx->set_gas_limit(gas_limit);
  tx->set_cid(cid);
  DLOG(INFO) << "tx:" << tx->ToValue();
  FilTxMeta meta(std::move(tx));
  meta.set_id(TxMeta::GenerateMetaID());
  meta.set_from(FilAddress::From(from).ToChecksumAddress());
  meta.set_created_time(base::Time::Now());
  meta.set_status(mojom::TransactionStatus::Unapproved);
  DLOG(INFO) << "meta:" << meta.ToValue();
  tx_state_manager_->AddOrUpdateTx(meta);
  std::move(callback).Run(true, meta.id(), "");
}

void FilTxManager::AddUnapprovedTransaction(
    mojom::TxDataUnionPtr tx_data_union,
    const std::string& from,
    AddUnapprovedTransactionCallback callback) {
  AddUnapprovedTransaction(std::move(tx_data_union->get_fil_tx_data()), from,
                           std::move(callback));
}

void FilTxManager::ApproveTransaction(const std::string& tx_meta_id,
                                      ApproveTransactionCallback callback) {
  std::unique_ptr<FilTxMeta> meta = tx_state_manager_->GetFilTx(tx_meta_id);
  if (!meta) {
    LOG(ERROR) << "No transaction found";
    std::move(callback).Run(
        false, mojom::ProviderError::kResourceNotFound,
        l10n_util::GetStringUTF8(IDS_BRAVE_WALLET_TRANSACTION_NOT_FOUND));
    return;
  }
  DLOG(INFO) << "tx_meta_id:" << tx_meta_id
             << " value:" << meta->tx()->ToValue();
  if (!meta->tx()->nonce()) {
    auto from = FilAddress::From(meta->from());
    nonce_tracker_->GetNextNonce(
        from, base::BindOnce(&FilTxManager::OnGetNextNonce,
                             weak_factory_.GetWeakPtr(), std::move(meta),
                             std::move(callback)));
  } else {
    uint64_t nonce = meta->tx()->nonce().value();
    OnGetNextNonce(std::move(meta), std::move(callback), true, nonce);
  }
}

void FilTxManager::OnGetNextNonce(std::unique_ptr<FilTxMeta> meta,
                                  ApproveTransactionCallback callback,
                                  bool success,
                                  uint64_t nonce) {
  if (!success) {
    meta->set_status(mojom::TransactionStatus::Error);
    tx_state_manager_->AddOrUpdateTx(*meta);
    LOG(ERROR) << "GetNextNonce failed";
    std::move(callback).Run(
        false, mojom::ProviderError::kInternalError,
        l10n_util::GetStringUTF8(IDS_WALLET_GET_NONCE_ERROR));
    return;
  }
  meta->tx()->set_nonce(nonce);
  DCHECK(!keyring_service_->IsLocked());
  meta->set_status(mojom::TransactionStatus::Approved);
  tx_state_manager_->AddOrUpdateTx(*meta);
  /*  keyring_service_->SignTransactionByFilecoinKeyring(meta->from(),
    meta->tx()); if (!meta->tx()->IsSigned()) { LOG(ERROR) << "Transaction must
    be signed first"; std::move(callback).Run( false,
    mojom::ProviderError::kInternalError,
          l10n_util::GetStringUTF8(IDS_WALLET_SIGN_TRANSACTION_ERROR));
      return;
    }
    PublishTransaction(meta->id(), meta->tx()->GetSignedTransaction(),
                       std::move(callback));
                       */
}

void FilTxManager::PublishTransaction(const std::string& tx_meta_id,
                                      const std::string& signed_transaction,
                                      ApproveTransactionCallback callback) {
  json_rpc_service_->SendRawTransaction(
      signed_transaction, base::BindOnce(&FilTxManager::OnPublishTransaction,
                                         weak_factory_.GetWeakPtr(), tx_meta_id,
                                         std::move(callback)));
}

void FilTxManager::RejectTransaction(const std::string& tx_meta_id,
                                     RejectTransactionCallback) {
  NOTIMPLEMENTED();
}

void FilTxManager::GetAllTransactionInfo(
    const std::string& from,
    GetAllTransactionInfoCallback callback) {
  auto from_address = FilAddress::From(from);
  if (from_address.IsEmpty()) {
    std::move(callback).Run(std::vector<mojom::TransactionInfoPtr>());
    return;
  }
  TxManager::GetAllTransactionInfo(from_address.ToChecksumAddress(),
                                   std::move(callback));
}

void FilTxManager::SpeedupOrCancelTransaction(
    const std::string& tx_meta_id,
    bool cancel,
    SpeedupOrCancelTransactionCallback callback) {
  NOTIMPLEMENTED();
}

void FilTxManager::RetryTransaction(const std::string& tx_meta_id,
                                    RetryTransactionCallback callback) {
  NOTIMPLEMENTED();
}

void FilTxManager::GetTransactionMessageToSign(
    const std::string& tx_meta_id,
    GetTransactionMessageToSignCallback callback) {
  std::unique_ptr<FilTxMeta> meta = tx_state_manager_->GetFilTx(tx_meta_id);
  if (!meta) {
    VLOG(1) << __FUNCTION__ << "No transaction found with id:" << tx_meta_id;
    std::move(callback).Run(absl::nullopt);
    return;
  }
  auto message = meta->tx()->GetMessageToSign();
  auto encoded = brave_wallet::ToHex(message);
  std::move(callback).Run(encoded);
}

void FilTxManager::OnNewUnapprovedTx(mojom::TransactionInfoPtr tx_info) {
  tx_service_->OnNewUnapprovedTx(tx_info->Clone());
}

void FilTxManager::OnTransactionStatusChanged(
    mojom::TransactionInfoPtr tx_info) {
  tx_service_->OnTransactionStatusChanged(tx_info->Clone());
}

void FilTxManager::Reset() {
  TxManager::Reset();
  pending_tx_tracker_->Reset();
  known_no_pending_tx_ = false;
}

std::unique_ptr<FilTxMeta> FilTxManager::GetTxForTesting(
    const std::string& tx_meta_id) {
  return tx_state_manager_->GetFilTx(tx_meta_id);
}

void FilTxManager::OnPublishTransaction(std::string tx_meta_id,
                                        ApproveTransactionCallback callback,
                                        const std::string& tx_hash,
                                        mojom::ProviderError error,
                                        const std::string& error_message) {
  std::unique_ptr<TxMeta> meta = tx_state_manager_->GetTx(tx_meta_id);
  if (!meta) {
    DCHECK(false) << "Transaction should be found";
    std::move(callback).Run(
        false, mojom::ProviderError::kResourceNotFound,
        l10n_util::GetStringUTF8(IDS_BRAVE_WALLET_TRANSACTION_NOT_FOUND));
    return;
  }

  if (error == mojom::ProviderError::kSuccess) {
    meta->set_status(mojom::TransactionStatus::Submitted);
    meta->set_submitted_time(base::Time::Now());
    meta->set_tx_hash(tx_hash);
  } else {
    meta->set_status(mojom::TransactionStatus::Error);
  }

  tx_state_manager_->AddOrUpdateTx(*meta);

  if (error == mojom::ProviderError::kSuccess) {
    UpdatePendingTransactions();
  }

  std::move(callback).Run(error_message.empty(), error, error_message);
}

void FilTxManager::UpdatePendingTransactions() {
  size_t num_pending;
  if (pending_tx_tracker_->UpdatePendingTransactions(&num_pending)) {
    known_no_pending_tx_ = num_pending == 0;
  }
}
}  // namespace brave_wallet
