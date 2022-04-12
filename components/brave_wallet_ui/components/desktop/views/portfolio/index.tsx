import * as React from 'react'

// Constants
import {
  PriceDataObjectType,
  AccountTransactions,
  BraveWallet,
  WalletAccountType,
  UserAssetInfoType,
  DefaultCurrencies,
  AddAccountNavTypes
} from '../../../../constants/types'
import { getLocale } from '../../../../../common/locale'
import { CurrencySymbols } from '../../../../utils/currency-symbols'

// Utils
import { sortTransactionByDate } from '../../../../utils/tx-utils'
import { getTokensNetwork, getTokensCoinType } from '../../../../utils/network-utils'
import Amount from '../../../../utils/amount'

// Options
import { ChartTimelineOptions } from '../../../../options/chart-timeline-options'

// Components
import { BackButton, LoadingSkeleton, withPlaceholderIcon } from '../../../shared'
import {
  ChartControlBar,
  LineChart,
  EditVisibleAssetsModal,
  WithHideBalancePlaceholder
} from '../../'

// import NFTDetails from './components/nft-details'
import TokenLists from './components/token-lists'
import AccountsAndTransactionsList from './components/accounts-and-transctions-list'

// Hooks
import { usePricing, useTransactionParser } from '../../../../common/hooks'

// Styled Components
import {
  StyledWrapper,
  TopRow,
  BalanceTitle,
  BalanceText,
  AssetIcon,
  AssetRow,
  AssetColumn,
  PriceRow,
  AssetNameText,
  DetailText,
  InfoColumn,
  PriceText,
  PercentBubble,
  PercentText,
  ArrowIcon,
  BalanceRow,
  ShowBalanceButton,
  NetworkDescription
} from './style'

export interface Props {
  toggleNav: () => void
  onChangeTimeline: (path: BraveWallet.AssetPriceTimeframe) => void
  onSelectAsset: (asset: BraveWallet.BlockchainToken | undefined) => void
  onSelectAccount: (account: WalletAccountType) => void
  onClickAddAccount: (tabId: AddAccountNavTypes) => () => void
  onAddCustomAsset: (token: BraveWallet.BlockchainToken) => void
  onShowVisibleAssetsModal: (showModal: boolean) => void
  onUpdateVisibleAssets: (updatedTokensList: BraveWallet.BlockchainToken[]) => void
  showVisibleAssetsModal: boolean
  defaultCurrencies: DefaultCurrencies
  addUserAssetError: boolean
  selectedNetwork: BraveWallet.NetworkInfo
  networkList: BraveWallet.NetworkInfo[]
  userAssetList: UserAssetInfoType[]
  accounts: WalletAccountType[]
  selectedTimeline: BraveWallet.AssetPriceTimeframe
  selectedPortfolioTimeline: BraveWallet.AssetPriceTimeframe
  selectedAsset: BraveWallet.BlockchainToken | undefined
  selectedAssetFiatPrice: BraveWallet.AssetPrice | undefined
  selectedAssetCryptoPrice: BraveWallet.AssetPrice | undefined
  selectedAssetPriceHistory: PriceDataObjectType[]
  portfolioPriceHistory: PriceDataObjectType[]
  portfolioBalance: string
  transactions: AccountTransactions
  isLoading: boolean
  fullAssetList: BraveWallet.BlockchainToken[]
  userVisibleTokensInfo: BraveWallet.BlockchainToken[]
  isFetchingPortfolioPriceHistory: boolean
  onRetryTransaction: (transaction: BraveWallet.TransactionInfo) => void
  onSpeedupTransaction: (transaction: BraveWallet.TransactionInfo) => void
  onCancelTransaction: (transaction: BraveWallet.TransactionInfo) => void
  onFindTokenInfoByContractAddress: (contractAddress: string) => void
  foundTokenInfoByContractAddress: BraveWallet.BlockchainToken | undefined
}

