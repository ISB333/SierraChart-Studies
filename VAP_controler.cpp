#include "sierrachart.h"

SCDLLName("ISB Studies")

SCSFExport scsf_VolumeAtPriceMultiplierControl(SCStudyInterfaceRef sc)
{
	SCInputRef Multiplier = sc.Input[0];
	SCInputRef Incrementation = sc.Input[1];
	SCInputRef Incrementation_Name = sc.Input[2];
	SCInputRef Decrementation = sc.Input[3];
	SCInputRef Decrementation_Name = sc.Input[4];
	SCString log_message;

	// Set up defaults and configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume at Price Multiplier Control";
		// Initialize input for multiplier
		Multiplier.Name = "Value to Increase/Decrease";
		Multiplier.SetInt(1); // Default multiplier
		Incrementation.Name = "ACS Button for Incrementation";
		Incrementation.SetInt(149); // Default multiplier
		Incrementation_Name.Name = "ACS Button Text for Incrementation";
		Incrementation_Name.SetString("+");
		Decrementation.Name = "ACS Button for Decrementation";
		Decrementation.SetInt(150); // Default multiplier
		Decrementation_Name.Name = "ACS Button Text for Decrementation";
		Decrementation_Name.SetString("-");

		return;
	}
	sc.SetStudyVisibilityState(sc.GetStudyIDByName(sc.ChartNumber, "Volume at Price Multiplier Control", 0), 0);
	sc.SetCustomStudyControlBarButtonText(Incrementation.GetInt(), Incrementation_Name.GetString());
	sc.SetCustomStudyControlBarButtonText(Decrementation.GetInt(), Decrementation_Name.GetString());

	int persistentMultiplier = sc.GetPersistentInt(0);
	if (persistentMultiplier == 0)
	{
		persistentMultiplier = sc.VolumeAtPriceMultiplier;
		sc.SetPersistentInt(0, persistentMultiplier);
	}
	else
	{
		sc.VolumeAtPriceMultiplier = persistentMultiplier;
	}
	// Capture button clicks
	if (sc.MenuEventID != 0) 
	{ 
		if (sc.MenuEventID == Incrementation.GetInt()) // Increase MultiplierQ
		{ 
			sc.SetCustomStudyControlBarButtonEnable(Incrementation.GetInt(), false);
			sc.SetPersistentInt(0, sc.VolumeAtPriceMultiplier + Multiplier.GetInt());
		}
		else if (sc.MenuEventID == Decrementation.GetInt()) // Decrease Multiplier
		{
			sc.SetCustomStudyControlBarButtonEnable(Decrementation.GetInt(), false);
			if (sc.VolumeAtPriceMultiplier - Multiplier.GetInt() > 0)
				sc.SetPersistentInt(0, sc.VolumeAtPriceMultiplier - Multiplier.GetInt());
		}
		sc.RecalculateChart(sc.ChartNumber);
	}
}
