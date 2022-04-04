import * as React from 'react'

import { StyledWrapper, PriceChange, ArrowDown, ArrowUp } from './style'

export interface Props {
  isDown: boolean
  priceChangePercentage: string
}

export const AssetPriceChange = (props: Props) => {
  const { isDown, priceChangePercentage } = props

  return (
    <StyledWrapper isDown={isDown}>
      {isDown ? <ArrowDown /> : <ArrowUp /> }
      <PriceChange>
        {priceChangePercentage}
      </PriceChange>
    </StyledWrapper>
  )
}
