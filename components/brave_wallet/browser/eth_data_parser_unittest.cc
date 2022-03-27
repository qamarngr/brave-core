/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_data_parser.h"

#include <string>
#include <vector>

#include "brave/components/brave_wallet/browser/brave_wallet_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace brave_wallet {

TEST(EthDataParser, GetTransactionInfoFromDataTransfer) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0xa9059cbb000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003fffffffffffffff",
      &tx_type, &tx_params, &tx_args));
  ASSERT_EQ(tx_type, mojom::TransactionType::ERC20Transfer);
  ASSERT_EQ(tx_params.size(), 2UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_params[1], "uint256");
  ASSERT_EQ(tx_args.size(), 2UL);
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
  EXPECT_EQ(tx_args[1], "0x3fffffffffffffff");

  // Missing a char for the last param
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0xa9059cbb000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003ffffffffffffff",
      &tx_type, &tx_params, &tx_args));
  // Missing the entire last param
  EXPECT_FALSE(
      GetTransactionInfoFromData("0xa9059cbb000000000000000000000000BFb30a082f6"
                                 "50C2A15D0632f0e87bE4F8e64460f",
                                 &tx_type, &tx_params, &tx_args));
  // No params
  EXPECT_FALSE(
      GetTransactionInfoFromData("0xa9059cbb", &tx_type, &tx_params, &tx_args));
  // Extra data
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0xa9059cbb000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003fffffffffffffff00",
      &tx_type, &tx_params, &tx_args));
}

TEST(EthDataParser, GetTransactionInfoFromDataApprove) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x095ea7b3000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003fffffffffffffff",
      &tx_type, &tx_params, &tx_args));
  ASSERT_EQ(tx_type, mojom::TransactionType::ERC20Approve);
  ASSERT_EQ(tx_params.size(), 2UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_params[1], "uint256");
  ASSERT_EQ(tx_args.size(), 2UL);
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
  EXPECT_EQ(tx_args[1], "0x3fffffffffffffff");

  // Function case doesn't matter
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x095EA7b3000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003fffffffffffffff",
      &tx_type, &tx_params, &tx_args));
  ASSERT_EQ(tx_type, mojom::TransactionType::ERC20Approve);
  ASSERT_EQ(tx_params.size(), 2UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_params[1], "uint256");
  ASSERT_EQ(tx_args.size(), 2UL);
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
  EXPECT_EQ(tx_args[1], "0x3fffffffffffffff");

  // Missing a char for the last param
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0x095ea7b3000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003ffffffffffffff",
      &tx_type, &tx_params, &tx_args));
  // Missing the entire last param
  EXPECT_FALSE(
      GetTransactionInfoFromData("0x095ea7b3000000000000000000000000BFb30a082f6"
                                 "50C2A15D0632f0e87bE4F8e64460f",
                                 &tx_type, &tx_params, &tx_args));
  // No params
  EXPECT_FALSE(
      GetTransactionInfoFromData("0x095ea7b3", &tx_type, &tx_params, &tx_args));
  // Extra data
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0x095ea7b3000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f0000000000000000000000000000000000000000000000003fffffffffffffff00",
      &tx_type, &tx_params, &tx_args));
}

TEST(EthDataParser, GetTransactionInfoFromDataETHSend) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_TRUE(
      GetTransactionInfoFromData("0x0", &tx_type, &tx_params, &tx_args));
  EXPECT_EQ(tx_type, mojom::TransactionType::ETHSend);
  ASSERT_EQ(tx_params.size(), 0UL);
  ASSERT_EQ(tx_args.size(), 0UL);
  ASSERT_TRUE(GetTransactionInfoFromData("", &tx_type, &tx_params, &tx_args));
  EXPECT_EQ(tx_type, mojom::TransactionType::ETHSend);
  ASSERT_EQ(tx_params.size(), 0UL);
  ASSERT_EQ(tx_args.size(), 0UL);
}

TEST(EthDataParser, GetTransactionInfoFromDataERC721TransferFrom) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x23b872dd000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460a000000"
      "000000000000000000000000000000000000000000000000000000000f",
      &tx_type, &tx_params, &tx_args));
  ASSERT_EQ(tx_type, mojom::TransactionType::ERC721TransferFrom);
  ASSERT_EQ(tx_params.size(), 3UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_params[1], "address");
  EXPECT_EQ(tx_params[2], "uint256");
  ASSERT_EQ(tx_args.size(), 3UL);
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
  EXPECT_EQ(tx_args[1], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460a");
  EXPECT_EQ(tx_args[2], "0xf");

  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x42842e0e000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460a000000"
      "000000000000000000000000000000000000000000000000000000000f",
      &tx_type, &tx_params, &tx_args));
  ASSERT_EQ(tx_type, mojom::TransactionType::ERC721SafeTransferFrom);
  ASSERT_EQ(tx_params.size(), 3UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_params[1], "address");
  EXPECT_EQ(tx_params[2], "uint256");
  ASSERT_EQ(tx_args.size(), 3UL);
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
  EXPECT_EQ(tx_args[1], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460a");
  EXPECT_EQ(tx_args[2], "0xf");

  // Missing a char for the last param
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0x23b872dd000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460a000000"
      "000000000000000000000000000000000000000000000000000000000",
      &tx_type, &tx_params, &tx_args));
  // Missing the entire last param
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0x23b872dd000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460a",
      &tx_type, &tx_params, &tx_args));
  // No params
  EXPECT_FALSE(
      GetTransactionInfoFromData("0x23b872dd", &tx_type, &tx_params, &tx_args));
  // Extra data
  EXPECT_FALSE(GetTransactionInfoFromData(
      "0x23b872dd000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e6446"
      "0f000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460a000000"
      "000000000000000000000000000000000000000000000000000000000f00",
      &tx_type, &tx_params, &tx_args));
}

