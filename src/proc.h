#pragma once
#include "pch.h"

DWORD GetProcID(const wchar_t* procname);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

