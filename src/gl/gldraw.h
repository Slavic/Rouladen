#pragma once
#pragma comment(lib, "OpenGL32.lib")

#include "pch.h"
#include "mem.h"
#include "geom.h"

#include "imgui/imgui.h"

namespace GL::rgb 
{
	const GLubyte red[3]{ 255, 0, 0 };
	const GLubyte green[3]{ 0, 255, 0 };
	const GLubyte gray[3]{ 55, 55, 55 };
	const GLubyte lightGray[3]{ 192, 192, 192 };
	const GLubyte black[3]{ 0, 0, 0 };
}

namespace GL
{
	constexpr auto doublePi{ PI * 2 };
	void SetUpOrtho();
	void SetUpOrtho(int top, int bottom, int left, int right);
	void RestoreGL();
	void LoadMatrix(float* m);

	void DrawFilledRect(float x, float y, float width, float height, const float color[4], bool border, float lineWidth = 1.0f, const float borderColor[4] = nullptr);
	void DrawFilledRect(float x, float y, float width, float height, ImVec4& color, bool border, float lineWidth = 1.0f, ImVec4 borderColor = ImVec4());
	void DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);

	void DrawDot(float x, float y);
	void DrawCircle(float x, float y, float radius, int numSides, ImVec4& color);

}

namespace Cube::GL
{
	typedef void(__fastcall* trender2dbox)(vec* o, float x, float y, float z);
	extern trender2dbox Render2dBox;
	
	typedef void(__fastcall* trender3dbox)(vec* o, float tofloor, float toceil, float xradius, float yradius);
	extern trender3dbox Render3dBox;
}