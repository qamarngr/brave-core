import { getLocale } from '$web-common/locale'
import { BraveWallet, BuyOption } from '../constants/types'

export const BuyOptions = [
  {
    id: BraveWallet.OnRampProvider.kRamp,
    label: getLocale('braveWalletBuyWithRamp')
  },
  {
    id: BraveWallet.OnRampProvider.kWyre,
    label: getLocale('braveWalletBuyWithWyre')
  }
] as BuyOption[]
