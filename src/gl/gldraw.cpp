#include "pch.h"
#include "gldraw.h"


void GL::SetUpOrtho()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::SetUpOrtho(int left, int right, int bottom, int top)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::RestoreGL()
{
	glPopMatrix();
	glPopAttrib();
}

void GL::LoadMatrix(float* m)
{
	glLoadMatrixf(m);
}


void GL::DrawFilledRect(float x, float y, float width, float height, const float color[4], bool border, float lineWidth, const float borderColor[4])
{
	if (border && borderColor == nullptr)
	{
		borderColor = color;
	}
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glColor4f(color[0], color[1], color[2], color[3]);

	float oldWidth;
	glGetFloatv(GL_LINE_WIDTH, &oldWidth);

	if (border) 
		glLineWidth(lineWidth);

	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width , y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glVertex2f(x, y);
	glEnd();


	if (border)
	{
		glColor4f(borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
		
		glBegin(GL_LINE_STRIP);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width + 0.5f, y + height);
		glVertex2f(x, y + height);
		glVertex2f(x, y);
		glEnd();
	}

	// restore line width
	glLineWidth(oldWidth);

}

void GL::DrawFilledRect(float x, float y, float width, float height, ImVec4& color, bool border, float lineWidth, ImVec4 borderColor)
{
	// Ugly default detection
	if (border && (borderColor.x + borderColor.y + borderColor.z + borderColor.w) / 4 < 0.01f)
	{
		borderColor = color;
	}
	glDepthMask(false);
	

	glColor4f(color.x, color.y, color.z, color.w);

	float oldWidth;
	glGetFloatv(GL_LINE_WIDTH, &oldWidth);

	if (border)
		glLineWidth(lineWidth);

	glBegin(GL_QUADS);
	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();


	if (border)
	{
		glColor3f(borderColor.x, borderColor.y, borderColor.z);

		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 0.5f, y - 0.5f);
		glVertex2f(x + width + 0.5f, y - 0.5f);
		glVertex2f(x + width + 0.5f, y + height + 0.5f);
		glVertex2f(x - 0.5f, y + height + 0.5f);
		glVertex2f(x - 0.5f, y - 0.5f);
		glEnd();
	}

	// restore line width
	glLineWidth(oldWidth);
}

void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y- 0.5f);
	glVertex2f(x + width + 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();

}

void GL::DrawDot(float x, float y)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void GL::DrawCircle(float x, float y, float radius, int numSides, ImVec4& color)
{
	float theta{};
	glColor4f(color.x, color.y, color.z, color.w);
	float oldWidth;
	glGetFloatv(GL_LINE_WIDTH, &oldWidth);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= numSides; ++i)
	{
		theta = DegreesToRadians(i);
		glVertex2f(x + (radius * cosf(theta)), y + (radius * sinf(theta)));
	}
	glEnd();
	glLineWidth(oldWidth);

}


namespace Cube::GL
{
	trender2dbox Render2dBox{ nullptr };
	trender3dbox Render3dBox{ nullptr };
}