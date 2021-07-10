#include "pch.h"
#include "shoot.h"
#include "hacks/aimbot/aimbot.h"
#include "hacks/misc/itempickup.h"
#include "offsets.h"
#include "config.h"
#include "mem.h"

tShoot Shoot;

void __fastcall hShoot(Cube::fpsent* e, vec3* target)
{
	static Itempickup items;
	static Aimbot aimbot;
	Vec3 oldPos;
	bool bRestorePos{ false };
	bool bSpreadDisabled{ false };

	if (Global::cfg && e == Global::Offsets::localPlayer && *Global::Offsets::intermission == false)
	{
		Vec3 targ;
		if (aimbot.Run(target, targ))
		{
			*target = targ;
			if (Global::cfg->GetState("r_MagicBullets"))
			{
				oldPos = Global::Offsets::localPlayer->o;
				Global::Offsets::localPlayer->o = (targ - Vec3(0.01f, 0.01f, 0.01f));
				bRestorePos = true;
			}
		}
	
		if (Global::cfg->GetState("r_NoSpread"))
		{
			mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1d8dcb), reinterpret_cast<BYTE*>(const_cast<char*>("\x24")), 1);
			Global::bNopRecoil = true;
			bSpreadDisabled = true;
		}
		
		items.Run();

	}

	Shoot(e, target);

	if (bRestorePos)
	{
		Global::Offsets::localPlayer->o = oldPos;
		bRestorePos = false;
	}

	if (bSpreadDisabled)
	{
		mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1d8dcb), reinterpret_cast<BYTE*>(const_cast<char*>("\x0C")), 1);
		Global::bNopRecoil = false;
		bSpreadDisabled = false;
	}

}
