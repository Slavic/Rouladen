#include "pch.h"
#include "updateweapons.h"

#include "offsets.h"
#include "config.h"

tUpdateWeapons UpdateWeapons;

void __fastcall hUpdateWeapons(int curtime)
{
	static bool bHopSet{ false };
	if (Global::cfg)
	{
		
		if (Global::cfg->GetState("r_BunnyHop") && *Global::Offsets::intermission == false && Global::Offsets::localPlayer->state != 1)
		{
			Global::Offsets::localPlayer->jumping = true;
			bHopSet = true;
		}
		else if (bHopSet)
		{
			Global::Offsets::localPlayer->jumping = false;
			bHopSet = false;
		}
	}

	return UpdateWeapons(curtime);

	
}
