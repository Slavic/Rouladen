#include "pch.h"
#include "esp.h"

#include "gl/gltext.h"
#include "gl/gldraw.h"
#include "offsets.h"
#include "util.h"
#include "config.h"

#include "imgui/imgui.h"

namespace Cube
{
	tLoadModel LoadModel;
	tGetPlayerModelInfo GetPlayerModelInfo;
	tRotateBB RotateBB;
	tLookUpShader LookUpShader;
	tSetShader SetShader;
	tVColor vColor;
	tLookUpMaterials LookUpMaterials;
	tLookUpVSlot LookUpVSlot;
}

ESP::ESP() : m_built{true}, m_viewport{0, 0, 0, 0}
{
	
}

void ESP::Run()
{
	if (m_built && Global::Offsets::entList && Global::cfg)
	{
		glGetIntegerv(GL_VIEWPORT, m_viewport);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (Global::cfg->GetState("r_Esp"))
		{
			for (int32_t i = 1; i < Global::Offsets::entList->length(); ++i)
			{
			
				Cube::fpsent* ent { Global::Offsets::entList->buf[i] };
				if (ent->state == 1)
					continue;
				if (Global::cfg->GetState("r_EspEnemy") && !IsEnemy(ent))
					continue;

				if (Global::cfg->GetState("r_EspVisible"))
				{
					Vec3 resultVec;
					if (!Cube::RayCubeLos(&Global::Offsets::localPlayer->o, &ent->o, &resultVec))
						continue;
				}
			
				
				vec3 center{ ent->o };
				vec3 screenCords;
				vec3 screenCordsLeft;
				vec3 screenCordsRight;
			
				if (CalcEspBox(ent, screenCordsLeft, screenCordsRight))
				{
					Vec3 offsets{ screenCordsRight.x - screenCordsLeft.x, screenCordsRight.y - screenCordsLeft.y, 0.0f };
				
					Draw(ent, screenCordsLeft, offsets);
				}
			
			}
		}
		if (Global::cfg->GetState("r_Aimbot") && Global::cfg->GetState("r_Fov") && Global::cfg->GetState("r_FovDraw"))
		{
			ImVec4 color{ Global::cfg->GetVec4("FovColor") };
			float fovLimit{ m_viewport[2] * (Global::cfg->GetValue("AimbotFov").get<json::number_integer_t>() / static_cast<float>(*Global::Offsets::fov)) / 2.0f };
			GL::DrawCircle(m_viewport[2] / 2.0f, m_viewport[3] / 2.0f, fovLimit, 360, color);
		}
		glDisable(GL_BLEND);

	}
}

// Calculates two points perpendiculuar of an object within 3d space based on angle
bool ESP::CalcEspBox(Cube::fpsent* e, Vec3& left, Vec3& right)
{
	Vec3 playerAngs{ Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, Global::Offsets::localPlayer->roll };
	Vec3 angle{ CalcAngle(Global::Offsets::localPlayer->o, e->newpos)};
	Vec3 diffPos{ Global::Offsets::localPlayer->o - e->newpos };

	// Ignore height or z distance
	float distance{ sqrtf(diffPos.x * diffPos.x + diffPos.y * diffPos.y) };
	float radius{ e->radius + 0.2f };

	float deltaAngle{ RadiansToDegrees(atanf(radius / distance)) };

	float hypotenuse{ sqrtf((distance * distance) + (radius * radius)) };

	// Basic trig, calculates the lengths of the opposite and adjacent side formed by the hypotenuse and delta angle
	float sinOffsetLeft{ sinf(DegreesToRadians(angle.x - deltaAngle)) * hypotenuse };
	float cosOffsetLeft{ cosf(DegreesToRadians(angle.x - deltaAngle)) * hypotenuse };

	float sinOffsetRight{ sinf(DegreesToRadians(angle.x + deltaAngle)) * hypotenuse };
	float cosOffsetRight{ cosf(DegreesToRadians(angle.x + deltaAngle)) * hypotenuse };

	// Add the offsets to the player pos as thats the origin of the angle
	Vec3 playerPos{ Global::Offsets::localPlayer->o};
	Vec3 vecLeft{ playerPos.x - sinOffsetLeft, playerPos.y + cosOffsetLeft, e->newpos.z + e->aboveeye };
	Vec3 vecRight{ playerPos.x - sinOffsetRight, playerPos.y + cosOffsetRight, e->newpos.z - e->eyeheight };

	// If both points are infront of us and visible, draw esp
	if (WorldToScreen(vecLeft, left, Global::Offsets::matrix, m_viewport[2], m_viewport[3]) && WorldToScreen(vecRight, right, Global::Offsets::matrix, m_viewport[2], m_viewport[3]))
		return true;
	return false;
	
}

void ESP::DrawHealthBar(Cube::fpsent* e, float x, float y, float width, float height)
{
	float color[4]{};
	// Percentage of health left
	float scale{ static_cast<float>(e->health) / e->maxhealth };
	// HealthBoost
	if (e->health > 100)
	{
		// Light Blue
		color[0] = 0.015f;
		color[1] = 0.749f;
		color[2] = 0.964f;
		color[3] = 0.9f;
	}
	else
	{
		if (scale > 0.66f)
		{
			// Green
			color[0] = 0.082f;
			color[1] = 0.698f;
			color[2] = 0.145f;
			color[3] = 0.9f;
		}
		else if (scale > 0.33f)
		{
			// Yellow
			color[0] = 0.886f;
			color[1] = 0.776f;
			color[2] = 0.094f;
			color[3] = 0.9f;
		}
		else
		{
			color[0] = 0.964f;
			color[1] = 0.117f;
			color[2] = 0.125f;
			color[3] = 0.9f;
		}
	}
	
	
	GL::DrawFilledRect(x + 0.1f, y + 0.1f + (height - (scale * height)), width - 0.2f, (height - 0.2f) * scale , color, false);
	GL::DrawOutline(x, y, width, height, 2.0f, GL::rgb::black);
}


void ESP::Draw(Cube::fpsent* e, const Vector3& cords, const Vector3& offsets)
{
	// Push matrix to allow us to translate and scale it
	glPushMatrix();
	ImVec4 color;

	if (IsEnemy(e))
		color = Global::cfg->GetVec4("EnemyColor");
	else
		color = Global::cfg->GetVec4("TeamColor");

	if (Global::cfg->GetState("r_EspBoxes"))
		GL::DrawFilledRect(cords.x, cords.y, offsets.x, offsets.y, color, true, 2.0f);
	if (Global::cfg->GetState("r_HealthBar"))
		DrawHealthBar(e, cords.x - offsets.x / 2 , cords.y, offsets.x / 6, offsets.y );

	static GL::Font font;
	if (!font.GetBuildStatus())
		font.Build(14);
	const uint8_t textColor[]{ 240, 240, 240 };

	if (Global::cfg->GetState("r_EspNames"))
	{
		glTranslatef(cords.x + offsets.x * 1.1, cords.y + 7.0f, 1.0f);
		float scale{ offsets.x / 1.5f };
		scale = std::clamp(scale, 0.5f, 1.5f);
		glScalef(scale, scale, scale);
		font.Print(0, 0, GL::rgb::lightGray, e->name);
	}

	glPopMatrix();
}
