#include "RiskManager.h"

	/*
	If (Entry.Price > std1 && (Entry.Price - -(std0.1)) * Entry.Quantity < RPT.Max && Entry == BUY)
		Condition1 = -(std0.1);
	Else If (Entry.Price > std2 && (Entry.Price - std0.9) * Entry.Quantity < RPT.Max && Entry == BUY)
		Condition2 = std0.9;
	Else If (Entry.Price < -(std1) && (Entry.Price - std0.1) * Entry.Quantity < RPT.Max && Entry == SELL)
		Condition3 = std0.1;
	Else If (Entry.Price < -(std2) && (Entry.Price - -(std0.9)) * Entry.Quantity < RPT.Max && Entry == SELL)
		Condition4 = -(std0.9);
	Else If (Entry.Price < -(std1) && (Entry.Price - -(std2.5)) * Entry.Quantity < RPT.Max && Entry == BUY)
		Condition5 = -(std2.5);
	Else If (Entry.Price > std1 && (Entry.Price - std2.5) * Entry.Quantity < RPT.Max && Entry == SELL)
		Condition6 = std2.5;
	*/

int	caseFinder(float entryPrice, int orderType, stdPrice stdPrices) {
	if (entryPrice > stdPrices.RVWAP && orderType == BUY_ORDER)
		return (STD_N0_1);
	else if (entryPrice > stdPrices.std_2 && orderType == BUY_ORDER)
		return (STD_0_9);
	else if (entryPrice < stdPrices.RVWAP && orderType == SELL_ORDER)
		return (STD_0_1);
	else if (entryPrice < stdPrices.std_n2 && orderType == SELL_ORDER)
		return (STD_N0_9);
	else if (entryPrice < stdPrices.RVWAP && orderType == BUY_ORDER)
		return (STD_N2_5);
	else if (entryPrice > stdPrices.RVWAP && orderType == SELL_ORDER)
		return (STD_2_5);
	return (0);
}

void set_stdsprice(stdPrice& prices, SCInputRef& std1, SCInputRef& std2, SCInputRef& std2_5, SCInputRef& std0_1, SCInputRef& std0_9, SCInputRef& RVWAP, SCStudyInterfaceRef sc) {
	for (int i = 0; i < 6; ++i)
	{
		int studyID;
		switch (i)
		{
			case 0: studyID   = std1.GetStudyID(); break;
			case 1: studyID   = std2.GetStudyID(); break;
			case 2: studyID   = std2_5.GetStudyID(); break;
			case 3: studyID   = std0_1.GetStudyID(); break;
			case 4: studyID   = std0_9.GetStudyID(); break;
			case 5: studyID   = RVWAP.GetStudyID(); break;
		}
		int subgraphIndex;
		switch (i)
		{
			case 0: subgraphIndex   = std1.GetSubgraphIndex(); break;
			case 1: subgraphIndex   = std2.GetSubgraphIndex(); break;
			case 2: subgraphIndex   = std2_5.GetSubgraphIndex(); break;
			case 3: subgraphIndex   = std0_1.GetSubgraphIndex(); break;
			case 4: subgraphIndex   = std0_9.GetSubgraphIndex(); break;
			case 5: subgraphIndex   = RVWAP.GetSubgraphIndex(); break;
		}
		SCFloatArray studySubgraph;
		sc.GetStudyArrayUsingID(studyID, subgraphIndex, studySubgraph);
		switch (i)
		{
			case 0: prices.std_1   = studySubgraph[sc.Index]; break;
			case 1: prices.std_2   = studySubgraph[sc.Index]; break;
			case 2: prices.std_2_5 = studySubgraph[sc.Index]; break;
			case 3: prices.std_0_1 = studySubgraph[sc.Index]; break;
			case 4: prices.std_0_9 = studySubgraph[sc.Index]; break;
			case 5: prices.RVWAP   = studySubgraph[sc.Index]; break;
		}
		SCFloatArray studySubgraph2;
		sc.GetStudyArrayUsingID(studyID, subgraphIndex + 1, studySubgraph2);
		switch (i)
		{
			case 0: prices.std_n1   = studySubgraph2[sc.Index]; break;
			case 1: prices.std_n2   = studySubgraph2[sc.Index]; break;
			case 2: prices.std_n2_5 = studySubgraph2[sc.Index]; break;
			case 3: prices.std_n0_1 = studySubgraph2[sc.Index]; break;
			case 4: prices.std_n0_9 = studySubgraph2[sc.Index]; break;
		}
	}
}

