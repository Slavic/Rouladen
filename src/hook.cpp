#include "pch.h"
#include "hook.h"
#include "mem.h"
#include "console.h"


BYTE* TrampHook64(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5)
		return nullptr;

	BYTE* relayAddr{ AllocatePageNearAddr(src) };

	if (relayAddr == nullptr)
		return nullptr;

	WriteAbsJmp64(relayAddr, dst);

	DWORD oldProt;

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProt);

	memcpy_s(reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(relayAddr) + 13u), len, src, len);

	memset(src, 0x90, len);

	BYTE jmpInstruction[5]{ 0xE9, 0x0, 0x0, 0x0, 0x0 };

	uintptr_t relAddr{ reinterpret_cast<uintptr_t>(src) - reinterpret_cast<uintptr_t>(relayAddr) - sizeof(jmpInstruction) - 13u };

	memcpy(&jmpInstruction[1], &relAddr, 4);
	BYTE* gatewayAddr{ reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(relayAddr) + 13u) };

	memcpy(reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(relayAddr) + len + 13u), jmpInstruction, sizeof(jmpInstruction));

	relAddr = reinterpret_cast<uintptr_t>(relayAddr) - reinterpret_cast<uintptr_t>(src) - sizeof(jmpInstruction);
	memcpy(&jmpInstruction[1], &relAddr, 4);

	memcpy(src, jmpInstruction, sizeof(jmpInstruction));

	return gatewayAddr;
}

BYTE* AllocatePageNearAddr(BYTE* addr)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	const uintptr_t PAGE_SIZE{ sysInfo.dwPageSize };

	uintptr_t startAddr{ reinterpret_cast<uintptr_t>(addr) & ~(PAGE_SIZE - 1) };
	uintptr_t minAddr{ std::min(startAddr - 0x7FFFFF00, reinterpret_cast<uintptr_t>(sysInfo.lpMinimumApplicationAddress)) };
	uintptr_t maxAddr{ std::max(startAddr + 0x7FFFFF00, reinterpret_cast<uintptr_t>(sysInfo.lpMaximumApplicationAddress)) };

	uintptr_t startPage{ startAddr - (startAddr % PAGE_SIZE) };

	uint64_t pageOffset{ 1 };

	while (true)
	{
		uintptr_t byteOffset{ pageOffset * PAGE_SIZE };
		uintptr_t highAddr{ startPage + byteOffset };
		uintptr_t lowAddr{ (startPage > byteOffset) ? startPage - byteOffset : 0 };

		bool needsExit{ highAddr > maxAddr && lowAddr < minAddr };

		if (highAddr < maxAddr)
		{
			BYTE* outAddr{ reinterpret_cast<BYTE*>(VirtualAlloc(reinterpret_cast<void*>(highAddr), PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) };
			if (outAddr != nullptr)
				return outAddr;
		}

		if (lowAddr > minAddr)
		{
			BYTE* outAddr{ reinterpret_cast<BYTE*>(VirtualAlloc(reinterpret_cast<void*>(lowAddr), PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) };
			if (outAddr != nullptr)
				return outAddr;
		}

		++pageOffset;

		if (needsExit)
			break;
	}
	return nullptr;

}

void WriteAbsJmp64(BYTE* absJmpMem, BYTE* jmpAddr)
{
	BYTE absJmpInstructions[]
	{
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov rax, addr
		0xFF, 0xE0, 0x90 //jmp rax
	};

	uintptr_t jmpAddr64{ reinterpret_cast<uintptr_t>(jmpAddr) };

	memcpy(&absJmpInstructions[2], &jmpAddr64, sizeof(jmpAddr64));
	memcpy(absJmpMem, absJmpInstructions, sizeof(absJmpInstructions));
}


Hook::Hook(BYTE* src, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len) 
	: m_src{ src }, m_dst{ dst }, m_ptrToGateWayFnPtr{ ptrToGateWayPtr }, m_len{ len }
{
}

Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len) 
	: m_dst { dst }, m_ptrToGateWayFnPtr{ ptrToGateWayPtr }, m_len{ len }
{
	HMODULE hMod{ GetModuleHandleA(modName) };
	if (hMod)
		m_src = reinterpret_cast<BYTE*>(GetProcAddress(hMod, exportName));
}

Hook::Hook(const char* exportName, const wchar_t* modName, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len)
	: m_dst{ dst }, m_ptrToGateWayFnPtr{ ptrToGateWayPtr }, m_len{ len }
{
	HMODULE hMod{ GetModuleHandle(modName) };
	if (hMod)
		m_src = reinterpret_cast<BYTE*>(GetProcAddress(hMod, exportName));
}

Hook::~Hook()
{
    if (m_bStatus)
        Disable();
}

void Hook::Enable()
{
	memcpy(m_originalBytes, m_src, m_len);
	*reinterpret_cast<uintptr_t*>(m_ptrToGateWayFnPtr) = reinterpret_cast<uintptr_t>(TrampHook64(m_src, m_dst, m_len));
	m_bStatus = true;
	// Logging
	std::stringstream msgFormat{};
	msgFormat << "Hooked 0x"  << std::hex << reinterpret_cast<void*>(m_src) << std::dec <<  " with a length of " << m_len;
	msgFormat << "\nBytes: ";
	for (int i = 0; i < m_len; ++i)
		msgFormat << "0x" << std::hex << static_cast<unsigned int>(m_originalBytes[i]) << std::dec << ' ';

	Global::consoleQueue.emplace_back(msgFormat.str());
}

void Hook::Disable()
{
	mem::Patch(m_src, m_originalBytes, m_len);
	m_bStatus = false;
}

void Hook::Toggle()
{
	if (!m_bStatus)
		Enable();
	else
		Disable();
}
