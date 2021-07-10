#pragma once
#include "cube.h"

class Aimbot
{
private:
    Cube::fpsent* m_CurrentTarget;
    bool m_AttackToggled;

public:
    Aimbot();
    
    Cube::fpsent* GetClosetsEntity();
    Cube::fpsent* GetAnyEntity();
    void AimAt(Vec3& target);
    bool GetCrossHairEnt(vec3* cords);

    bool Run(Vec3* target, Vec3& pos);

};
