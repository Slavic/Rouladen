#pragma once
#include "pch.h"
#include "cube.h"

typedef void(__fastcall* tRenderModel)(Cube::entitylight* light, char* mdl, int anim, vec* o, float yaw, float pitch, int flags, Cube::dynent* d, Cube::modelattach* a, int basetime, int basetime2, float trans);

extern tRenderModel RenderModel;

extern void __fastcall hRenderModel(Cube::entitylight* light, char* mdl, int anim, vec* o, float yaw, float pitch, int flags, Cube::dynent* d, Cube::modelattach* a, int basetime, int basetime2, float trans);


typedef void(__fastcall* tAnimRender)(Cube::model* _this, int anim, int basetime, int basetime2, vec* o, float yaw, float pitch, Cube::dynent* d, Cube::modelattach* a, vec* color, vec* dir, float transparent);

extern tAnimRender AnimRender;

extern void __fastcall hAnimRender(Cube::model* _this, int anim, int basetime, int basetime2, vec* o, float yaw, float pitch, Cube::dynent* d, Cube::modelattach* a, vec* color, vec* dir, float transparent);

typedef void(__fastcall* tEndRender)(Cube::model* _this);

extern tEndRender EndRender;

extern void __fastcall hEndRender(Cube::model* _this);


