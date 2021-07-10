#pragma once
#include "pch.h"
static bool g_contextCreated{ false };
inline HGLRC g_myContext;
inline HGLRC g_gameContext;


typedef BOOL(__fastcall* twglSwapBuffers) (HDC hdc);

extern twglSwapBuffers wglSwapBuffersGateWay;

extern BOOL __fastcall hkwglSwapBuffers(HDC hdc);

