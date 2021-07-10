#pragma once
#include "pch.h"
#include "offsets.h"
#include "cube.h"

bool InRange(char x, char a, char b);

// No clue what this is
int32_t GetBits(char x);

int32_t GetBytes(const char* x);

bool IsTeamGameMode();

bool IsEnemy(Cube::fpsent* e);

bool IsValidModel(const char* mdl);

bool IsValidModel(const Cube::playermodelinfo* mdl);

bool IsEnemyModel(const char* mdl);

namespace Cube 
{
	typedef bool(__fastcall* tIntersect)(Cube::dynent* d, const vec* from, const vec* to, float* dist);
	extern tIntersect Intersect;
	
	typedef bool(__fastcall* tRayCubeLos)(vec* o, vec* dest, vec* hitpos);
	extern tRayCubeLos RayCubeLos;
	
}


