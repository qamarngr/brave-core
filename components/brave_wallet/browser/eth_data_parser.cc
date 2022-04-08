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
  std::string calldata = data.substr(10);
  if (selector == "0xa9059cbb") {
    *tx_type = mojom::TransactionType::ERC20Transfer;
    if (!brave_wallet::ABIDecode({"address", "uint256"}, calldata, tx_params,
                                 tx_args)) {
      return false;
    }
  } else if (selector == "0x095ea7b3") {
    *tx_type = mojom::TransactionType::ERC20Approve;
    if (!brave_wallet::ABIDecode({"address", "uint256"}, calldata, tx_params,
                                 tx_args)) {
      return false;
    }
  } else if (selector == "0x23b872dd") {
    *tx_type = mojom::TransactionType::ERC721TransferFrom;
    if (!brave_wallet::ABIDecode({"address", "address", "uint256"}, calldata,
                                 tx_params, tx_args)) {
      return false;
    }
  } else if (selector == "0x42842e0e") {
    *tx_type = mojom::TransactionType::ERC721SafeTransferFrom;
    if (!brave_wallet::ABIDecode({"address", "address", "uint256"}, calldata,
                                 tx_params, tx_args)) {
      return false;
    }
  } else if (selector == "0x3598d8ab") {
    *tx_type = mojom::TransactionType::ETHSwap;

    // Function:
    // sellEthForTokenToUniswapV3(bytes encodedPath,
    //                            uint256 minBuyAmount,
    //                            address recipient)
    //
    // Ref:
    // https://github.com/0xProject/protocol/blob/b46eeadc64485288add5940a210e1a7d0bcb5481/contracts/zero-ex/contracts/src/features/interfaces/IUniswapV3Feature.sol#L29-L41
    if (!brave_wallet::ABIDecode({"bytes", "uint256", "address"}, calldata,
                                 tx_params, tx_args)) {
      return false;
    }

    std::vector<std::string> decoded_path;
    if (!brave_wallet::UniswapEncodedPathDecode(tx_args->at(0), decoded_path))
      return false;
    std::string fill_path = "0x";
    for (const auto& path : decoded_path) {
      fill_path += path.substr(2);
    }

    // Populate ETHSwap tx_params and tx_args.
    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };
    *tx_args = {fill_path,
                "",  // maker asset is ETH, amount is txn value
                tx_args->at(1)};
  } else if (selector == "0x803ba26d" || selector == "0x6af479b2") {
    *tx_type = mojom::TransactionType::ETHSwap;

    // Function: 0x803ba26d
    // sellTokenForEthToUniswapV3(bytes encodedPath,
    //                            uint256 sellAmount,
    //                            uint256 minBuyAmount,
    //                            address recipient)
    //
    // Ref:
    // https://github.com/0xProject/protocol/blob/b46eeadc64485288add5940a210e1a7d0bcb5481/contracts/zero-ex/contracts/src/features/interfaces/IUniswapV3Feature.sol#L43-L56
    //
    //
    // Function: 0x6af479b2
    // sellTokenForTokenToUniswapV3(bytes encodedPath,
    //                              uint256 sellAmount,
    //                              uint256 minBuyAmount,
    //                              address recipient)
    //
    // Ref:
    // https://github.com/0xProject/protocol/blob/b46eeadc64485288add5940a210e1a7d0bcb5481/contracts/zero-ex/contracts/src/features/interfaces/IUniswapV3Feature.sol#L58-L71
    if (!brave_wallet::ABIDecode({"bytes", "uint256", "uint256", "address"},
                                 calldata, tx_params, tx_args)) {
      return false;
    }

    std::vector<std::string> decoded_path;
    if (!brave_wallet::UniswapEncodedPathDecode(tx_args->at(0), decoded_path))
      return false;
    std::string fill_path = "0x";
    for (const auto& path : decoded_path) {
      fill_path += path.substr(2);
    }

    // Populate ETHSwap tx_params and tx_args.
    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };
    *tx_args = {fill_path, tx_args->at(1), tx_args->at(2)};
  } else if (selector == "0xd9627aa4") {
    *tx_type = mojom::TransactionType::ETHSwap;

    // Function:
    // sellToUniswap(address[] tokens,
    //               uint256 sellAmount,
    //               uint256 minBuyAmount,
    //               bool isSushi)
    //
    // Ref:
    // https://github.com/0xProject/protocol/blob/8d6f6e76e053f7b065d3315ddb31d2c35caddca7/contracts/zero-ex/contracts/src/features/UniswapFeature.sol#L93-L104
    if (!brave_wallet::ABIDecode({"address[]", "uint256", "uint256", "bool"},
                                 calldata, tx_params, tx_args)) {
      return false;
    }

    // Populate ETHSwap tx_params and tx_args.
    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };
    *tx_args = {tx_args->at(0), tx_args->at(1), tx_args->at(2)};

  } else if (selector == "0x415565b0") {
    *tx_type = mojom::TransactionType::ETHSwap;

    // Function:
    // transformERC20(address inputToken,
    //                address outputToken,
    //                uint256 inputTokenAmount,
    //                uint256 minOutputTokenAmount,
    //                (uint32,bytes)[] transformations)
    //
    // Ref:
    // https://github.com/0xProject/protocol/blob/b46eeadc64485288add5940a210e1a7d0bcb5481/contracts/zero-ex/contracts/src/features/interfaces/ITransformERC20Feature.sol#L113-L134
    if (!brave_wallet::ABIDecode(
            {"address", "address", "uint256", "uint256", "(uint32,bytes)[]"},
            calldata, tx_params, tx_args)) {
      return false;
    }

    // Populate ETHSwap tx_params and tx_args.
    *tx_params = {
        "bytes",    // fill path
        "uint256",  // maker amount
        "uint256"   // taker amount
    };
    std::string fill_path = tx_args->at(0) + tx_args->at(1).substr(2);
    *tx_args = {fill_path, tx_args->at(2), tx_args->at(3)};
  } else {
    *tx_type = mojom::TransactionType::Other;
  }

  return true;
}

}  // namespace brave_wallet
