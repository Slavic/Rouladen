// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "init.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
    ////Create console
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);
       
    Init init;
    init.MainLoop();

    

    //clean up & eject
    Sleep(50);
   /* if (f)
        fclose(f);
    FreeConsole();*/
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(HackThread), hModule, 0, nullptr));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

