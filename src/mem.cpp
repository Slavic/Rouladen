#include "pch.h"
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);

}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t address{ ptr };
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		
		ReadProcessMemory(hProc, reinterpret_cast<BYTE*>(address), &address, sizeof(address), 0);
		address += offsets[i];
	}
	return address;
}

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);

}

void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

uintptr_t mem::FindDMAAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t address{ ptr };
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		address = *(reinterpret_cast<uintptr_t*>(address));
		address += offsets[i];
	}
	return address;
}
