### VAP_Controler
- A simple study that allows you to increment/decrement (by the value of your choice) the "Volume At Price Multiplier for Intraday Chart" value through an ACS_Button.
	- credits: FrozenTundra video and shared code helped me a lot

### RiskManager
- WORK IN PROGRESS, should be a study who manage the risk based on Entry Location x orderType x RVWAP

### ChartManager
- A study that automatically change the Bar Period and the VAP Multiplier based on the Subgraph Value given and a Scaling Factor. I use only range bars but I think it's okay for any non-Time Based Bar Type, example with my parameters:
    - Subgraph Value = ATR 5m.
    - Scaling Factor = 5.
    e.g:
        -> ATR == 8, so rounded to 10.
        -> VAP = 10 / 5 = 5.
    e.g2:
        -> ATR == 33, so rounded to 35.
        -> VAP = 35 / 5 = 7.
- The next step of this study would be to be able to change the "Market Depth Combine Increment in Ticks" of my DOM Chart to the same Value of the VAP, in order to have a constant view of the Market, whatever the level of Volatility. But actually I don't have any clue if this is even possible.
