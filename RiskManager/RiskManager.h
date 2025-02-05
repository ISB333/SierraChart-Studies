#pragma once

# include "sierrachart.h"

/*
	TODO : 
	- Display :
		1. Actual Trade Risk (%)
		2. P&L (%) of dLL
		3. If Possible (RVWAP Market State) 
	- If (2Losses in a ROW) "Take a break of few minutes to reflect" && block for 3min
*/

# define STD_N0_1 1
# define STD_0_9 2
# define STD_0_1 3
# define STD_N0_9 4
# define STD_N2_5 5
# define STD_2_5 6

# define BUY_ORDER 100
# define SELL_ORDER 101

# define SL_SET 102

struct	stdPrice {
	float RVWAP;
	float std_1;
	float std_n1;
	float std_2;
	float std_n2;
	float std_2_5;
	float std_n2_5;
	float std_0_1;
	float std_n0_1;
	float std_0_9;
	float std_n0_9;
};

struct	ACS_Button {
	int ACS_std_n0_1;
	int ACS_std_0_9;
	int ACS_std_0_1;
	int ACS_std_n0_9;
	int ACS_std_n2_5;
	int ACS_std_2_5;
	int VTrend;
};

extern int SL_State;
extern int TSL_State;
extern int Scenario;

float	Set_TSL_Trigger_Price(stdPrice stdPrices, float entryPrice, SCStudyInterfaceRef sc, int theScenario);
void	setStopLoss(int scenario, SCStudyInterfaceRef sc, ACS_Button button, SCInputRef Input_ControlSubgraphRef,
	SCInputRef Scen1_Subgraph, SCInputRef Scen2_Subgraph, SCInputRef Scen3_Subgraph, SCInputRef Scen4_Subgraph, SCInputRef Scen5_Subgraph, SCInputRef Scen6_Subgraph);
void	printMessage(const char *str, SCStudyInterfaceRef sc);
void    set_ACS_Buttons(ACS_Button& buttons, SCInputRef& tsl_std2_5, SCInputRef& tsl_std0_1, SCInputRef& tsl_std0_9, SCInputRef& tsl_VTrend);
void    set_stdsprice(stdPrice& prices, SCInputRef& std1, SCInputRef& std2, SCInputRef& std2_5, SCInputRef& std0_1, SCInputRef& std0_9, SCInputRef& RVWAP, SCStudyInterfaceRef sc);
int     caseFinder(float entryPrice, int orderType, stdPrice stdPrices);