#pragma once
#include "pch.h"
#include "cube.h"

namespace Cube
{
	typedef void(__fastcall* tTryPickup)(int index, Cube::fpsent* e);

	extern tTryPickup TryPickup;

	typedef int(__fastcall* tGetClockMillis)();

	extern tGetClockMillis GetClockMillis;
}

struct ItemTracker
{
	int cooldown{ };
	int index{ 0 };
	Cube::extentity* item{ nullptr };

	ItemTracker(int time, int i, Cube::extentity* e)
		: cooldown{ time }, index{ i }, item{ e }
	{}
};

class Itempickup
{
	std::vector<ItemTracker> m_Items;
	std::string m_CurrentMap;

	std::mt19937 m_Gen;
	std::uniform_int_distribution<int> m_Rand;

	bool m_Built;
public:
	Itempickup();
	bool Build();
	void Run();
};

