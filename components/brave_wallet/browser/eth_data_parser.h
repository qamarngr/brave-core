/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_DATA_PARSER_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_DATA_PARSER_H_

#include <string>
#include <vector>

#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"

namespace brave_wallet {

constexpr char kERC20TransferSelector[] = "0xa9059cbb";
constexpr char kERC20ApproveSelector[] = "0x095ea7b3";
constexpr char kERC721TransferFromSelector[] = "0x23b872dd";
constexpr char kERC721SafeTransferFromSelector[] = "0x42842e0e";
constexpr char kSellEthForTokenToUniswapV3Selector[] = "0x3598d8ab";
constexpr char kSellTokenForEthToUniswapV3Selector[] = "0x803ba26d";
constexpr char kSellTokenForTokenToUniswapV3Selector[] = "0x6af479b2";
constexpr char kSellToUniswapSelector[] = "0xd9627aa4";
constexpr char kTransformERC20Selector[] = "0x415565b0";

bool GetTransactionInfoFromData(const std::string& data,
                                mojom::TransactionType* tx_type,
                                std::vector<std::string>* tx_params,
                                std::vector<std::string>* tx_args);

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_DATA_PARSER_H_
