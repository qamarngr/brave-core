/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Constants
import {
  BraveWallet,
  SolFeeEstimates,
  TimeDelta,
  WalletAccountType
} from '../../constants/types'
import { MAX_UINT256 } from '../constants/magics'

// Utils
import Amount from '../../utils/amount'
import { getLocale } from '../../../common/locale'

// Hooks
import usePricing from './pricing'
import useAddressLabels, { SwapExchangeProxy } from './address-labels'
import useBalance from './balance'

// Options
import { makeNetworkAsset } from '../../options/asset-options'

interface ParsedTransactionFees {
  gasLimit: string
  gasPrice: string
  maxPriorityFeePerGas: string
  maxFeePerGas: string
  gasFee: string
  gasFeeFiat: string
  isEIP1559Transaction: boolean
  missingGasLimitError?: string
}

export interface ParsedTransaction extends ParsedTransactionFees {
  // Common fields
  hash: string
  nonce: string
  createdTime: TimeDelta
  status: BraveWallet.TransactionStatus
  sender: string
  senderLabel: string
  recipient: string
  recipientLabel: string
  fiatValue: Amount
  fiatTotal: Amount
  formattedNativeCurrencyTotal: string
  value: string
  valueExact: string
  symbol: string
  decimals: number
  insufficientFundsError: boolean
  contractAddressError?: string
  sameAddressError?: string
  erc721BlockchainToken?: BraveWallet.BlockchainToken
  erc721TokenId?: string
  isSwap?: boolean

  // Token approvals
  approvalTarget?: string
  approvalTargetLabel?: string
  isApprovalUnlimited?: boolean
}

export function useTransactionFeesParser (selectedNetwork: BraveWallet.NetworkInfo, networkSpotPrice: string, solFeeEstimates?: SolFeeEstimates) {
  /**
   * Checks if a given gasLimit is empty or zero-value, and returns an
   * appropriate localized error string.
   *
   * @remarks
   *
   * This function may only be used on ALL transaction types.
   *
   * @param gasLimit - The parsed gasLimit string.
   * @returns Localized string describing the error, or undefined in case of
   * no error.
   */
  const checkForMissingGasLimitError = React.useCallback((gasLimit: string): string | undefined => {
    return (gasLimit === '' || Amount.normalize(gasLimit) === '0')
      ? undefined //getLocale('braveWalletMissingGasLimitError')
      : undefined
  }, [])
  const getTransactionFees = (transactionInfo: BraveWallet.TransactionInfo)  => {
    const { txDataUnion: { ethTxData1559: txData, solanaTxData: solTxData, filTxData: filTxData } } = transactionInfo
    if (filTxData) {
      return {
        gasLimit: filTxData.gasLimit,
        gasPrice: filTxData.gasPremium,
        maxFeePerGas: filTxData.maxFee,
        maxPriorityFeePerGas: filTxData.maxFee,
        gasFee: filTxData.gasFeeCap
      }
    } else if (solTxData) {
      const gasLimit = txData?.baseData.gasLimit || ''
      const gasPrice = txData?.baseData.gasPrice || ''
      const maxFeePerGas = txData?.maxFeePerGas || ''
      const maxPriorityFeePerGas = txData?.maxPriorityFeePerGas || ''
      const gasFee =  solFeeEstimates?.fee.toString() ?? ''
      return {
        gasLimit: gasLimit,
        gasPrice: gasPrice,
        maxFeePerGas: maxFeePerGas,
        maxPriorityFeePerGas: maxPriorityFeePerGas,
        gasFee: gasFee
      }
    }
    const gasLimit = txData?.baseData.gasLimit || ''
    const gasPrice = txData?.baseData.gasPrice || ''
    const maxFeePerGas = txData?.maxFeePerGas || ''
    const maxPriorityFeePerGas = txData?.maxPriorityFeePerGas || ''
    const isEIP1559Transaction = maxPriorityFeePerGas !== '' && maxFeePerGas !== ''
    const gasFee = isEIP1559Transaction
        ? new Amount(maxFeePerGas)
          .times(gasLimit)
          .format()
        : new Amount(gasPrice)
          .times(gasLimit)
          .format()
    return {
      gasLimit: gasLimit,
      gasPrice: gasPrice,
      maxFeePerGas: maxFeePerGas,
      maxPriorityFeePerGas: maxPriorityFeePerGas,
      gasFee: gasFee
    }
  }

  return React.useCallback((transactionInfo: BraveWallet.TransactionInfo): ParsedTransactionFees => {
    const { txType } = transactionInfo
    const isSolTransaction = txType === BraveWallet.TransactionType.SolanaSystemTransfer
    const { gasLimit, gasPrice, maxFeePerGas, maxPriorityFeePerGas, gasFee } = getTransactionFees(transactionInfo)
    const isEIP1559Transaction = maxPriorityFeePerGas !== '' && maxFeePerGas !== ''
    return {
      gasLimit: Amount.normalize(gasLimit),
      gasPrice: Amount.normalize(gasPrice),
      maxFeePerGas: Amount.normalize(maxFeePerGas),
      maxPriorityFeePerGas: Amount.normalize(maxPriorityFeePerGas),
      gasFee,
      gasFeeFiat: Amount.normalize(gasFee),
      isEIP1559Transaction,
      missingGasLimitError: isSolTransaction ? undefined : checkForMissingGasLimitError(gasLimit)
    }
  }, [selectedNetwork, networkSpotPrice])
}