const Portfolio = (props: Props) => {
  const {
    toggleNav,
    onChangeTimeline,
    onSelectAsset,
    onSelectAccount,
    onClickAddAccount,
    onAddCustomAsset,
    onShowVisibleAssetsModal,
    onUpdateVisibleAssets,
    showVisibleAssetsModal,
    defaultCurrencies,
    addUserAssetError,
    userVisibleTokensInfo,
    selectedNetwork,
    fullAssetList,
    portfolioPriceHistory,
    selectedAssetPriceHistory,
    selectedAssetFiatPrice,
    selectedAssetCryptoPrice,
    selectedTimeline,
    selectedPortfolioTimeline,
    accounts,
    networkList,
    selectedAsset,
    portfolioBalance,
    transactions,
    userAssetList,
    isLoading,
    isFetchingPortfolioPriceHistory,
    onRetryTransaction,
    onSpeedupTransaction,
    onCancelTransaction,
    onFindTokenInfoByContractAddress,
    foundTokenInfoByContractAddress
  } = props

  const [filteredAssetList, setfilteredAssetList] = React.useState<UserAssetInfoType[]>(userAssetList)
  const [fullPortfolioFiatBalance, setFullPortfolioFiatBalance] = React.useState<string>(portfolioBalance)
  const [hoverBalance, setHoverBalance] = React.useState<string>()
  const [hoverPrice, setHoverPrice] = React.useState<string>()
  const [hideBalances, setHideBalances] = React.useState<boolean>(false)
  const selectedAssetsNetwork = React.useMemo(() => {
    if (!selectedAsset) {
      return selectedNetwork
    }
    return getTokensNetwork(networkList, selectedAsset)
  }, [selectedNetwork, selectedAsset, networkList])
  const parseTransaction = useTransactionParser()

  const onSetFilteredAssetList = (filteredList: UserAssetInfoType[]) => {
    setfilteredAssetList(filteredList)
  }

  React.useEffect(() => {
    if (portfolioBalance !== '') {
      setFullPortfolioFiatBalance(portfolioBalance)
    }
  }, [portfolioBalance])

  React.useEffect(() => {
    setfilteredAssetList(userAssetList)
  }, [userAssetList])

  const portfolioHistory = React.useMemo(() => {
    return portfolioPriceHistory
  }, [portfolioPriceHistory])

  const selectAsset = (asset: BraveWallet.BlockchainToken) => () => {
    onSelectAsset(asset)
    toggleNav()
  }

  const goBack = () => {
    onSelectAsset(undefined)
    setfilteredAssetList(userAssetList)
    toggleNav()
  }

  const onUpdateBalance = (value: number | undefined) => {
    if (!selectedAsset) {
      if (value) {
        setHoverBalance(new Amount(value).formatAsFiat())
      } else {
        setHoverBalance(undefined)
      }
    } else {
      if (value) {
        setHoverPrice(new Amount(value).formatAsFiat())
      } else {
        setHoverPrice(undefined)
      }
    }
  }

  const toggleShowVisibleAssetModal = () => {
    onShowVisibleAssetsModal(!showVisibleAssetsModal)
  }

  const priceHistory = React.useMemo(() => {
    if (parseFloat(portfolioBalance) === 0) {
      return []
    } else {
      return portfolioHistory
    }
  }, [portfolioHistory, portfolioBalance])

  const transactionsByNetwork = React.useMemo(() => {
    const accountsByNetwork = accounts.filter((account) => account.coin === selectedAssetsNetwork.coin)
    return accountsByNetwork.map((account) => {
      return transactions[account.address]
    }).flat(1)
  }, [accounts, transactions, selectedAssetsNetwork])

  const selectedAssetTransactions = React.useMemo((): BraveWallet.TransactionInfo[] => {
    if (selectedAsset) {
      const filteredTransactions = transactionsByNetwork.filter((tx) => {
        return parseTransaction(tx).symbol === selectedAsset?.symbol ? tx : []
      })
      return sortTransactionByDate(filteredTransactions, 'descending')
    }
    return []
  }, [selectedAsset, transactionsByNetwork])

  const fullAssetBalances = React.useMemo(() => {
    if (selectedAsset?.contractAddress === '') {
      return filteredAssetList.find(
        (asset) =>
          asset.asset.symbol.toLowerCase() === selectedAsset?.symbol.toLowerCase() &&
          asset.asset.chainId === selectedAsset?.chainId
      )
    }
    return filteredAssetList.find(
      (asset) =>
        asset.asset.contractAddress.toLowerCase() === selectedAsset?.contractAddress.toLowerCase() &&
        asset.asset.chainId === selectedAsset?.chainId
    )
  }, [filteredAssetList, selectedAsset])

  const AssetIconWithPlaceholder = React.useMemo(() => {
    return withPlaceholderIcon(AssetIcon, { size: 'big', marginLeft: 0, marginRight: 12 })
  }, [])

  const formattedFullAssetBalance = fullAssetBalances?.assetBalance
    ? '(' + new Amount(fullAssetBalances?.assetBalance ?? '')
      .divideByDecimals(selectedAsset?.decimals ?? 18)
      .formatAsAsset(6, selectedAsset?.symbol ?? '') + ')'
    : ''

  const { computeFiatAmount } = usePricing()
  const fullAssetFiatBalance = fullAssetBalances?.assetBalance
    ? computeFiatAmount(
      fullAssetBalances.assetBalance,
      fullAssetBalances.asset.symbol,
      fullAssetBalances.asset.decimals
    )
    : Amount.empty()

  const onToggleHideBalances = () => {
    setHideBalances(!hideBalances)
  }

  const filteredAccountsByCoinType = React.useMemo(() => {
    if (!selectedAsset) {
      return []
    }
    const coinType = getTokensCoinType(networkList, selectedAsset)
    return accounts.filter((account) => account.coin === coinType)
  }, [networkList, accounts, selectedAsset])

  return (
    <StyledWrapper>
      <TopRow>
        <BalanceRow>
          {!selectedAsset ? (
            <BalanceTitle>{getLocale('braveWalletBalance')}</BalanceTitle>
          ) : (
            <BackButton onSubmit={goBack} />
          )}
        </BalanceRow>
        <BalanceRow>
          {!selectedAsset?.isErc721 &&
            <ChartControlBar
              onSubmit={onChangeTimeline}
              selectedTimeline={selectedAsset ? selectedTimeline : selectedPortfolioTimeline}
              timelineOptions={ChartTimelineOptions()}
            />
          }
          <ShowBalanceButton
            hideBalances={hideBalances}
            onClick={onToggleHideBalances}
          />
        </BalanceRow>
      </TopRow>
      {!selectedAsset ? (
        <WithHideBalancePlaceholder
          size='big'
          hideBalances={hideBalances}
        >
          <BalanceText>
            {fullPortfolioFiatBalance !== ''
              ? `${CurrencySymbols[defaultCurrencies.fiat]}${hoverBalance || fullPortfolioFiatBalance}`
              : <LoadingSkeleton width={150} height={32} />
            }
          </BalanceText>
        </WithHideBalancePlaceholder>
      ) : (
        <>
          {!selectedAsset.isErc721 &&
            <InfoColumn>
              <AssetRow>
                <AssetIconWithPlaceholder asset={selectedAsset} network={selectedAssetsNetwork} />
                <AssetColumn>
                  <AssetNameText>{selectedAsset.name}</AssetNameText>
                  <NetworkDescription>{selectedAsset.symbol} on {selectedAssetsNetwork?.chainName ?? ''}</NetworkDescription>
                </AssetColumn>
              </AssetRow>
              {/* <DetailText>{selectedAsset.name} {getLocale('braveWalletPrice')} ({selectedAsset.symbol})</DetailText> */}
              <PriceRow>
                <PriceText>{CurrencySymbols[defaultCurrencies.fiat]}{hoverPrice || (selectedAssetFiatPrice ? new Amount(selectedAssetFiatPrice.price).formatAsFiat() : 0.00)}</PriceText>
                <PercentBubble isDown={selectedAssetFiatPrice ? Number(selectedAssetFiatPrice.assetTimeframeChange) < 0 : false}>
                  <ArrowIcon isDown={selectedAssetFiatPrice ? Number(selectedAssetFiatPrice.assetTimeframeChange) < 0 : false} />
                  <PercentText>{selectedAssetFiatPrice ? Number(selectedAssetFiatPrice.assetTimeframeChange).toFixed(2) : 0.00}%</PercentText>
                </PercentBubble>
              </PriceRow>
              <DetailText>
                {
                  selectedAssetCryptoPrice
                    ? new Amount(selectedAssetCryptoPrice.price)
                      .formatAsAsset(undefined, defaultCurrencies.crypto)
                    : ''
                }
              </DetailText>
            </InfoColumn>
          }
        </>
      )}
      {!selectedAsset?.isErc721 &&
        <LineChart
          isDown={selectedAsset && selectedAssetFiatPrice ? Number(selectedAssetFiatPrice.assetTimeframeChange) < 0 : false}
          isAsset={!!selectedAsset}
          priceData={selectedAsset ? selectedAssetPriceHistory : priceHistory}
          onUpdateBalance={onUpdateBalance}
          isLoading={selectedAsset ? isLoading : parseFloat(portfolioBalance) === 0 ? false : isFetchingPortfolioPriceHistory}
          isDisabled={selectedAsset ? false : parseFloat(portfolioBalance) === 0}
        />
      }

      {/* Temp Commented out until we have an API to get NFT MetaData */}
      {/* {selectedAsset?.isErc721 &&
        <NFTDetails
          selectedAsset={selectedAsset}
          nftMetadata={}
          defaultCurrencies={defaultCurrencies}
          selectedNetwork={selectedNetwork}
        />
      } */}

      <AccountsAndTransactionsList
        accounts={filteredAccountsByCoinType}
        defaultCurrencies={defaultCurrencies}
        formattedFullAssetBalance={formattedFullAssetBalance}
        fullAssetFiatBalance={fullAssetFiatBalance}
        selectedAsset={selectedAsset}
        selectedAssetTransactions={selectedAssetTransactions}
        selectedNetwork={selectedAssetsNetwork ?? selectedNetwork}
        userVisibleTokensInfo={userVisibleTokensInfo}
        onClickAddAccount={onClickAddAccount}
        onSelectAccount={onSelectAccount}
        onSelectAsset={selectAsset}
        onCancelTransaction={onCancelTransaction}
        onRetryTransaction={onRetryTransaction}
        onSpeedupTransaction={onSpeedupTransaction}
        hideBalances={hideBalances}
        networkList={networkList}
      />
      {!selectedAsset &&
        <TokenLists
          defaultCurrencies={defaultCurrencies}
          userAssetList={userAssetList}
          filteredAssetList={filteredAssetList}
          networks={networkList}
          onSetFilteredAssetList={onSetFilteredAssetList}
          onSelectAsset={selectAsset}
          onShowAssetModal={toggleShowVisibleAssetModal}
          hideBalances={hideBalances}
        />
      }
      {showVisibleAssetsModal &&
        <EditVisibleAssetsModal
          fullAssetList={fullAssetList}
          userVisibleTokensInfo={userVisibleTokensInfo}
          addUserAssetError={addUserAssetError}
          onClose={toggleShowVisibleAssetModal}
          onAddCustomAsset={onAddCustomAsset}
          selectedNetwork={selectedNetwork}
          networkList={networkList}
          onFindTokenInfoByContractAddress={onFindTokenInfoByContractAddress}
          foundTokenInfoByContractAddress={foundTokenInfoByContractAddress}
          onUpdateVisibleAssets={onUpdateVisibleAssets}
        />
      }
    </StyledWrapper>
  )
}

export default Portfolio
