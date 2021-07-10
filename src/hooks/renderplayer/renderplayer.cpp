#include "pch.h"
#include "renderplayer.h"
#include "offsets.h"
#include "gl/gldraw.h"
#include "hacks/esp/esp.h"
#include "util.h"


tRenderPlayer RenderPlayer;

void __fastcall hRenderPlayer(Cube::fpsent* d, Cube::playermodelinfo* mdl, int team, float fade, bool mainpass)
{	
	
	if (d && d->state != 1 && d != Global::Offsets::localPlayer && !Global::drawinfo.IsDuplicate(d)) // CS_DEAD
	{
		// Redundent method of getting player information for rendermodel hook, unused but left in here as it can be usefull
		std::string name;
		if (IsTeamGameMode())
		{
			if (!strcmp(d->team, "evil"))
				name = mdl->blueteam;
			else
				name = mdl->redteam;
		}
		else
			name = mdl->ffa;
			
		Global::drawinfo.draw = true;
		Global::drawinfo.players.emplace_back(d, name);
		
	}
	return RenderPlayer(d, mdl, team, fade, mainpass);
}
