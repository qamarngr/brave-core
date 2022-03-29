import * as React from 'react'
import {
  BraveWallet,
  BuySendSwapViewTypes,
  ToOrFromType,
  DefaultCurrencies,
  BuySupportedChains,
  BuyOption
} from '../../../constants/types'
import { NavButton } from '../../extension'
import SwapInputComponent from '../swap-input-component'
import { getLocale } from '../../../../common/locale'

// Styled Components
import {
  StyledWrapper,
  FaucetTitle,
  FaucetWrapper,
  FaucetDescription,
  Spacer,
  RampLogo,
  WyreLogo
} from './style'
import BuyWithButton from '../../buy-with-button'
import { BuyOptions } from '../../../options/buy-with-options'

export interface Props {
  selectedAsset: BraveWallet.BlockchainToken
  selectedNetwork: BraveWallet.NetworkInfo
  buyAmount: string
  networkList: BraveWallet.NetworkInfo[]
  defaultCurrencies: DefaultCurrencies
  onSubmit: () => void
  onInputChange: (value: string, name: string) => void
  onChangeBuyView: (view: BuySendSwapViewTypes, option?: ToOrFromType) => void
  selectedBuyOption: BraveWallet.OnRampProvider
  onSelectBuyOption: (optionId: BraveWallet.OnRampProvider) => void
  isAvailableOnWyre: boolean
  isAvailableOnRamp: boolean
}

function Buy (props: Props) {
  const {
    selectedNetwork,
    selectedAsset,
    buyAmount,
    networkList,
    defaultCurrencies,
    onInputChange,
    onSubmit,
    onChangeBuyView,
    selectedBuyOption,
    onSelectBuyOption,
    isAvailableOnWyre,
    isAvailableOnRamp
  } = props
  const [buyOptions, setBuyOptions] = React.useState<BuyOption[]>(BuyOptions)

  const onShowAssets = () => {
    onChangeBuyView('assets', 'from')
  }

  React.useEffect(() => {
    if (isAvailableOnRamp && isAvailableOnWyre) {
      setBuyOptions(BuyOptions)
    }

    if (isAvailableOnRamp && !isAvailableOnWyre) {
      setBuyOptions(BuyOptions.filter(option => option.id !== BraveWallet.OnRampProvider.kWyre))
    }

    if (!isAvailableOnRamp && isAvailableOnWyre) {
      setBuyOptions(BuyOptions.filter(option => option.id !== BraveWallet.OnRampProvider.kRamp))
    }
  }, [isAvailableOnWyre, isAvailableOnRamp])

  React.useEffect(() => {
    if (buyOptions.length > 0) {
      onSelectBuyOption(buyOptions[0]?.id)
    }
  }, [buyOptions])

  const networkName = React.useMemo((): string => {
    return networkList.find((network) => network.chainId === selectedNetwork.chainId)?.chainName ?? ''
  }, [networkList, selectedNetwork])

  const buyWithLabel = React.useMemo(() => {
    const selected = buyOptions.find(option => option.id === selectedBuyOption)

    return selected !== undefined ? selected.label : ''
  }, [selectedBuyOption])

  return (
    <StyledWrapper>
      {BuySupportedChains.includes(selectedNetwork.chainId) ? (
        <SwapInputComponent
          defaultCurrencies={defaultCurrencies}
          componentType='buyAmount'
          onInputChange={onInputChange}
          selectedAssetInputAmount={buyAmount}
          inputName='buy'
          selectedAsset={selectedAsset}
          selectedNetwork={selectedNetwork}
          onShowSelection={onShowAssets}
          autoFocus={true}
        />
      ) : (
        <FaucetWrapper>
          <FaucetTitle>{getLocale('braveWalletBuyTitle')}</FaucetTitle>
          <FaucetDescription>{getLocale('braveWalletBuyDescription').replace('$1', networkName)}</FaucetDescription>
        </FaucetWrapper>
      )}

      <BuyWithButton
        options={buyOptions}
        value={selectedBuyOption}
        onSelect={onSelectBuyOption}
        disabled={buyOptions?.length === 1}
      >
        {selectedBuyOption === BraveWallet.OnRampProvider.kRamp ? <RampLogo /> : <WyreLogo />}
        {buyWithLabel}
      </BuyWithButton>
      <Spacer />
      <NavButton
        disabled={false}
        buttonType='primary'
        text={getLocale('braveWalletBuyContinueButton')}
        onSubmit={onSubmit}
      />
    </StyledWrapper>
  )
}

export default Buy