TEST(EthDataParser, GetTransactionInfoFromDataOther) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;

  // No function hash
  EXPECT_TRUE(
      GetTransactionInfoFromData("0x1", &tx_type, &tx_params, &tx_args));
  EXPECT_EQ(tx_type, mojom::TransactionType::Other);
  EXPECT_TRUE(
      GetTransactionInfoFromData("0xaa0ffceb000000000000000000000000BFb30a082f6"
                                 "50C2A15D0632f0e87bE4F8e64460f",
                                 &tx_type, &tx_params, &tx_args));
  EXPECT_EQ(tx_type, mojom::TransactionType::Other);
  // Invalid input
  EXPECT_FALSE(
      GetTransactionInfoFromData("hello", &tx_type, &tx_params, &tx_args));
}

TEST(EthDataParser, GetTransactionInfoFromDataETHSwapUniswapV3) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;

  // Function:
  // sellEthForTokenToUniswapV3(bytes encodedPath,
  //                            uint256 minBuyAmount,
  //                            address recipient)
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x3598d8ab"  // function selector
      /*********************** HEAD (32x3 bytes) **********************/
      "0000000000000000000000000000000000000000000000000000000000000060"
      "0000000000000000000000000000000000000000000000030c1a39b13e25f498"
      "0000000000000000000000000000000000000000000000000000000000000000"

      /***************************** TAIL *****************************/
      // calldata reference position for encodedPath
      "000000000000000000000000000000000000000000000000000000000000002b"
      "c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2002710af5191b0de278c7286"
      "d6c7cc6ab6bb8a73ba2cd6000000000000000000000000000000000000000000"

      // extraneous tail segment to be ignored
      "869584cd0000000000000000000000003ce37278de6388532c3949ce4e886f36"
      "5b14fb560000000000000000000000000000000000000000000000f7834ab14c"
      "623f4f93",
      &tx_type, &tx_params, &tx_args));

  EXPECT_EQ(tx_type, mojom::TransactionType::ETHSwap);

  ASSERT_EQ(tx_params.size(), 3UL);
  EXPECT_EQ(tx_params[0], "bytes");
  EXPECT_EQ(tx_params[1], "uint256");
  EXPECT_EQ(tx_params[2], "uint256");

  ASSERT_EQ(tx_args.size(), 3UL);
  EXPECT_EQ(tx_args[0],
            "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"
            "af5191b0de278c7286d6c7cc6ab6bb8a73ba2cd6");
  EXPECT_EQ(tx_args[1], "");
  EXPECT_EQ(tx_args[2], "0x30c1a39b13e25f498");
}

TEST(EthDataParser, GetTransactionInfoFromDataETHSwapTransformERC20) {
  mojom::TransactionType tx_type;
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;

  // Function:
  // transformERC20(address inputToken,
  //                address outputToken,
  //                uint256 inputTokenAmount,
  //                uint256 minOutputTokenAmount,
  //                (uint32,bytes)[] transformations)
  ASSERT_TRUE(GetTransactionInfoFromData(
      "0x415565b0"  // function selector
      /*********************** HEAD (32x5 bytes) **********************/
      "000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec7"
      "000000000000000000000000eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
      "00000000000000000000000000000000000000000000000000000004a817c7ff"
      "00000000000000000000000000000000000000000000000055c75b6fa8d5f0d4"
      "00000000000000000000000000000000000000000000000000000000000000a0"

      /*********************** TAIL (truncated) ***********************/
      // calldata reference position for transformations
      "0000000000000000000000000000000000000000000000000000000000000004"
      "0000000000000000000000000000000000000000000000000000000000000080"
      "00000000000000000000000000000000000000000000000000000000000003e0"
      "0000000000000000000000000000000000000000000000000000000000000480",
      &tx_type, &tx_params, &tx_args));

  EXPECT_EQ(tx_type, mojom::TransactionType::ETHSwap);

  ASSERT_EQ(tx_params.size(), 3UL);
  EXPECT_EQ(tx_params[0], "bytes");
  EXPECT_EQ(tx_params[1], "uint256");
  EXPECT_EQ(tx_params[2], "uint256");

  ASSERT_EQ(tx_args.size(), 3UL);
  EXPECT_EQ(tx_args[0],
            "0xdac17f958d2ee523a2206206994597c13d831ec7"
            "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
  EXPECT_EQ(tx_args[1], "0x4a817c7ff");
  EXPECT_EQ(tx_args[2], "0x55c75b6fa8d5f0d4");
}

}  // namespace brave_wallet
