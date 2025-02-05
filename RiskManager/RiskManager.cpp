#include "RiskManager.h"

int SL_State = 0;
int TSL_State = 0;
int Scenario = 0;

SCDLLName("ISB TSL Management")

void HandlePointersEvents(int &LastLogMessageIdentifier, SCStudyInterfaceRef sc, SCInputRef Input_ACSButtonNumber, SCInputRef Input_Enabled,
	SCInputRef Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable, SCInputRef Input_AllowCustomPropertiesForControlBarButton) {
	// Handle pointer events
	if (sc.IsFullRecalculation)//Is full recalculation
	{
		// set ACS Tool Control Bar tool tip
		sc.SetCustomStudyControlBarButtonHoverText(Input_ACSButtonNumber.GetInt(), "Enable/Disable Trade Management by Study");

		// set Custom Study Control Bar button text if the input to allow custom properties is not true. Otherwise, rely upon what the user sets.
		if (!Input_AllowCustomPropertiesForControlBarButton.GetYesNo())
			sc.SetCustomStudyControlBarButtonShortCaption(Input_ACSButtonNumber.GetInt(), "TradeMan\nStudy");

		sc.SetCustomStudyControlBarButtonEnable(Input_ACSButtonNumber.GetInt(), Input_Enabled.GetBoolean() );

		LastLogMessageIdentifier = 0;
	}

	if (Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable.GetYesNo())
	{
		if (sc.MenuEventID >= ACS_BUTTON_1 && sc.MenuEventID <= ACS_BUTTON_150)
			sc.SetCustomStudyControlBarButtonEnable(sc.PriorSelectedCustomStudyControlBarButtonNumber, false);
	}

	// Wait for an event to enable study if not already enabled
	if (sc.MenuEventID == Input_ACSButtonNumber.GetInt())
	{
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		Input_Enabled.SetYesNo(ButtonState);
	}
}

