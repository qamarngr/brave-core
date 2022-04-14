import {
  BraveWallet,
  UserAccountType
} from '../constants/types'

import { getBuyAssetUrl } from '../common/async/lib'
import Amount from './amount'

export function GetBuyOrFaucetUrl (onRampProvider: BraveWallet.OnRampProvider, networkChainId: string, asset: BraveWallet.BlockchainToken, account: UserAccountType, buyAmount: string): Promise<string> {
  const _buyAmount = (onRampProvider === BraveWallet.OnRampProvider.kRamp ? new Amount(buyAmount).multiplyByDecimals(asset.decimals).value?.toString(10) : buyAmount) || ''

  return getBuyAssetUrl(onRampProvider, networkChainId, account.address, asset.symbol, _buyAmount)
}
