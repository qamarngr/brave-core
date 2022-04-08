/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_abi_decoder.h"

#include <string>
#include <vector>

#include "brave/components/brave_wallet/browser/brave_wallet_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace brave_wallet {

TEST(EthDataParser, ABIDecodeAddressValid) {
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_TRUE(ABIDecode(
      {"address"},
      "000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64460f",
      &tx_params, &tx_args));

  ASSERT_EQ(tx_params.size(), 1UL);
  EXPECT_EQ(tx_params[0], "address");
  EXPECT_EQ(tx_args[0], "0xBFb30a082f650C2A15D0632f0e87bE4F8e64460f");
}

TEST(EthDataParser, ABIDecodeAddressInValidHex) {
  std::vector<std::string> tx_params;
  std::vector<std::string> tx_args;
  ASSERT_FALSE(
      ABIDecode({"address"},
                "000000000000000000000000BFb30a082f650C2A15D0632f0e87bE4F8e64",
                &tx_params, &tx_args));
}

TEST(EthDataParser, UniswapEncodedPathDecodeValid) {
  // Single-hop swap: WETH → STG
  std::vector<std::string> path;
  ASSERT_TRUE(UniswapEncodedPathDecode(
      "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"  // WETH
      "002710"                                      // POOL FEE (10000)
      "af5191b0de278c7286d6c7cc6ab6bb8a73ba2cd6",   // STG
      path));
  ASSERT_EQ(path.size(), 2UL);
  ASSERT_EQ(path[0], "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2");
  ASSERT_EQ(path[1], "0xaf5191b0de278c7286d6c7cc6ab6bb8a73ba2cd6");

  // Multi-hop swap: RSS3 → USDC → WETH
  path = {};
  ASSERT_TRUE(UniswapEncodedPathDecode(
      "0xc98d64da73a6616c42117b582e832812e7b8d57f"  // RSS3
      "000bb8"                                      // POOL FEE (3000)
      "a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48"    // USDC
      "0001f4"                                      // POOL FEE (500)
      "c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2",   // WETH
      path));
  ASSERT_EQ(path.size(), 3UL);
  ASSERT_EQ(path[0], "0xc98d64da73a6616c42117b582e832812e7b8d57f");
  ASSERT_EQ(path[1], "0xa0b86991c6218b36c1d19d4a2e9eb0ce3606eb48");
  ASSERT_EQ(path[2], "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2");
}

TEST(EthDataParser, UniswapEncodedPathDecodeInvalid) {
  // Missing source hop.
  std::vector<std::string> path;
  ASSERT_FALSE(UniswapEncodedPathDecode(
      "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"  // WETH
      "002710",                                     // POOL FEE
      path));

  // Missing destination hop.
  ASSERT_FALSE(UniswapEncodedPathDecode(
      "0x002710"                                   // POOL FEE
      "af5191b0de278c7286d6c7cc6ab6bb8a73ba2cd6",  // STG
      path));

  // Missing POOL FEE
  ASSERT_FALSE(UniswapEncodedPathDecode(
      "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"  // WETH
      "af5191b0de278c7286d6c7cc6ab6bb8a73ba2cd6",   // STG
      path));
}

}  // namespace brave_wallet
