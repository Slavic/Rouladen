#include "pch.h"
#include "init.h"
#include "hook.h"
#include "hooks/hooks.h"
#include "gl/gltext.h"
#include "gl/gldraw.h"
#include "menu/menu.h"
#include "hacks/esp/esp.h"
#include "hacks/misc/itempickup.h"
#include "util.h"
#include "config.h"

Init::Init()
{
	// Pointers
	Global::Offsets::moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
	HMODULE SDL_Mod{ GetModuleHandleA("SDL2.dll") };
	
	Global::Offsets::localPlayer = *reinterpret_cast<Cube::fpsent**>(Global::Offsets::moduleBase + 0x2a5730);
	Global::Offsets::entList = reinterpret_cast<Cube::vector<Cube::fpsent*>*>(Global::Offsets::moduleBase + 0x346c90);
	Global::Offsets::ents = reinterpret_cast<Cube::vector<Cube::extentity*>*>(Global::Offsets::moduleBase + 0x346c80);
	Global::Offsets::guns = reinterpret_cast<Cube::guninfo*>(Global::Offsets::moduleBase + 0x21d070);
	Global::Offsets::materialSlots = reinterpret_cast<Cube::MSlot*>(Global::Offsets::moduleBase + 0x33f010);

	Global::Offsets::matrix =  reinterpret_cast<float*>(Global::Offsets::moduleBase + 0x32d040);
	Global::Offsets::showBoundingBox = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x32e3c4);
	Global::Offsets::gameMode = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x26f6b0);
	Global::Offsets::testTeam = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x346de0);
	Global::Offsets::gameModes = *reinterpret_cast<Cube::gamemodeinfo**>(Global::Offsets::moduleBase + 0x26f830);
	Global::Offsets::grabInput = reinterpret_cast<bool*>(Global::Offsets::moduleBase + 0x2a3057);
	Global::Offsets::shouldInput = reinterpret_cast<bool*>(Global::Offsets::moduleBase + 0x02a32e4);
	Global::Offsets::relativeMouse = reinterpret_cast<bool*>(Global::Offsets::moduleBase + 0x2a3056);
	Global::Offsets::intermission = reinterpret_cast<bool*>(Global::Offsets::moduleBase + 0x2a353f);
	Global::Offsets::thirdperson = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x32cfa8);
	Global::Offsets::maxModelDistance = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x32e3c8);
	Global::Offsets::clientMap = reinterpret_cast<Cube::string*>(Global::Offsets::moduleBase + 0x02a58c0);
	Global::Offsets::fov = reinterpret_cast<int*>(Global::Offsets::moduleBase + 0x32bf34);

	// Functions
	
	// OpenGL
	Cube::GL::draw_text = reinterpret_cast<Cube::GL::tdraw_text>(Global::Offsets::moduleBase + 0x15fb80);
	Cube::GL::draw_textf = reinterpret_cast<Cube::GL::tdraw_textf>(Global::Offsets::moduleBase + 0x15f2b0);

	Cube::GL::Render2dBox = reinterpret_cast<Cube::GL::trender2dbox>(Global::Offsets::moduleBase + 0x14d1b0);
	Cube::GL::Render3dBox = reinterpret_cast<Cube::GL::trender3dbox>(Global::Offsets::moduleBase + 0x14d030);

	// For Esp
	Cube::LoadModel = reinterpret_cast<Cube::tLoadModel>(Global::Offsets::moduleBase + 0x151760);
	Cube::GetPlayerModelInfo = reinterpret_cast<Cube::tGetPlayerModelInfo>(Global::Offsets::moduleBase + 0x1cb860);
	Cube::RotateBB = reinterpret_cast<Cube::tRotateBB>(Global::Offsets::moduleBase + 0x112cb0);
	Cube::LookUpShader = reinterpret_cast<Cube::tLookUpShader>(Global::Offsets::moduleBase + 0x1707c0);
	Cube::SetShader = reinterpret_cast<Cube::tSetShader>(Global::Offsets::moduleBase + 0x27200);
	Cube::vColor = reinterpret_cast<Cube::tVColor>(Global::Offsets::moduleBase + 0xf3060);
	Cube::LookUpMaterials = reinterpret_cast<Cube::tLookUpMaterials>(Global::Offsets::moduleBase + 0x183dd0);
	Cube::LookUpVSlot = reinterpret_cast<Cube::tLookUpVSlot>(Global::Offsets::moduleBase + 0x186890);

	// Aimbot
	Cube::RayCubeLos = reinterpret_cast<Cube::tRayCubeLos>(Global::Offsets::moduleBase + 0x1140e0);

	Cube::Intersect = reinterpret_cast<Cube::tIntersect>(Global::Offsets::moduleBase + 0x1dadb0);


	// Menu
	if (SDL_Mod != 0)
	{
		SDL_ShowCursor = reinterpret_cast<tShowCursor>(GetProcAddress(SDL_Mod, "SDL_ShowCursor"));
		SDL_SetRelativeMouseMode = reinterpret_cast<tSDL_SetRelativeMouseMode>(GetProcAddress(SDL_Mod, "SDL_SetRelativeMouseMode"));
	}
	else
		throw std::exception("SDL_Mod was 0, SDL functions could not be found");

	// Misc
	Cube::TryPickup = reinterpret_cast<Cube::tTryPickup>(Global::Offsets::moduleBase + 0x1bfd50);
	Cube::GetClockMillis = reinterpret_cast<Cube::tGetClockMillis>(Global::Offsets::moduleBase + 0x1a2e20);
	

	Global::bDrawEsp = false;
	Global::bRestoreDepth = false;
	Global::bNopRecoil = false;
	Global::bInitilized = true;
}