// void set_ACS_Buttons(ACS_Button& buttons, SCInputRef& tsl_std2_5, SCInputRef& tsl_std0_1, SCInputRef& tsl_std0_9, SCInputRef& tsl_VTrend) {
// 	buttons.ACS_std_n0_1 = buttons.ACS_std_0_1 + 1;
// 	buttons.ACS_std_0_9 = tsl_std0_9.GetInt();
// 	buttons.ACS_std_0_1 = tsl_std0_1.GetInt();
// 	buttons.ACS_std_n0_9 = buttons.ACS_std_0_9 + 1;
// 	buttons.ACS_std_n2_5 = buttons.ACS_std_2_5 + 1;
// 	buttons.ACS_std_2_5 = tsl_std2_5.GetInt();
// 	buttons.VTrend = tsl_VTrend.GetInt();
// }

void	printMessage(const char *str, SCStudyInterfaceRef sc) {
	SCString logMessage;
	logMessage.Format(str);
	sc.AddMessageToLog(logMessage, 1);

}

void	setStopLoss(int scenario, SCStudyInterfaceRef sc, ACS_Button button, SCInputRef Input_ControlSubgraphRef,
	SCInputRef Scen1_Subgraph, SCInputRef Scen2_Subgraph, SCInputRef Scen3_Subgraph, SCInputRef Scen4_Subgraph, SCInputRef Scen5_Subgraph, SCInputRef Scen6_Subgraph) {
	switch (scenario) {
		case 1: Input_ControlSubgraphRef = Scen1_Subgraph; sc.SetPersistentInt(3, 1); sc.SetPersistentInt(1, 1); printMessage("Scenario 1", sc); break;
		case 2: Input_ControlSubgraphRef = Scen2_Subgraph; sc.SetPersistentInt(3, 2); sc.SetPersistentInt(1, 1); printMessage("Scenario 2", sc); break;
		case 3: Input_ControlSubgraphRef = Scen3_Subgraph; sc.SetPersistentInt(3, 3); sc.SetPersistentInt(1, 1); printMessage("Scenario 3", sc); break;
		case 4: Input_ControlSubgraphRef = Scen4_Subgraph; sc.SetPersistentInt(3, 4); sc.SetPersistentInt(1, 1); printMessage("Scenario 4", sc); break;
		case 5: Input_ControlSubgraphRef = Scen5_Subgraph; sc.SetPersistentInt(3, 5); sc.SetPersistentInt(1, 1); printMessage("Scenario 5", sc); break;
		case 6: Input_ControlSubgraphRef = Scen6_Subgraph; sc.SetPersistentInt(3, 6); sc.SetPersistentInt(1, 1); printMessage("Scenario 6", sc); break;
	}
}

float	Set_TSL_Trigger_Price(stdPrice stdPrices, float entryPrice, SCStudyInterfaceRef sc, int theScenario) {
	float TriggerPrice;
	switch (theScenario) {
		case 1: TriggerPrice = (stdPrices.std_2 - (stdPrices.std_2 - entryPrice) / 2); break;
		case 2:	TriggerPrice = (stdPrices.std_2_5); break;
		case 3: TriggerPrice = (stdPrices.std_n2 + (stdPrices.std_n2 - entryPrice) / 2); break;
		case 4: TriggerPrice = (stdPrices.std_n2_5); break;
		case 5: TriggerPrice = (stdPrices.std_2 - (stdPrices.std_2 - entryPrice) / 2); break;
		case 6: TriggerPrice = (stdPrices.std_n2 + (stdPrices.std_n2 - entryPrice) / 2); break;
	}
	return (TriggerPrice);
}
