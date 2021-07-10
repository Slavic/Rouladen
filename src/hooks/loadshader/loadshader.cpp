#include "pch.h"
#include "loadshader.h"
#include "hacks/esp/esp.h"

tLoadShader LoadShader;

// Unused in current build but can be used with Cube::model->PreloadShader() to grab model shaders
Cube::Shader* __fastcall hLoadShader(Cube::animmodel::skin* _this)
{
    if (!Global::drawinfo.shader)
        Global::drawinfo.shader = _this->shader;

    Cube::VSlot* vslot{ Cube::LookUpVSlot(_this->tex->id, true) };

    vslot->colorscale.x = 1.0f;
    vslot->colorscale.y = 1.0f;
    vslot->colorscale.z = 0.0f;

    return LoadShader(_this);
}
