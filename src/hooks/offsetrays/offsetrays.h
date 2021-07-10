#pragma once
#include "geom.h"
typedef void(__fastcall* tOffsetRays)(vec* from, vec* to, int spread, float range, vec* dest);

extern tOffsetRays OffsetRays;

extern void __fastcall hOffsetRays(vec* from, vec* to, int spread, float range, vec* dest);

