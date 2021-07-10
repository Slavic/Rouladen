#pragma once
#include "pch.h"
#include "mem.h"
#include "console.h"

// Trampoline Hook, based on Kyle Halladay's implementation http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
BYTE* TrampHook64(BYTE* src, BYTE* dst, const uintptr_t len);

BYTE* AllocatePageNearAddr(BYTE* addr);

// Write Absolute Jump x64
void WriteAbsJmp64(BYTE* absJmpMem, BYTE* jmpAddr);

// Class that manages a single trampoline hook
class Hook
{
private:
	bool  m_bStatus{ false };

	BYTE* m_src{ nullptr };
	BYTE* m_dst{ nullptr };
	BYTE* m_ptrToGateWayFnPtr{ nullptr };
	uintptr_t m_len{ 0 };

	BYTE m_originalBytes[10]{ 0 };

public:
	Hook(BYTE* src, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len);
	Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len);
	Hook(const char* exportName, const wchar_t* modName, BYTE* dst, BYTE* ptrToGateWayPtr, uintptr_t len);

    ~Hook();

	void Enable();
	void Disable();
	void Toggle();
};

// Changes a single pointer within a objects vmt table
class VMTHook
{
public:
    VMTHook()
        : m_pBase{ nullptr }, m_uIndex{ 0 }, m_pOldFunc{ nullptr }, m_bSetup{ false } {}

    template<typename T>
    VMTHook(void* pBase, size_t uIndex, T fnHook) 
        : m_pBase{ pBase }, m_uIndex{ uIndex }, m_pOldFunc{ nullptr }, m_bSetup{ false } 
    {
       // Setup must return true before inserting function, redundent but safe
        if (Setup(pBase))
            InsertFunctionHook(m_uIndex, fnHook);

    }


    ~VMTHook() { RemoveHooks(); }

    // Sets up member variables, kinda useless but left incase more complicated setup is needed
    bool Setup(void* pBase)
    {
        if (pBase != nullptr) m_pBase = pBase;
        if (m_pBase == nullptr) return false;

        m_bSetup = true;
        return true;
    }

    const bool GetSetup() const { return m_bSetup; }

    // Swaps pointer in VMT table for our own pointer, stores orignal
    template<typename T>
    void InsertFunctionHook(size_t uIndex, T fnHook)
    {
        if (!m_bSetup)
            return;
        m_uIndex = uIndex;
        // Get VMT pointer through derefrencing
        void** pVMT{ *static_cast<void***>(m_pBase) };
        try
        {
            // Change protections of desired index within the VMT table
            DWORD guard;
            VirtualProtect(&pVMT[m_uIndex], sizeof(void*), PAGE_READWRITE, &guard);
            // Store and overwrite function pointer
            m_pOldFunc = pVMT[m_uIndex];
            pVMT[m_uIndex] = static_cast<void*>(fnHook);
            VirtualProtect(&pVMT[m_uIndex], sizeof(void*), guard, &guard);
        }
        catch (std::exception& e)
        {
            std::string temp{ "[Error] " };
            temp.append(e.what());
            Global::consoleQueue.emplace_back(temp);
        }
    }
    // Restores original function pointer
    void RemoveHooks()
    {
        if (!m_bSetup)
            return;
        void** pVMT{ *static_cast<void***>(m_pBase) };
        try
        {

            DWORD guard;
            VirtualProtect(&pVMT[m_uIndex], sizeof(void*), PAGE_READWRITE, &guard);
            pVMT[m_uIndex] = m_pOldFunc;
            VirtualProtect(&pVMT[m_uIndex], sizeof(void*), guard, &guard);
        }
        catch (std::exception& e)
        {
            std::string temp{ "[Error] " };
            temp.append(e.what());
            Global::consoleQueue.emplace_back(temp);
        }
    }
    // Returns original function pointer casted into a function typedef
    template<typename T>
    T GetOriginal()
    {
        return static_cast<T>(m_pOldFunc);
    }

private:
    void* m_pBase;
    size_t m_uIndex;
    void* m_pOldFunc;
    bool m_bSetup;
};
