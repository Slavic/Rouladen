#pragma once
#include "cube.h"

typedef void(__fastcall* tRenderPlayer)(Cube::fpsent* d, Cube::playermodelinfo* mdl, int team, float fade, bool mainpass);

extern tRenderPlayer RenderPlayer;

extern void __fastcall hRenderPlayer(Cube::fpsent* d, Cube::playermodelinfo* mdl, int team, float fade, bool mainpass);

