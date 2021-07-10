#include "pch.h"
#include "offsetrays.h"
#include "offsets.h"

tOffsetRays OffsetRays;

void __fastcall hOffsetRays(vec* from, vec* to, int spread, float range, vec* dest)
{
	if (Global::bNopRecoil)
	{
		spread = 0;
	}
	return OffsetRays(from, to, spread, range, dest);
}
