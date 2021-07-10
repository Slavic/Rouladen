#pragma once
#include "cube.h"

// Global offsets file
namespace Global
{
	inline volatile bool bFinished;
	inline bool bInitilized;
	inline bool bDrawEsp;
	inline bool bRestoreDepth;
	inline bool bNopRecoil;

}

namespace Global::Offsets
{
	//Base
	inline uintptr_t moduleBase;

	// Globals
	inline Cube::fpsent* localPlayer;
	inline Cube::vector<Cube::fpsent*>* entList;
	inline Cube::vector<Cube::extentity*>* ents;
	inline float* matrix;
	inline Cube::guninfo* guns;
	inline Cube::MSlot* materialSlots;

	inline int* gameMode;
	inline int* testTeam;
	inline Cube::gamemodeinfo* gameModes;
	inline int* showBoundingBox;
	inline bool* grabInput;
	inline bool* shouldInput;
	inline bool* relativeMouse;
	inline bool* intermission;
	inline int* thirdperson;
	inline int* maxModelDistance;
	inline Cube::string* clientMap;
	inline int* fov;
}

