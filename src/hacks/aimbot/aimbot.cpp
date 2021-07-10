#include "pch.h"
#include "aimbot.h"
#include "config.h"
#include "util.h"
#include "geom.h"
#include "gl/gldraw.h"

Aimbot::Aimbot() : m_CurrentTarget{ nullptr }, m_AttackToggled{ false }
{
}

Cube::fpsent* Aimbot::GetClosetsEntity()
{
	float closestDistance{ 100000.0f };
	int closestDistanceIndex{ 0 };
	float currentDistance{ 0.0f };
	Vec3 angleDiff{ 0.0f, 0.0f, 0.0f };
	Vec3 resultVec;
	int viewport[4]{};
	if (Global::cfg->GetState("r_Fov"))
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		if (m_CurrentTarget != nullptr && IsEnemy(m_CurrentTarget) && Cube::RayCubeLos(&Global::Offsets::localPlayer->o, &m_CurrentTarget->o, &resultVec))
		{
			Vec3 localPlayerAng{ Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, Global::Offsets::localPlayer->roll };

			angleDiff = CalcAngle(Global::Offsets::localPlayer->o, m_CurrentTarget->o) - localPlayerAng;
			currentDistance = sqrt(pow(angleDiff.x, 2) + pow(angleDiff.y, 2));
			float fovLimit{ static_cast<float>(Global::cfg->GetValue("AimbotFov").get<json::number_integer_t>()) };
			if (currentDistance < closestDistance && currentDistance < fovLimit)
			{
				return m_CurrentTarget;
			}

		}

		for (int32_t i = 1; i < Global::Offsets::entList->length(); ++i)
		{
			Cube::fpsent* e{ Global::Offsets::entList->buf[i] };
			if (IsEnemy(e) && Cube::RayCubeLos(&Global::Offsets::localPlayer->o, &e->o, &resultVec))
			{
				Vec3 localPlayerAng{ Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, 0.0f };
				
				angleDiff = CalcAngle(Global::Offsets::localPlayer->o, e->o) - localPlayerAng;
				currentDistance = sqrt(pow(angleDiff.x, 2) + pow(angleDiff.y, 2));
				float fovLimit{ static_cast<float>(Global::cfg->GetValue("AimbotFov").get<json::number_integer_t>()) };
				if (currentDistance < closestDistance && currentDistance < fovLimit)
				{
					closestDistance = currentDistance;
					closestDistanceIndex = i;
				}
			}
		}
		if (closestDistanceIndex > 0)
		{
			m_CurrentTarget = Global::Offsets::entList->buf[closestDistanceIndex];
			return m_CurrentTarget;
		}
		
	}
	else
	{
		for (int32_t i = 1; i < Global::Offsets::entList->length(); ++i)
		{
			Cube::fpsent* e{ Global::Offsets::entList->buf[i] };
			if (IsEnemy(e) && Cube::RayCubeLos(&Global::Offsets::localPlayer->o, &e->o, &resultVec))
			{
				currentDistance = Global::Offsets::localPlayer->o.Distance(e->o);
				if (currentDistance < closestDistance)
				{
					closestDistance = currentDistance;
					closestDistanceIndex = i;
				}
			}
		}
		if (closestDistanceIndex > 0)
		{
			m_CurrentTarget = Global::Offsets::entList->buf[closestDistanceIndex];
			return m_CurrentTarget;
		}
	}
	return nullptr;
}

