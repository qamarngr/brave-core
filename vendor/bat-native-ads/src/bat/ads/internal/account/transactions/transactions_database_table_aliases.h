/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_DATABASE_TABLE_ALIASES_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_DATABASE_TABLE_ALIASES_H_

#include <functional>

#include "bat/ads/transaction_info_aliases.h"

namespace ads {

using GetTransactionsCallback =
    std::function<void(const bool, const TransactionList&)>;

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_DATABASE_TABLE_ALIASES_H_
