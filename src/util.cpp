#include "pch.h"
#include "util.h"
#include "config.h"


// InRange, GetBits, and GetBytes are only used by scan.cpp, which itself isn't used. Left for future refrence
bool InRange(char x, char a, char b)
{
    return x >= a && x <= b;
}

int32_t GetBits(char x)
{
    if (InRange(static_cast<char>(x & (~0x20)), 'A', 'F'))
    {
        return (x & (~0x20)) - 'A' + 0xa;
    }
    else if (InRange(x, '0', '9'))
    {
        return x - '0';
    }

    return 0;
}
// Converts a character hex "0F" to actual hex "\x0F"
int32_t GetBytes(const char* x)
{
    return GetBits(x[0]) << 4 | GetBits(x[1]);
}

bool IsTeamGameMode()
{
    return (Cube::gamemodes[*Global::Offsets::gameMode + 3].flags & Cube::M_TEAM);
}

bool IsEnemy(Cube::fpsent* e)
{
    if (Global::cfg && Global::cfg->GetState("r_IgnoreTeam"))
        return true;
    if ((IsTeamGameMode() && strcmp(e->team, Global::Offsets::localPlayer->team) == 0))
    {
        return false;
    }
    return true;
}

bool IsValidModel(const char* mdl)
{
    for (size_t i = 0; i < 5; ++i)
    {
        // strcmp returns 0 if the strings match, so !0 == true, hence !strcmp
        if (IsTeamGameMode())
        {
            if (!strcmp(mdl, Cube::playermodels[i].blueteam) || !strcmp(mdl, Cube::playermodels[i].redteam))
                return true;
        }
        else
            if (!strcmp(mdl, Cube::playermodels[i].ffa))
                return true;
    }
    return false;
}

bool IsValidModel(const Cube::playermodelinfo* mdl)
{
    return false;
}

bool IsEnemyModel(const char* mdl)
{
    if (IsTeamGameMode())
    {
        std::string name{ mdl };
        if (name.find("red"))
            return true;
    }
    return false;
}

Cube::tIntersect Cube::Intersect;

namespace Cube
{
    tRayCubeLos RayCubeLos;
}