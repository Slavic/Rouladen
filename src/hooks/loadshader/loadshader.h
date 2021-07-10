#pragma once

#include "cube.h"

typedef Cube::Shader* (__fastcall* tLoadShader)(Cube::animmodel::skin* _this);

extern tLoadShader LoadShader;

extern Cube::Shader* __fastcall hLoadShader(Cube::animmodel::skin* _this);
