/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_ABI_DECODER_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_ABI_DECODER_H_

#include <string>
#include <vector>
#include "base/values.h"

namespace brave_wallet {

bool UniswapEncodedPathDecode(const std::string& encoded_path,
                              // NOLINTNEXTLINE(runtime/references)
                              std::vector<std::string>& path);

bool ABIDecode(const std::vector<std::string>& types,
               const std::string& data,
               std::vector<std::string>* tx_params,
               std::vector<std::string>* tx_args);

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_ETH_ABI_DECODER_H_
