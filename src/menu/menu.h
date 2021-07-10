#pragma once
#include "pch.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

typedef int(__fastcall* tShowCursor)(int show);

extern tShowCursor SDL_ShowCursor;

typedef int(__fastcall* tSDL_SetRelativeMouseMode)(int enable);

extern tSDL_SetRelativeMouseMode SDL_SetRelativeMouseMode;

namespace Global
{
	
}

namespace UI
{
	class Console
	{
	private:
		char m_InputBuf[256];
		std::vector<char*> m_Items;
		bool m_AutoScroll;
		bool m_ScrollToBottom;
		std::filesystem::path m_Path;
	public:
		Console()
			: m_AutoScroll{ true }, m_ScrollToBottom{ false }, m_Path{}
		{
			ClearLog();
			memset(m_InputBuf, 0, sizeof(m_InputBuf));

		}
		~Console()
		{
			ClearLog();
		}

		void ClearLog()
		{
			for (int i = 0; i < m_Items.size(); ++i)
				delete m_Items[i];
			m_Items.clear();
		}
		bool SaveLog();

		void AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			char buf[1024];
			va_list args;
			va_start(args, fmt);
			vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
			buf[IM_ARRAYSIZE(buf) - 1] = '\0';
			va_end(args);
			char* onHeap{ new char[1024] };
			memcpy(onHeap, buf, sizeof(buf));
			m_Items.push_back(onHeap);
		}
		void Draw(const std::string& title);
	};

	class Menu
	{
	private:
		bool m_Focused;
		HWND m_hWnd;
		std::unordered_map<int64_t, std::string> m_KeyNames;

		void RenderToggle(const std::string& name, const std::string& id);
		const std::string& GetKeyName(int64_t index) const;

		Console m_DebugConsole;
	public:
		Menu();
		~Menu();
		void Initilize();
		void Render();

	};
}
namespace ImGui
{
	bool SmallButtonSized(const char* label, const ImVec2& size);
}