Cube::fpsent* Aimbot::GetAnyEntity()
{
	float closestDistance{ 100000.0f };
	int closestDistanceIndex{ 0 };
	float currentDistance{ 0.0f };
	Vec3 angleDiff{ 0.0f, 0.0f, 0.0f };
	Vec3 resultVec;

	if (Global::cfg->GetState("r_Fov"))
	{

		if (m_CurrentTarget != nullptr && IsEnemy(m_CurrentTarget))
		{
			Vec3 localPlayerAng{ Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, Global::Offsets::localPlayer->roll };

			angleDiff = CalcAngle(Global::Offsets::localPlayer->o, m_CurrentTarget->o) - localPlayerAng;
			currentDistance = sqrt(pow(angleDiff.x, 2) + pow(angleDiff.y, 2));
			if (currentDistance < closestDistance && currentDistance < Global::cfg->GetValue("AimbotFov").get<json::number_integer_t>())
			{
				return m_CurrentTarget;
			}

		}

		for (int32_t i = 1; i < Global::Offsets::entList->length(); ++i)
		{
			Cube::fpsent* e{ Global::Offsets::entList->buf[i] };
			if (IsEnemy(e))
			{
				Vec3 localPlayerAng{ Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, 0.0f };

				angleDiff = CalcAngle(Global::Offsets::localPlayer->o, e->o) - localPlayerAng;
				currentDistance = sqrt(pow(angleDiff.x, 2) + pow(angleDiff.y, 2));
				if (currentDistance < closestDistance && currentDistance < Global::cfg->GetValue("AimbotFov").get<json::number_integer_t>())
				{
					closestDistance = currentDistance;
					closestDistanceIndex = i;
				}
			}
		}
		if (closestDistanceIndex > 0)
		{
			m_CurrentTarget = Global::Offsets::entList->buf[closestDistanceIndex];
			return m_CurrentTarget;
		}

	}
	else
	{
		for (int32_t i = 1; i < Global::Offsets::entList->length(); ++i)
		{
			Cube::fpsent* e{ Global::Offsets::entList->buf[i] };
			if (IsEnemy(e))
			{
				currentDistance = Global::Offsets::localPlayer->o.Distance(e->o);
				if (currentDistance < closestDistance)
				{
					closestDistance = currentDistance;
					closestDistanceIndex = i;
				}
			}
		}
		if (closestDistanceIndex > 0)
		{
			m_CurrentTarget = Global::Offsets::entList->buf[closestDistanceIndex];
			return m_CurrentTarget;
		}
	}
	return nullptr;
}

void Aimbot::AimAt(Vec3& target)
{
	Vec3 angle{ CalcAngle(Global::Offsets::localPlayer->o, target) };
	
	if (angle.y < 90.0f && angle.y > -90.0f && angle.x > 0.0f && angle.x < 360.0f)
	{
		Global::Offsets::localPlayer->pitch = angle.y;
		Global::Offsets::localPlayer->yaw = angle.x;
	}
}

bool Aimbot::GetCrossHairEnt(vec3* cords)
{
	for (int i = 1; i < Global::Offsets::entList->length(); ++i)
	{
		Cube::fpsent* e{ Global::Offsets::entList->buf[i] };
		float hold;
		if (IsEnemy(e) && Cube::Intersect(e, &Global::Offsets::localPlayer->o, cords, &hold))
			return true;
	}
	return false;
}

bool Aimbot::Run(Vec3* target, Vec3& pos)
{
	if (!Global::cfg)
		return false;
	if (Global::cfg->GetState("r_Aimbot"))
	{
		Cube::fpsent* e{ nullptr };
		if (Global::cfg->GetState("r_MagicBullets"))
			e = GetAnyEntity();
		else
			e = GetClosetsEntity();

		if (e)
		{
			pos = e->o;
			if (!Global::cfg->GetState("r_Silent"))
				AimAt(e->o);
			if (Global::cfg->GetState("r_AutoShoot"))
			{
				Global::Offsets::localPlayer->attacking = true;
				m_AttackToggled = true;
			}
			else if (m_AttackToggled)
			{
				Global::Offsets::localPlayer->attacking = false;
				m_AttackToggled = false;
			}
		
			return true;	
		}

	}
	else if (Global::cfg->GetState("r_Trigger"))
	{
		if (GetCrossHairEnt(target))
		{
			Global::Offsets::localPlayer->attacking = true;
			m_AttackToggled = true;
		}
		else if (m_AttackToggled)
		{
			Global::Offsets::localPlayer->attacking = false;
			m_AttackToggled = false;
		}
		return false;
	}
	
	m_CurrentTarget = nullptr;
	if (m_AttackToggled)
	{
		Global::Offsets::localPlayer->attacking = false;
		m_AttackToggled = false;
	}

	return false;
}
