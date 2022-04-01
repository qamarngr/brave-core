/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_data_parser.h"

#include <map>

#include "brave/components/brave_wallet/browser/brave_wallet_constants.h"
#include "brave/components/brave_wallet/browser/eth_abi_decoder.h"
#include "brave/components/brave_wallet/common/brave_wallet_types.h"
#include "brave/components/brave_wallet/common/hex_utils.h"

namespace brave_wallet {

bool GetTransactionInfoFromData(const std::string& data,
                                mojom::TransactionType* tx_type,
                                std::vector<std::string>* tx_params,
                                std::vector<std::string>* tx_args) {
  CHECK(tx_type);

  if (tx_params)
    tx_params->clear();
  if (tx_args)
    tx_args->clear();

  *tx_type = mojom::TransactionType::Other;

  if (data.empty() || data == "0x0") {
    *tx_type = mojom::TransactionType::ETHSend;
    return true;
  }
  if (!IsValidHexString(data))
    return false;
  if (data.length() < 10) {
    *tx_type = mojom::TransactionType::Other;
    return true;
  }

  std::string selector = base::ToLowerASCII(data.substr(0, 10));
  std::string left_over_data = data.substr(10);
  if (selector == "0xa9059cbb") {
    *tx_type = mojom::TransactionType::ERC20Transfer;
    if (!brave_wallet::ABIDecode({"address", "uint256"}, left_over_data,
                                 tx_params, tx_args))
      return false;
  } else if (selector == "0x095ea7b3") {
    *tx_type = mojom::TransactionType::ERC20Approve;
    if (!brave_wallet::ABIDecode({"address", "uint256"}, left_over_data,
                                 tx_params, tx_args))
      return false;
  } else if (selector == "0x23b872dd") {
    *tx_type = mojom::TransactionType::ERC721TransferFrom;
    if (!brave_wallet::ABIDecode({"address", "address", "uint256"},
                                 left_over_data, tx_params, tx_args))
      return false;
  } else if (selector == "0x42842e0e") {
    *tx_type = mojom::TransactionType::ERC721SafeTransferFrom;
    if (!brave_wallet::ABIDecode({"address", "address", "uint256"},
                                 left_over_data, tx_params, tx_args))
      return false;
  } else if (selector == "0x3598d8ab") {
    // Function:
    // sellEthForTokenToUniswapV3(bytes encodedPath,
    //                            uint256 minBuyAmount,
    //                            address recipient)
    *tx_type = mojom::TransactionType::ETHSwap;
    if (!brave_wallet::ABIDecode({"bytes", "uint256", "address"},
                                 left_over_data, tx_params, tx_args))
      return false;

    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };

    // encodedPath is formatted in the following manner:
    //   <address><POOL_FEE><address>...
    // where, POOL FEE is associated with the token address that follows.
    //
    // We remove the POOL_FEE from the bytearray since it is not useful for
    // the clients.
    std::string original_fill_path = (*tx_args)[0].substr(2);
    std::string fill_path = "0x";
    while (true) {
      fill_path += original_fill_path.substr(0, 40);
      if (original_fill_path.substr(40).length() == 0)
        break;
      original_fill_path = original_fill_path.substr(40 + 3 * 2);
    }

    *tx_args = {fill_path,
                "",  // maker asset is ETH, amount is txn value
                (*tx_args)[1]};
  } else if (selector == "0x415565b0") {
    // Function:
    // transformERC20(address inputToken,
    //                address outputToken,
    //                uint256 inputTokenAmount,
    //                uint256 minOutputTokenAmount,
    //                (uint32,bytes)[] transformations)
    *tx_type = mojom::TransactionType::ETHSwap;
    if (!brave_wallet::ABIDecode(
            {"address", "address", "uint256", "uint256", "(uint32,bytes)[]"},
            left_over_data, tx_params, tx_args))
      return false;

    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };
    std::string fill_path = (*tx_args)[0] + (*tx_args)[1].substr(2);
    *tx_args = {fill_path, (*tx_args)[2], (*tx_args)[3]};
  } else {
    *tx_type = mojom::TransactionType::Other;
  }

  return true;
}

}  // namespace brave_wallet
