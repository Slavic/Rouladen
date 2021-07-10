#include "pch.h"
#include "wglSwapBuffers.h"
#include "menu/menu.h"
#include "gl/gltext.h"
#include "offsets.h"
#include "cube.h"
#include "hacks/esp/esp.h"
#include "hacks/aimbot/aimbot.h"
#include "config.h"
#pragma comment(lib, "OpenGL32.lib")

twglSwapBuffers wglSwapBuffersGateWay;

static bool runOnce{ false };

BOOL __fastcall hkwglSwapBuffers(HDC hdc)
{
    if (!Global::bInitilized)
        return wglSwapBuffersGateWay(hdc);
    g_gameContext = wglGetCurrentContext();

    if (!g_contextCreated)
    {
        g_myContext = wglCreateContext(hdc);

        wglMakeCurrent(hdc, g_myContext);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);

        glOrtho(0.0, m_viewport[2], m_viewport[3], 0.0, 1.0, -1.0);  // Might want to make these your actual screen resolution
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
        g_contextCreated = true;
    }

    wglMakeCurrent(hdc, g_myContext);
   
    
    static ESP esp;
    static UI::Menu menu;
    static Aimbot aimbot;

    if (!runOnce)
    {
        menu.Initilize();
        // Patches AllowThirdPerson to return true
        mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1c63bf), reinterpret_cast<BYTE*>(const_cast<char*>("\x31\xD2\x90\x80\xFA\x00")), 6);
        // Patches inlined AllowThirdPerson calls
        mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x146176), reinterpret_cast<BYTE*>(const_cast<char*>("\x30\xC0")), 2);
        mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1c53f5), reinterpret_cast<BYTE*>(const_cast<char*>("\x30\xC0")), 2);
        runOnce = true;
    }
    
    // Placeholder input method, hardcoded and used for key inputs such as opening the menu and ejecting
    if (GetAsyncKeyState(VK_END) & 1)
        Global::bFinished = true;
    if (Global::cfg)
    {
        esp.Run();

        // Could be moved to updateweapons but it's not crucial
        static bool recoilSet{ false };
        if (Global::cfg->GetState("r_NoRecoil"))
        {
            mem::Nop(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1db76f), 18);
            recoilSet = true;
        }
        else if (recoilSet)
        {
            mem::Patch(reinterpret_cast<BYTE*>(Global::Offsets::moduleBase + 0x1db76f), 
                reinterpret_cast<BYTE*>(const_cast<char*>("\xF3\x41\x0F\x11\x4E\x0C\xF3\x41\x0F\x11\x56\x10\xF3\x41\x0F\x11\x5E\x14")), 18);
            recoilSet = false;
        }
        
        if (Global::cfg->GetState("r_ThirdPerson"))
            *Global::Offsets::thirdperson = 1;
        else
            *Global::Offsets::thirdperson = 0;
    }
    

    menu.Render();
   
   
    wglMakeCurrent(hdc, g_gameContext);


    return wglSwapBuffersGateWay(hdc);
}