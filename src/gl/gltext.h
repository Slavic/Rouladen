#pragma once
#pragma comment(lib, "OpenGL32.lib")

#include "pch.h"
#include "mem.h"
#include "geom.h"



namespace GL
{
	class Font
	{
	private:
		bool m_bBuilt{ false };
		unsigned int m_base{ 0 };
		HDC m_hdc{ nullptr };
		int m_height{ 0 };
		int m_width{ 0 };
	public:
		void Build(int height);
		void Print(float x, float y, const unsigned char color[3], const char* format, ...);

		vec3 CenterText(float x, float y, float width, float height, float textWidth, float textHeight);
		float CenterText(float x, float width, float textWidth);

		bool GetBuildStatus();
		HDC GetHDC();

	};

	void RenderText(const char* str, int left, int top, int r = 255, int g = 255, int b = 255, int a = 255, int cursor = -1, int maxwidth = -1);

}

namespace Cube
{
	namespace GL
	{
		typedef void(__fastcall* tdraw_text)(const char* str, int left, int top, int r, int g, int b, int a, int cursor, int maxwidth);
		extern tdraw_text draw_text;

		typedef void(__fastcall* tdraw_textf)(const char* fstr, int left, int top, ...);
		extern tdraw_textf draw_textf;

	}
}