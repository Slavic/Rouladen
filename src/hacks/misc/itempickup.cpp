#include "pch.h"
#include "itempickup.h"
#include "offsets.h"
#include "config.h"
#include "console.h"

namespace Cube
{
	tTryPickup TryPickup;

	tGetClockMillis GetClockMillis;
}

Itempickup::Itempickup() 
	: m_Items{}, m_Built{ false }, m_CurrentMap{}, m_Rand{ 0, 200 }
{
	std::random_device rd;
	m_Gen.seed(rd());
}

bool Itempickup::Build()
{
	if (!Global::Offsets::ents)
		return false;

	m_Items.clear();
	int numOfEnts{ Global::Offsets::ents->length() };
	for (int i = 0; i < numOfEnts - 1; ++i)
	{
		if (!Global::Offsets::ents)
			return false;
		Cube::extentity* item{ Global::Offsets::ents->buf[i] };
		if (!item)
			continue;

		// Check if item is consumable
		if (item->type != Cube::NOTUSED && item->type != Cube::TELEPORT && item->type != Cube::JUMPPAD && item->type != Cube::RESPAWNPOINT)
		{
			// Check if consumable is valid consumable
			if (item->type < Cube::I_SHELLS || item->type > Cube::I_QUAD)
				continue;
			
			m_Items.emplace_back(Cube::GetClockMillis(), i, item);
		}
	}
	m_Built = true;
	m_CurrentMap = *Global::Offsets::clientMap;
	std::stringstream format{};
	format << "Built extentity list of size " << m_Items.size() << " for " << m_CurrentMap;
	
	Global::consoleQueue.emplace_back(format.str());
	return true;
}

void Itempickup::Run()
{
	if (!Global::cfg || Global::Offsets::localPlayer->state != 0 || !*Global::Offsets::clientMap[0])
		return;
	// We flip the clientnum check to remain consistant with Sauerbraten
	if (*Global::Offsets::intermission || !(Global::Offsets::localPlayer->clientnum >= 0))
	{
		m_Built = false;
		return;
	}
	else if (m_CurrentMap.compare(*Global::Offsets::clientMap))
	{
		m_Built = false;
	}

	if (!m_Built || !Global::Offsets::ents)
		if (!Build())
			return;

	static int loop{ 0 };
	if (Global::cfg->GetState("r_ItemPickUp"))
	{
		for (int i = 0; i < m_Items.size(); ++i)
		{
			ItemTracker& item{ m_Items[i] };
			if (!item.item)
			{
				m_Built = false;
				return;
			}
			// m_Rand returns (0, 200), so -100 alters it to (-100, 100)
			if (Cube::GetClockMillis() - item.cooldown > 1000 + (m_Rand(m_Gen) - 100))
			{
				Cube::TryPickup(item.index, Global::Offsets::localPlayer);
				item.cooldown = Cube::GetClockMillis();
				break;
			}
		}
	}
}
