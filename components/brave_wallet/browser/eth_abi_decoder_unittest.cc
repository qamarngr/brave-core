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

}  // namespace brave_wallet
