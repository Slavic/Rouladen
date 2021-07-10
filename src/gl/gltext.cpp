#include "pch.h"
#include "gltext.h"
#include "geom.h"
#include "offsets.h"

void GL::Font::Build(int height)
{
	m_hdc = wglGetCurrentDC();
	m_base = glGenLists(96);
	HFONT hFont{ CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas") };
	HFONT hOldFont{ reinterpret_cast<HFONT>(SelectObject(m_hdc, hFont)) };
	wglUseFontBitmaps(m_hdc, 32, 96, m_base);
	SelectObject(m_hdc, hOldFont);
	DeleteObject(hFont);

	m_bBuilt = true;
}

void GL::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...)
{
	glColor3ub(color[0], color[1], color[2]);
	glRasterPos2f(x, y);


	char text[100];
	va_list args;

	va_start(args, format);
	vsprintf_s(text, 100, format, args);
	va_end(args);

	glPushAttrib(GL_LIST_BIT);
	glListBase(m_base - 32); 
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

vec3 GL::Font::CenterText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	vec3 text{ x + (width - textWidth) / 2 , y + textHeight, 0.0f };
	return text;

}

float GL::Font::CenterText(float x, float width, float textWidth)
{
	if (width > textWidth)
	{
		float difference{ width - textWidth };
		return x + (difference / 2);
	}
	else
	{
		float difference{ textWidth - width };
		return x - (difference / 2);
	}
}

bool GL::Font::GetBuildStatus(){ return m_bBuilt; }

HDC GL::Font::GetHDC() { return m_hdc; }

namespace Cube::GL
{
	tdraw_text draw_text{ nullptr };
	tdraw_textf draw_textf{ nullptr };
}

void GL::RenderText(const char* str, int left, int top, int r, int g, int b, int a, int cursor, int maxwidth)
{
	glEnable(GL_BLEND);
	Cube::GL::draw_text(str, left, top, r, g, b, a, cursor, maxwidth);
	glDisable(GL_BLEND);
}