export function useTransactionParser (
  selectedNetwork: BraveWallet.NetworkInfo,
  accounts: WalletAccountType[],
  spotPrices: BraveWallet.AssetPrice[],
  visibleTokens: BraveWallet.BlockchainToken[],
  fullTokenList?: BraveWallet.BlockchainToken[],
  solFeeEstimates?: SolFeeEstimates
) {
  const nativeAsset = React.useMemo(
    () => makeNetworkAsset(selectedNetwork),
    [selectedNetwork]
  )
  const { findAssetPrice, computeFiatAmount } = usePricing(spotPrices)
  const getBalance = useBalance([selectedNetwork])
  const getAddressLabel = useAddressLabels(accounts)

  const networkSpotPrice = React.useMemo(
    () => findAssetPrice(selectedNetwork.symbol),
    [selectedNetwork, findAssetPrice]
  )
  const parseTransactionFees = useTransactionFeesParser(selectedNetwork, networkSpotPrice, solFeeEstimates)

  const findToken = React.useCallback((contractAddress: string) => {
    const checkVisibleList = visibleTokens.find((token) => token.contractAddress.toLowerCase() === contractAddress.toLowerCase())
    return checkVisibleList ?? (fullTokenList?.find((token) => token.contractAddress.toLowerCase() === contractAddress.toLowerCase()))
  }, [visibleTokens, fullTokenList])

  /**
   * Checks if a given address is a known contract address from our token
   * registry.
   *
   * @remarks
   *
   * This function must only be used for the following transaction types:
   *  - ERC20Transfer
   *  - ERC721TransferFrom
   *  - ERC721SafeTransferFrom
   *
   * @param to - The address to check
   * @returns Localized string describing the error, or undefined in case of
   * no error.
   */
  const checkForContractAddressError = (to: string): string | undefined => {
    return fullTokenList?.some(token => token.contractAddress.toLowerCase() === to.toLowerCase())
      ? getLocale('braveWalletContractAddressError')
      : undefined
  }

  /**
   * Checks if a given set of sender and recipient addresses are the
   * same.
   *
   * @remarks
   *
   * This function must only be used for the following transaction types:
   *  - ERC20Transfer
   *  - ERC721TransferFrom
   *  - ERC721SafeTransferFrom
   *  - ERC20Approve
   *  - ETHSend
   *
   * @param to - The recipient address
   * @param from - The sender address
   */
  const checkForSameAddressError = (to: string, from: string): string | undefined => {
    return to.toLowerCase() === from.toLowerCase()
      ? getLocale('braveWalletSameAddressError')
      : undefined
  }
  const getTransactionInfo = (transactionInfo: BraveWallet.TransactionInfo)  => {
    const { txDataUnion: { ethTxData1559: txData, solanaTxData: solTxData, filTxData: filTxData } } = transactionInfo
    if (filTxData) {
      return { value: filTxData.value ?? '', to: filTxData.to ?? '', nonce: filTxData.nonce }
    } else if (solTxData) {
      return { value: solTxData?.lamports.toString() ?? '', to: solTxData?.toWalletAddress ?? '', nonce: '' }
    }
    return { value: txData?.baseData.value ?? '', to: txData?.baseData.to ?? '', nonce: txData?.baseData.nonce }
  }
  return React.useCallback((transactionInfo: BraveWallet.TransactionInfo) => {
    const { txArgs, fromAddress, txType } = transactionInfo
    const { value, to, nonce }  = getTransactionInfo(transactionInfo)
    const account = accounts.find((account) => account.address.toLowerCase() === fromAddress.toLowerCase())
    const token = findToken(to)
    const accountNativeBalance = getBalance(account, nativeAsset)
    const accountTokenBalance = getBalance(account, token)
    const symbol  = 'FIL' //token?.symbol ?? ''
    switch (true) {
      // transfer(address recipient, uint256 amount) → bool
      case txType === BraveWallet.TransactionType.ERC20Transfer: {
        const [address, amount] = txArgs
        const price = findAssetPrice(symbol)
        const sendAmountFiat = new Amount(amount)
          .divideByDecimals(token?.decimals ?? 18)
          .times(price)

        const feeDetails = parseTransactionFees(transactionInfo)
        const { gasFeeFiat, gasFee } = feeDetails
        const totalAmountFiat = new Amount(gasFeeFiat)
          .plus(sendAmountFiat)

        const insufficientNativeFunds1 = new Amount(gasFee)
          .gt(accountNativeBalance)
        const insufficientTokenFunds1 = new Amount(amount)
          .gt(accountTokenBalance)
        console.log(insufficientNativeFunds1, insufficientTokenFunds1)
        const insufficientNativeFunds = true
        const insufficientTokenFunds = true

        console.log(getAddressLabel(fromAddress))
        return {
          hash: transactionInfo.txHash,
          nonce,
          createdTime: transactionInfo.createdTime,
          status: transactionInfo.txStatus,
          sender: fromAddress,
          senderLabel: getAddressLabel(fromAddress),
          recipient: address,
          recipientLabel: getAddressLabel(address),
          fiatValue: sendAmountFiat,
          fiatTotal: totalAmountFiat,
          formattedNativeCurrencyTotal: sendAmountFiat
            .div(networkSpotPrice)
            .formatAsAsset(6, symbol),
          value: new Amount(amount)
            .format(6),
          valueExact: new Amount(amount)
            .format(),
          symbol: symbol,
          decimals: token?.decimals ?? 18,
          insufficientFundsError: insufficientNativeFunds || insufficientTokenFunds,
          contractAddressError: checkForContractAddressError(address),
          sameAddressError: checkForSameAddressError(address, fromAddress),
          ...feeDetails
        } as ParsedTransaction
      }

      // transferFrom(address owner, address to, uint256 tokenId)
      case txType === BraveWallet.TransactionType.ERC721TransferFrom:

      // safeTransferFrom(address owner, address to, uint256 tokenId)
      case txType === BraveWallet.TransactionType.ERC721SafeTransferFrom: {
        // The owner of the ERC721 must not be confused with the
        // caller (fromAddress).
        const [owner, toAddress, tokenID] = txArgs

        const feeDetails = parseTransactionFees(transactionInfo)
        const { gasFeeFiat, gasFee } = feeDetails
        const totalAmountFiat = gasFeeFiat

        const insufficientNativeFunds = new Amount(gasFee)
          .gt(accountNativeBalance)

        return {
          hash: transactionInfo.txHash,
          nonce,
          createdTime: transactionInfo.createdTime,
          status: transactionInfo.txStatus,
          sender: fromAddress, // The caller, which may not be the owner
          senderLabel: getAddressLabel(fromAddress),
          recipient: toAddress,
          recipientLabel: getAddressLabel(toAddress),
          fiatValue: Amount.zero(), // Display NFT values in the future
          fiatTotal: new Amount(totalAmountFiat),
          formattedNativeCurrencyTotal: totalAmountFiat && new Amount(totalAmountFiat)
            .div(networkSpotPrice)
            .formatAsAsset(6, symbol),
          value: '1', // Can only send 1 erc721 at a time
          valueExact: '1',
          symbol: symbol,
          decimals: 0,
          insufficientFundsError: insufficientNativeFunds,
          erc721BlockchainToken: token,
          erc721TokenId: tokenID && `#${Amount.normalize(tokenID)}`,
          contractAddressError: checkForContractAddressError(toAddress),
          sameAddressError: checkForSameAddressError(toAddress, owner),
          ...feeDetails
        } as ParsedTransaction
      }

      // approve(address spender, uint256 amount) → bool
      case txType === BraveWallet.TransactionType.ERC20Approve: {
        const [address, amount] = txArgs
        const feeDetails = parseTransactionFees(transactionInfo)
        const { gasFeeFiat, gasFee } = feeDetails
        const totalAmountFiat = new Amount(gasFeeFiat)
        const insufficientNativeFunds = new Amount(gasFee)
          .gt(accountNativeBalance)

        const amountWrapped = new Amount(amount)

        return {
          hash: transactionInfo.txHash,
          nonce,
          createdTime: transactionInfo.createdTime,
          status: transactionInfo.txStatus,
          sender: fromAddress,
          senderLabel: getAddressLabel(fromAddress),
          recipient: to,
          recipientLabel: getAddressLabel(to),
          fiatValue: Amount.zero(),
          fiatTotal: totalAmountFiat,
          formattedNativeCurrencyTotal: Amount.zero()
            .formatAsAsset(2, symbol),
          value: amountWrapped
            .divideByDecimals(token?.decimals ?? 18)
            .format(6),
          valueExact: amountWrapped
            .divideByDecimals(token?.decimals ?? 18)
            .format(),
          symbol: symbol,
          decimals: token?.decimals ?? 18,
          approvalTarget: address,
          approvalTargetLabel: getAddressLabel(address),
          isApprovalUnlimited: amountWrapped.eq(MAX_UINT256),
          insufficientFundsError: insufficientNativeFunds,
          sameAddressError: checkForSameAddressError(address, fromAddress),
          ...feeDetails
        } as ParsedTransaction
      }

      // FIXME: swap needs a real parser to figure out the From and To details.
      case to.toLowerCase() === SwapExchangeProxy:
      case txType === BraveWallet.TransactionType.ETHSend:
      case txType === BraveWallet.TransactionType.SolanaSystemTransfer:
      case txType === BraveWallet.TransactionType.Other:
      default: {
        const sendAmountFiat = computeFiatAmount(value, symbol, selectedNetwork.decimals)

        const feeDetails = parseTransactionFees(transactionInfo)
        const { gasFeeFiat, gasFee } = feeDetails
        const totalAmountFiat = new Amount(gasFeeFiat)
          .plus(sendAmountFiat)
        return {
          hash: transactionInfo.txHash,
          nonce,
          createdTime: transactionInfo.createdTime,
          status: transactionInfo.txStatus,
          sender: fromAddress,
          senderLabel: getAddressLabel(fromAddress),
          recipient: to,
          recipientLabel: getAddressLabel(to),
          fiatValue: sendAmountFiat,
          fiatTotal: totalAmountFiat,
          formattedNativeCurrencyTotal: sendAmountFiat
            .div(networkSpotPrice)
            .formatAsAsset(6, symbol),
          value: new Amount(value)
            .divideByDecimals(selectedNetwork.decimals)
            .format(6),
          valueExact: new Amount(value)
            .divideByDecimals(selectedNetwork.decimals)
            .format(),
          symbol: symbol,
          decimals: selectedNetwork?.decimals ?? 18,
          insufficientFundsError: new Amount(value)
            .plus(gasFee)
            .gt(accountNativeBalance),
          isSwap: to.toLowerCase() === SwapExchangeProxy,
          ...feeDetails
        } as ParsedTransaction
      }
    }
  }, [selectedNetwork, accounts, spotPrices, findToken])
}