SCSFExport scsf_RiskManager(SCStudyInterfaceRef sc)
{
	// Setup References
	SCInputRef Input_Enabled = sc.Input[0];
	SCInputRef Input_OrderType = sc.Input[1];
	SCInputRef Input_ControlSubgraphRef = sc.Input[2];
	SCInputRef Input_AdjustmentTiming = sc.Input[3]; 
	SCInputRef Input_AdjustmentFrequency = sc.Input[4];
	SCInputRef Input_TrailingOffset = sc.Input[5];
	SCInputRef Input_ACSButtonNumber = sc.Input[6];
	SCInputRef Input_PositionType = sc.Input[7];
	SCInputRef Input_DetailedLogging = sc.Input[8];
	SCInputRef Input_OnlyModifyInOneDirection = sc.Input[9];
	SCInputRef Input_Version = sc.Input[10];
	SCInputRef Input_AllowZeroValues = sc.Input[11];
	SCInputRef Input_AllowNegativeValues = sc.Input[12];
	SCInputRef Input_AllowCustomPropertiesForControlBarButton = sc.Input[13];
	SCInputRef Input_AttachedOrderToManage = sc.Input[14];
	SCInputRef Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable = sc.Input[15];
	SCInputRef Multiplier = sc.Input[16];
	SCInputRef DailyLossLimit = sc.Input[17];
	SCInputRef RiskPerTrade = sc.Input[18];
	SCInputRef RVWAP = sc.Input[19];
	SCInputRef std1 = sc.Input[20];
	SCInputRef std2 = sc.Input[21];
	SCInputRef std2_5 = sc.Input[22];
	SCInputRef std0_1 = sc.Input[23];
	SCInputRef std0_9 = sc.Input[24];
	SCInputRef VTrend = sc.Input[25];
	SCInputRef Scen1_Subgraph = sc.Input[26];
	SCInputRef Scen2_Subgraph = sc.Input[27];
	SCInputRef Scen3_Subgraph = sc.Input[28];
	SCInputRef Scen4_Subgraph = sc.Input[29];
	SCInputRef Scen5_Subgraph = sc.Input[30];
	SCInputRef Scen6_Subgraph = sc.Input[31];

	if (sc.SetDefaults)	
	{
		sc.GraphName = "ISB TSL Management";
		sc.AutoLoop = 1;//We are using Manual looping for efficiency since historical data is not relevant with this study
		sc.UpdateAlways = 1;
		sc.GraphRegion = 0;
		sc.CalculationPrecedence = VERY_LOW_PREC_LEVEL;
		
		Input_Enabled.Name = "Enabled";
		Input_Enabled.SetYesNo(0);

		Input_OrderType.Name = "Order Type To Manage";
		Input_OrderType.SetCustomInputStrings("Stops;Targets");
		Input_OrderType.SetCustomInputIndex(0);

		Input_ControlSubgraphRef.Name = "Controlling Subgraph Reference";
		Input_ControlSubgraphRef.SetStudySubgraphValues(0, 0);

		Input_AdjustmentTiming.Name = "Order Adjustment Timing";
		Input_AdjustmentTiming.SetCustomInputStrings("Every N Seconds;On Bar Close");
		Input_AdjustmentTiming.SetCustomInputIndex(1);

		Input_AdjustmentFrequency.Name = "Adjustment Frequency (in Seconds)";
		Input_AdjustmentFrequency.SetInt(5);

		Input_TrailingOffset.Name = "Trailing Offset (in Ticks)";
		Input_TrailingOffset.SetInt(2);

		Input_ACSButtonNumber.Name = "ACS Control Bar Button # for Enable/Disable";
		Input_ACSButtonNumber.SetInt(1);
		Input_ACSButtonNumber.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);

		Input_PositionType.  Name = "Position Type";
		Input_PositionType.SetCustomInputStrings("All Positions;Long Only;Short Only");
		Input_PositionType.SetCustomInputIndex(0);

		Input_DetailedLogging.Name = "Detailed Logging (for debugging)";
		Input_DetailedLogging.SetYesNo(0);

		Input_OnlyModifyInOneDirection.Name = "Only Modify Stops In One Direction";
		Input_OnlyModifyInOneDirection.SetYesNo(false);

		Input_Version.SetInt(2);

		Input_AllowZeroValues.Name = "Allow Zero Values";
		Input_AllowZeroValues.SetYesNo(false);

		Input_AllowNegativeValues.Name = "Allow Negative Values";
		Input_AllowNegativeValues.SetYesNo(false);

		Input_AllowCustomPropertiesForControlBarButton.Name = "Allow Custom 'Properties' for Control Bar Button";
		Input_AllowCustomPropertiesForControlBarButton.SetYesNo(false);

		Input_AttachedOrderToManage.Name = "Attached Order to Manage";
		Input_AttachedOrderToManage.SetCustomInputStrings("Nearest;OCO Group 1;OCO Group 2;OCO Group 3;OCO Group 4;OCO Group 5;OCO Group 6;OCO Group 7;OCO Group 8");
		Input_AttachedOrderToManage.SetCustomInputIndex(0);

		Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable.Name = "Change To Off All Other Custom Study Buttons On Enable";
		Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable.SetYesNo(false);

		// Do not receive mouse pointer events when ACS Tool Control Bar button is pressed in/enabled
		sc.ReceivePointerEvents = false;

		Multiplier.Name = "Chart Tick Value";
		Multiplier.SetFloat(0.0);
		DailyLossLimit.Name = "Daily Loss Limit ($)";
		DailyLossLimit.SetFloat(0.0);
		RiskPerTrade.Name = "Risk Per Trade MAX ($)";
		RiskPerTrade.SetFloat(0.0);
		RVWAP.Name = "RVWAP";
		RVWAP.SetStudySubgraphValues(0, 0);
		std1.Name = "RVWAP Top std1";
		std1.SetStudySubgraphValues(0, 0);
		std2.Name = "RVWAP Top std2";
		std2.SetStudySubgraphValues(0, 0);
		std2_5.Name = "RVWAP Top std2.5";
		std2_5.SetStudySubgraphValues(0, 0);
		std0_1.Name = "RVWAP Top std0.1";
		std0_1.SetStudySubgraphValues(0, 0);
		std0_9.Name = "RVWAP Top std0.9";
		std0_9.SetStudySubgraphValues(0, 0);
		VTrend.Name = "VTrend";
		VTrend.SetStudySubgraphValues(0, 0);
		Scen1_Subgraph.Name = "Subgraph for Scenario 1";
		Scen1_Subgraph.SetStudySubgraphValues(0, 0);
		Scen2_Subgraph.Name = "Subgraph for Scenario 2";
		Scen2_Subgraph.SetStudySubgraphValues(0, 0);
		Scen3_Subgraph.Name = "Subgraph for Scenario 3";
		Scen3_Subgraph.SetStudySubgraphValues(0, 0);
		Scen4_Subgraph.Name = "Subgraph for Scenario 4";
		Scen4_Subgraph.SetStudySubgraphValues(0, 0);
		Scen5_Subgraph.Name = "Subgraph for Scenario 5";
		Scen5_Subgraph.SetStudySubgraphValues(0, 0);
		Scen6_Subgraph.Name = "Subgraph for Scenario 6";
		Scen6_Subgraph.SetStudySubgraphValues(0, 0);

		return;
	}

	SCString logMessage;

	int& SL_State = sc.GetPersistentInt(1);
	int& TSL_State = sc.GetPersistentInt(2); 
	int& Scenario = sc.GetPersistentInt(3);
	// float& TriggerPrice = sc.GetPersistentFloat(1);

	//References to persistent variables
	SCDateTime& LastAdjustmentDateTime = sc.GetPersistentSCDateTime(1);
	int& MenuID = sc.GetPersistentInt(1);
	int & LastLogMessageIdentifier = sc.GetPersistentInt(2);

	HandlePointersEvents(LastLogMessageIdentifier, sc, Input_ACSButtonNumber, Input_Enabled, Input_ChangeToOffAllOtherCustomStudyButtonsOnEnable, Input_AllowCustomPropertiesForControlBarButton);

	// Get the actual price
	float actualPrice = sc.BaseData[SC_LAST][sc.Index];

	// Get Order Entry Price
	s_SCPositionData positionData;
	sc.GetTradePosition(positionData);
	float entryPrice = positionData.AveragePrice;
	int orderType = (positionData.PositionQuantity > 0) ? BUY_ORDER : 
				(positionData.PositionQuantity < 0) ? SELL_ORDER : 0;
	int orderQuantity = abs(positionData.PositionQuantity);
	float dLoss = positionData.DailyProfitLoss;

	stdPrice stdPrices;
	ACS_Button ACS_Buttons;
	set_stdsprice(stdPrices, std1, std2, std2_5, std0_1, std0_9, RVWAP, sc);

	if (!entryPrice)
	{
		sc.SetPersistentInt(1, 0);
		sc.SetPersistentInt(2, 0);
		sc.SetPersistentInt(3, 0);
		sc.SetPersistentFloat(1, 0.0);
		// logMessage.Format("I'M OUT MOTHERFUCKER    |    !EntryPrice");
		// sc.AddMessageToLog(logMessage, 1);
		return ;
	}

	if (Scenario && !TSL_State)
	{
		float TriggerPrice = Set_TSL_Trigger_Price(stdPrices, entryPrice, sc, Scenario);
		logMessage.Format("Scenario: %d   |   TriggerPrice: %f", Scenario, TriggerPrice);
		sc.AddMessageToLog(logMessage, 1);
		if (TriggerPrice && (orderType == BUY_ORDER && actualPrice >= TriggerPrice) ||  (orderType == SELL_ORDER && actualPrice <= TriggerPrice))
		{
			logMessage.Format("TriggerPrice Triggered: %f   |   ActualPrice: %f", TriggerPrice, actualPrice);
			sc.AddMessageToLog(logMessage, 1);
			Input_ControlSubgraphRef = VTrend;
			// TSL_State = true;
			sc.SetPersistentInt(2, 1);
			logMessage.Format("I'M OUT MOTHERFUCKER   |   Scenario && !TSL_State");
			sc.AddMessageToLog(logMessage, 1);
			return ;
		}	
	}

	logMessage.Format("Button state: %d | SL_State: %d | TSL_State: %d", sc.GetCustomStudyControlBarButtonEnableState(60), SL_State, TSL_State);
	sc.AddMessageToLog(logMessage, 1);
	if (entryPrice && !SL_State)
	{
		setStopLoss(caseFinder(entryPrice, orderType, stdPrices), sc, ACS_Buttons, Input_ControlSubgraphRef,
			Scen1_Subgraph, Scen2_Subgraph, Scen3_Subgraph, Scen4_Subgraph, Scen5_Subgraph, Scen6_Subgraph);
	}
	logMessage.Format("Button state: %d | SL_State: %d | TSL_State: %d", sc.GetCustomStudyControlBarButtonEnableState(60), SL_State, TSL_State);
	sc.AddMessageToLog(logMessage, 1);

	// Execute the order management

	// If study is not enabled, exit without doing anything
	if (Input_Enabled.GetYesNo() == 0)
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 1)
		{
			LastLogMessageIdentifier = 1;
			sc.AddMessageToLog("Study not enabled.", 0);
		}
		logMessage.Format("I'M OUT MOTHERFUCKER   |   Input_Enabled");
		sc.AddMessageToLog(logMessage, 1);
		return;
	}
	

	//For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 2)
		{
			LastLogMessageIdentifier = 2;
			sc.AddMessageToLog("No order adjustments performed because historical data is being downloaded for chart.", 0);
		}
		logMessage.Format("I'M OUT MOTHERFUCKER   |   HistoricalDownload");
		sc.AddMessageToLog(logMessage, 1);
		return;
	}

	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// Flat state is a good time to reset LastAdjustmentDateTime
	s_SCPositionData PositionData;
	sc.GetTradePosition (PositionData);

	if (PositionData.PositionQuantity == 0)
	{
		LastAdjustmentDateTime.Clear();

		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 3)
		{
			LastLogMessageIdentifier = 3;
			sc.AddMessageToLog("Position does not exist.", 0);
		}
		logMessage.Format("I'M OUT MOTHERFUCKER   |   FlatState");
		sc.AddMessageToLog(logMessage, 1);
		return;
	}

	if (Input_PositionType.GetIndex() == 1 && PositionData.PositionQuantity < 0)
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 4)
		{
			LastLogMessageIdentifier = 4;
			SCString  LogText("Position Type input is set to 'Long Only' and Position is Short. Position: ");
			SCString QuantityString;
			sc.OrderQuantityToString(PositionData.PositionQuantity, QuantityString);
			LogText += QuantityString;
			sc.AddMessageToLog(LogText, 0);
		}

		return;
	}

	if (Input_PositionType.GetIndex() == 2 && PositionData.PositionQuantity > 0)
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 5)
		{
			LastLogMessageIdentifier = 5;
			SCString  LogText("Position Type input is set to 'Short Only' and Position is Long. Position: ");
			SCString QuantityString;
			sc.OrderQuantityToString(PositionData.PositionQuantity, QuantityString);
			LogText += QuantityString;
			sc.AddMessageToLog(LogText, 0);
		}

		return;
	}

	// If the timing is every N seconds, check that we have surpassed the set frequency
	if (Input_AdjustmentTiming.GetInt() == 0 && (sc.LatestDateTimeForLastBar - LastAdjustmentDateTime) < SCDateTime::SECONDS_Fraction(Input_AdjustmentFrequency.GetFloat()))
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 6)
		{
			LastLogMessageIdentifier = 6;
			sc.AddMessageToLog("Time interval has not elapsed yet for 'Every N Seconds'.", 0);
		}
		return;		
	}


	//  Update the LastAdjustmentDateTime

	LastAdjustmentDateTime = sc.LatestDateTimeForLastBar;

	// Retrieve the controlling subgraph array
	SCFloatArray ControllingSubgraph;
	sc.GetStudyArrayUsingID(Input_ControlSubgraphRef.GetStudyID(), Input_ControlSubgraphRef.GetSubgraphIndex(), ControllingSubgraph);



	// Based on Input settings, retrieve nearest stop/target order
	s_SCTradeOrder ExistingTargetOrStopOrder;
	int Result = 0;

	if (Input_OrderType.IntValue == 0)
	{
		if (Input_AttachedOrderToManage.GetIndex() == 0)
		{
			Result = sc.GetNearestStopWorkingAttachedOrder(ExistingTargetOrStopOrder);
		}
		else
		{
			Result = sc.GetStopOrderInOCOGroupNumber(Input_AttachedOrderToManage.GetIndex(), ExistingTargetOrStopOrder);
		}
	}
	else
	{
		if (Input_AttachedOrderToManage.GetIndex() == 0)
		{
			Result = sc.GetNearestTargetWorkingAttachedOrder(ExistingTargetOrStopOrder);
		}
		else
		{
			Result = sc.GetTargetOrderInOCOGroupNumber(Input_AttachedOrderToManage.GetIndex(), ExistingTargetOrStopOrder);
		}
	}

	if (Result == SCTRADING_ORDER_ERROR)
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 7)
		{
			LastLogMessageIdentifier = 7;
			sc.AddMessageToLog("There is no Stop/Target Attached Order found.", 0);
		}
		return;
	}
	
	// only process open orders
	if (ExistingTargetOrStopOrder.OrderStatusCode != SCT_OSC_OPEN
		&& ExistingTargetOrStopOrder.OrderStatusCode != SCT_OSC_HELD)
	{	
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 8)
		{
			LastLogMessageIdentifier = 8;
			sc.AddMessageToLog("The found Stop/Target order is not in an Open/Held state.", 0);
		}
		return;
	}
	
	// Modify the order
	int BarIndex = sc.ArraySize - 1;
	
	const bool AdjustmentTimingIsOnBarClose = Input_AdjustmentTiming.GetIndex() == 1;
	if(AdjustmentTimingIsOnBarClose)//On Bar Close
	{
		BarIndex = sc.ArraySize - 2;
	}

	if (!Input_AllowZeroValues.GetYesNo() && ControllingSubgraph[BarIndex] == 0.0)
	{	
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 9)
		{
			LastLogMessageIdentifier = 9;
			if (AdjustmentTimingIsOnBarClose)
			{
				sc.AddMessageToLog("The 'Controlling Subgraph Reference' value is 0 at the bar prior to the last bar.", 0);
			}
			else
			{
				sc.AddMessageToLog("The 'Controlling Subgraph Reference' value is 0 at the last bar.", 0);
			}

		}
		return;
	}


	if (!Input_AllowNegativeValues.GetYesNo() && ControllingSubgraph[BarIndex] < 0)
	{	
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 15)
		{
			LastLogMessageIdentifier = 15;
			sc.AddMessageToLog("The 'Controlling Subgraph Reference' value is < 0 at the last bar.", 0);
		}
		return;
	}


	double NewPrice = 0.0;
	
	if (ExistingTargetOrStopOrder.BuySell == BSE_SELL)
	{
		NewPrice = ControllingSubgraph[BarIndex] - Input_TrailingOffset.GetInt() * sc.TickSize;
	}
	else if (ExistingTargetOrStopOrder.BuySell == BSE_BUY)
	{
		NewPrice = ControllingSubgraph[BarIndex] + Input_TrailingOffset.GetInt() * sc.TickSize;
	}
	else
		return;//Should never happen so no need for error logging.

	//Round the price to an actual order price.
	NewPrice = sc.RoundToTickSize(NewPrice);

	//If the price has not changed, then do not modify.
	if (sc.FormattedEvaluateUsingDoubles(ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat, EQUAL_OPERATOR, NewPrice, sc.BaseGraphValueFormat ) 
		|| sc.FormattedEvaluateUsingDoubles(ExistingTargetOrStopOrder.LastModifyPrice1, sc.BaseGraphValueFormat, EQUAL_OPERATOR, NewPrice, sc.BaseGraphValueFormat)
		)
	{	
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 10)
		{
			LastLogMessageIdentifier = 10;

			SCString LogMessage;
			LogMessage.Format("There is not a new price to modify the order to. Existing order: Price1=%f, LastModifyPrice1 =%f. New study subgraph price with offset=%f.", ExistingTargetOrStopOrder.Price1, ExistingTargetOrStopOrder.LastModifyPrice1, NewPrice);

			sc.AddMessageToLog(LogMessage, 0);
		}
		return;
	}

	// If modify stop price is in wrong direction, then do not modify.
	if (Input_OrderType.IntValue == 0 //Stops
		&&  Input_OnlyModifyInOneDirection.GetYesNo() &&
		((PositionData.PositionQuantity < 0 && sc.FormattedEvaluateUsingDoubles(NewPrice, sc.BaseGraphValueFormat, GREATER_OPERATOR, ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat)) ||
		(PositionData.PositionQuantity > 0 && sc.FormattedEvaluateUsingDoubles(NewPrice, sc.BaseGraphValueFormat, LESS_OPERATOR, ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat)) )	)
	{
		if (Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 14)
		{
			LastLogMessageIdentifier = 14;
			sc.AddMessageToLog("Modify stop price is in wrong direction.", 0);
		}
		return;
	}
	
	s_SCNewOrder ModifyOrder;
	ModifyOrder.InternalOrderID = ExistingTargetOrStopOrder.InternalOrderID;
	ModifyOrder.Price1 = NewPrice;

	Result = sc.ModifyOrder(ModifyOrder);

	if (Result == 1 && Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 11)
	{
		LastLogMessageIdentifier = 11;
		SCString  LogText;
		LogText.Format("The Stop/Target order has been modified. Internal Order ID: %d. New Price: %f", ExistingTargetOrStopOrder.InternalOrderID, NewPrice);

		sc.AddMessageToLog( LogText , 0);
	}
	else if (Result == SCT_SKIPPED_AUTO_TRADING_DISABLED && Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 13)
	{
		LastLogMessageIdentifier = 13;
		SCString  LogText;
		LogText.Format("'Trade >> Auto Trading Enabled' is disabled.");

		sc.AddMessageToLog( LogText , 0);
	}
	else if (Result != 1 && Input_DetailedLogging.GetYesNo() && LastLogMessageIdentifier != 12)
	{
		LastLogMessageIdentifier = 12;
		SCString  LogText;
		LogText.Format("There was an error modifying the Stop/Target order. Internal Order ID: %d. New Price: %f. Error Code %d.", ExistingTargetOrStopOrder.InternalOrderID, NewPrice, Result);

		sc.AddMessageToLog( LogText , 0);
	}
}
