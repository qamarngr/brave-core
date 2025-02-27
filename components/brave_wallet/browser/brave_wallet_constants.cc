/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/brave_wallet_constants.h"

namespace brave_wallet {

const std::vector<mojom::BlockchainToken>& GetWyreBuyTokens() {
  static base::NoDestructor<std::vector<mojom::BlockchainToken>> tokens(
      {{"0x0D8775F648430679A709E98d2b0Cb6250d2887EF", "Basic Attention Token",
        "bat.png", true, false, "BAT", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"", "Ethereum", "", false, false, "ETH", 18, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48", "USD Coin", "usdc.png",
        true, false, "USDC", 6, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x6B175474E89094C44Da98b954EedeAC495271d0F", "DAI", "dai.png", true,
        false, "DAI", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x7Fc66500c84A76Ad7e9c93437bFc5Ac33E2DDaE9", "AAVE", "AAVE.png", true,
        false, "AAVE", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x4Fabb145d64652a948d72533023f6E7A623C7C53", "Binance USD", "busd.png",
        true, false, "BUSD", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0xc00e94Cb662C3520282E6f5717214004A7f26888", "Compound", "comp.png",
        true, false, "Comp", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0xD533a949740bb3306d119CC777fa900bA034cd52", "Curve", "curve.png",
        true, false, "CRV", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x056Fd409E1d7A124BD7017459dFEa2F387b6d5Cd", "Gemini Dollar",
        "gusd.png", true, false, "GUSD", 2, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0x514910771AF9Ca656af840dff83E8264EcF986CA", "Chainlink",
        "chainlink.png", true, false, "LINK", 18, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0x9f8F72aA9304c8B593d555F12eF6589cC3A579A2", "Maker", "mkr.png", true,
        false, "MKR", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0xC011a73ee8576Fb46F5E1c5751cA3B9Fe0af2a6F", "Synthetix",
        "synthetix.png", true, false, "SNX", 18, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0x04Fa0d235C4abf4BcF4787aF4CF447DE572eF828", "UMA", "UMA.png", true,
        false, "UMA", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x1f9840a85d5aF5bf1D1762F925BDADdC4201F984", "Uniswap", "uni.png",
        true, false, "UNI", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0xA4Bdb11dc0a2bEC88d24A3aa1E6Bb17201112eBe", "Stably Dollar",
        "usds.png", true, false, "USDS", 6, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0xdAC17F958D2ee523a2206206994597C13D831ec7", "Tether", "usdt.png",
        true, false, "USDT", 6, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x2260FAC5E5542a773Aa44fBCfeDf7C193bc2C599", "Wrapped Bitcoin",
        "wbtc.png", true, false, "WBTC", 8, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"0x0bc529c00C6401aEF6D220BE8C6Ea1667F6Ad93e", "Yearn.Finance",
        "yfi.png", true, false, "YFI", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x9043d4d51C9d2e31e3F169de4551E416970c27Ef", "Palm DAI", "pdai.png",
        true, false, "PDAI", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH}});
  return *tokens;
}

const std::vector<mojom::BlockchainToken>& GetRampBuyTokens() {
  static base::NoDestructor<std::vector<mojom::BlockchainToken>> tokens(
      {{"", "Ethereum", "", false, false, "ETH", 18, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH},
       {"", "Avalanche", "", false, false, "AVAX", 18, true, "", "",
        mojom::kAvalancheMainnetChainId, mojom::CoinType::ETH},
       {"0x82030cdbd9e4b7c5bb0b811a61da6360d69449cc", "RealFevr", "", true,
        false, "BSC_FEVR", 18, true, "", "",
        mojom::kBinanceSmartChainMainnetChainId, mojom::CoinType::ETH},
       {"", "Celo", "", false, false, "CELO", 18, true, "", "",
        mojom::kCeloMainnetChainId, mojom::CoinType::ETH},
       {"0xD8763CBa276a3738E6DE85b4b3bF5FDed6D6cA73", "Celo Euro", "ceur.png",
        true, false, "CEUR", 18, true, "", "", mojom::kCeloMainnetChainId,
        mojom::CoinType::ETH},
       {"0x765DE816845861e75A25fCA122bb6898B8B1282a", "Celo Dollar", "cusd.png",
        true, false, "CUSD", 18, true, "", "", mojom::kCeloMainnetChainId,
        mojom::CoinType::ETH},
       {"0x6b175474e89094c44da98b954eedeac495271d0f", "DAI Stablecoin",
        "dai.png", true, false, "DAI", 18, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"", "Polygon", "", false, false, "MATIC", 18, true, "", "",
        mojom::kPolygonMainnetChainId, mojom::CoinType::ETH},
       {"0x8f3cf7ad23cd3cadbd9735aff958023239c6a063", "DAI Stablecoin",
        "dai.png", true, false, "MATIC_DAI", 18, true, "", "",
        mojom::kPolygonMainnetChainId, mojom::CoinType::ETH},
       {"0x7ceB23fD6bC0adD59E62ac25578270cFf1b9f619", "Ethereum", "eth.png",
        true, false, "MATIC_ETH", 18, true, "", "",
        mojom::kPolygonMainnetChainId, mojom::CoinType::ETH},
       {"0xbbba073c31bf03b8acf7c28ef0738decf3695683", "Sandbox", "sand.png",
        true, false, "MATIC_SAND", 18, true, "", "",
        mojom::kPolygonMainnetChainId, mojom::CoinType::ETH},
       {"0x2791bca1f2de4661ed88a30c99a7a9449aa84174", "USD Coin", "usdc.png",
        true, false, "MATIC_USDC", 6, true, "", "",
        mojom::kPolygonMainnetChainId, mojom::CoinType::ETH},
       {"", "Solana", "", false, false, "SOLANA_SOL", 9, true, "", "",
        mojom::kSolanaMainnet, mojom::CoinType::SOL},
       {"Es9vMFrzaCERmJfrF4H2FYD4KCoNkY11McCe8BenwNYB", "Tether", "usdt.png",
        false, false, "SOLANA_USDT", 6, true, "", "", mojom::kSolanaMainnet,
        mojom::CoinType::SOL},
       {"0xdac17f958d2ee523a2206206994597c13d831ec7", "Tether", "usdt.png",
        true, false, "USDT", 6, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0xa0b86991c6218b36c1d19d4a2e9eb0ce3606eb48", "USD Coin", "usdc.png",
        true, false, "USDC", 6, true, "", "", mojom::kMainnetChainId,
        mojom::CoinType::ETH},
       {"0x0d8775f648430679a709e98d2b0cb6250d2887ef", "Basic Attention Token",
        "bat.png", true, false, "ETH_BAT", 18, true, "", "",
        mojom::kMainnetChainId, mojom::CoinType::ETH}});
  return *tokens;
}

}  // namespace brave_wallet
