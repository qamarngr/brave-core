import * as React from 'react'
import Fuse from 'fuse.js'
import { BraveWallet, MarketDataTableColumnTypes, SortOrder } from '../../constants/types'

const searchOptions: Fuse.IFuseOptions<BraveWallet.CoinMarket> = {
  shouldSort: true,
  threshold: 0.1,
  location: 0,
  distance: 0,
  minMatchCharLength: 1,
  keys: [
    { name: 'name', weight: 0.5 },
    { name: 'symbol', weight: 0.5 }
  ]
}

export const useMarketDataManagement = (marketData: BraveWallet.CoinMarket[], sortOrder: SortOrder, columnId: MarketDataTableColumnTypes) => {
  const sortCoinMarketData = React.useCallback(() => {
    const sortedMarketData = [...marketData]

    if (sortOrder === 'asc') {
      sortedMarketData.sort((a, b) => a[columnId] - b[columnId])
    } else {
      sortedMarketData.sort((a, b) => b[columnId] - a[columnId])
    }

    return sortedMarketData
  }, [marketData, sortOrder, columnId])

  const searchCoinMarkets = React.useCallback((searchList: BraveWallet.CoinMarket[], searchTerm: string) => {
    if (!searchTerm) {
      return searchList
    }

    const fuse = new Fuse(searchList, searchOptions)
    return fuse.search(searchTerm)
      .map((result: Fuse.FuseResult<BraveWallet.CoinMarket>) => result.item)
  }, [marketData])

  return {
    sortCoinMarketData,
    searchCoinMarkets
  }
}
