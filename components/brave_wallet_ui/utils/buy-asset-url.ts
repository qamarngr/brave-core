import {
  BraveWallet,
  UserAccountType
} from '../constants/types'

import { getBuyAssetUrl } from '../common/async/lib'
import Amount from './amount'

export function GetBuyOrFaucetUrl (onRampProvider: BraveWallet.OnRampProvider, networkChainId: string, asset: BraveWallet.BlockchainToken, account: UserAccountType, buyAmount: string): Promise<string> {
  const _buyAmount = (onRampProvider === BraveWallet.OnRampProvider.kRamp ? new Amount(buyAmount).multiplyByDecimals(asset.decimals).value?.toString(10) : buyAmount) || ''

  return new Promise(async (resolve, reject) => {
    switch (networkChainId) {
      case BraveWallet.MAINNET_CHAIN_ID:
        getBuyAssetUrl(onRampProvider, BraveWallet.MAINNET_CHAIN_ID, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.POLYGON_MAINNET_CHAIN_ID:
        getBuyAssetUrl(onRampProvider, BraveWallet.POLYGON_MAINNET_CHAIN_ID, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.BINANCE_SMART_CHAIN_MAINNET_CHAIN_ID:
        getBuyAssetUrl(onRampProvider, BraveWallet.BINANCE_SMART_CHAIN_MAINNET_CHAIN_ID, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.AVALANCHE_MAINNET_CHAIN_ID:
        getBuyAssetUrl(onRampProvider, BraveWallet.AVALANCHE_MAINNET_CHAIN_ID, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.CELO_MAINNET_CHAIN_ID:
        getBuyAssetUrl(onRampProvider, BraveWallet.CELO_MAINNET_CHAIN_ID, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.SOLANA_MAINNET:
        getBuyAssetUrl(onRampProvider, BraveWallet.SOLANA_MAINNET, account.address, asset.symbol, _buyAmount)
          .then(resolve)
          .catch(reject)
        break
      case BraveWallet.ROPSTEN_CHAIN_ID:
        resolve('https://faucet.dimensions.network/')
        break
      case BraveWallet.KOVAN_CHAIN_ID:
        resolve('https://github.com/kovan-testnet/faucet')
        break
      case BraveWallet.RINKEBY_CHAIN_ID:
        resolve('https://www.rinkeby.io/#faucet')
        break
      case BraveWallet.GOERLI_CHAIN_ID:
        resolve('https://goerli-faucet.slock.it/')
        break
      default:
        reject()
    }
  })
}