void Init::MainLoop()
{
	Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", reinterpret_cast<BYTE*>(hkwglSwapBuffers), reinterpret_cast<BYTE*>(&wglSwapBuffersGateWay), 5);
	Hook RenderPlayerHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1cb140), reinterpret_cast<BYTE*>(hRenderPlayer), reinterpret_cast<BYTE*>(&RenderPlayer), 5);
	Hook RenderModelHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x150990), reinterpret_cast<BYTE*>(hRenderModel), reinterpret_cast<BYTE*>(&RenderModel), 7);
	Hook AnimRenderHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x0507d0), reinterpret_cast<BYTE*>(hAnimRender), reinterpret_cast<BYTE*>(&AnimRender), 7);
	Hook UpdateWeaponsHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1dae30), reinterpret_cast<BYTE*>(hUpdateWeapons), reinterpret_cast<BYTE*>(&UpdateWeapons), 5);

	Hook ShootHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1db4c0), reinterpret_cast<BYTE*>(hShoot), reinterpret_cast<BYTE*>(&Shoot), 5);
	Hook OffsetRaysHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1d90b0), reinterpret_cast<BYTE*>(hOffsetRays), reinterpret_cast<BYTE*>(&OffsetRays), 7);
	Hook LoadShaderHook(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x4db60), reinterpret_cast<BYTE*>(hLoadShader), reinterpret_cast<BYTE*>(&LoadShader), 9);

	LoadConfig();

	SwapBuffersHook.Enable();
	UpdateWeaponsHook.Enable();
	ShootHook.Enable();
	OffsetRaysHook.Enable();
	RenderPlayerHook.Enable();
	RenderModelHook.Enable();
	AnimRenderHook.Enable();
	LoadShaderHook.Enable();

	 
	while (!Global::bFinished)
	{
		// Compiler optimizations have messed up this code in the past, hence why it's doubled
		if (Global::bFinished)
			break;
	}

	// Techically Disable() will be called in the destructor, however unhooking in the same order as you hooked is prefered
	LoadShaderHook.Disable();
	AnimRenderHook.Disable();
	RenderModelHook.Disable();
	RenderPlayerHook.Disable();
	OffsetRaysHook.Disable();
	ShootHook.Disable();
	UpdateWeaponsHook.Disable();
	SwapBuffersHook.Disable();
	
	SaveConfig();
}
