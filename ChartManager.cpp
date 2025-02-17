#include "sierrachart.h"

SCDLLName("ISB Studies")

n_ACSIL::s_BarPeriod g_BarPeriod;

SCSFExport scsf_ChartManager(SCStudyInterfaceRef sc)
{
	SCInputRef	ATR_Subgraph = sc.Input[0];
	SCInputRef	scaleFactor = sc.Input[1];
	SCString log_message;

	// Set up defaults and configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Chart Manager";
		ATR_Subgraph.Name = "ATR";
		ATR_Subgraph.SetStudySubgraphValues(0, 0);
		scaleFactor.Name = "The Scaling Factor for BarPeriod & VAP";
		scaleFactor.SetInt(5); // Default scalingFactor 

		return;
	}
	sc.SetStudyVisibilityState(sc.GetStudyIDByName(sc.ChartNumber, "Chart Manager", 0), 0);

	SCFloatArray studySubgraph;
	sc.GetStudyArrayUsingID(ATR_Subgraph.GetStudyID(), ATR_Subgraph.GetSubgraphIndex(), studySubgraph);
	int	ATR_Value = studySubgraph[sc.Index];
	int scaleFactorValue = scaleFactor.GetInt();
	int	rounded_ATR_Value = ((ATR_Value + scaleFactorValue / 2) / scaleFactorValue) * scaleFactorValue;
	
	sc.GetBarPeriodParameters(g_BarPeriod);
	int barPeriod = g_BarPeriod.IntradayChartBarPeriodParameter1;
	
	int persistentATRValue = sc.GetPersistentInt(0);
	int persistentVAPMultiplier = sc.GetPersistentInt(1);
	if (!persistentVAPMultiplier)
	{
		persistentVAPMultiplier = sc.VolumeAtPriceMultiplier;
		sc.SetPersistentInt(1, persistentVAPMultiplier);
	}
	else
	{
		sc.VolumeAtPriceMultiplier = persistentVAPMultiplier;
	}
	if (persistentATRValue && persistentATRValue != barPeriod) {
		g_BarPeriod.IntradayChartBarPeriodParameter1 = rounded_ATR_Value;
		sc.SetBarPeriodParameters(g_BarPeriod);
	}
	else
	{
		sc.SetPersistentInt(0, rounded_ATR_Value);
		sc.SetPersistentInt(1, rounded_ATR_Value / scaleFactorValue);
	}
	// log_message.Format("ATR_Value: %d     Rounded: %d     BarPeriod: %d     VAP: %d", ATR_Value, rounded_ATR_Value, barPeriod, persistentVAPMultiplier);
	// sc.AddMessageToLog(log_message, 1);
}
