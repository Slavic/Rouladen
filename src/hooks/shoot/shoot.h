#pragma once
#include "cube.h"

typedef void(__fastcall* tShoot)(Cube::fpsent* e, vec3* target);
extern tShoot Shoot;

extern void __fastcall hShoot(Cube::fpsent* e, vec3* target);

