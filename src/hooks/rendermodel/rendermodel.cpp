#include "pch.h"
#include "rendermodel.h"
#include "gl/gldraw.h"
#include "offsets.h"
#include "hacks/esp/esp.h"
#include "hook.h"
#include "config.h"
#include "util.h"

tRenderModel RenderModel;

void __fastcall hRenderModel(Cube::entitylight* light, char* mdl, int anim, vec* o, float yaw, float pitch, int flags, Cube::dynent* d, Cube::modelattach* a, int basetime, int basetime2, float trans)
{	
	// Disable culling on player models
	if (Global::cfg && Global::drawinfo.draw && Global::cfg->GetState("r_WallHack") && Global::cfg->GetState("r_NoCull") && IsValidModel(mdl))
	{
		flags &= ~(Cube::MDL_CULL_VFC | Cube::MDL_CULL_DIST | Cube::MDL_CULL_OCCLUDED | Cube::MDL_CULL_QUERY | Cube::MDL_SHADOW | Cube::MDL_DYNSHADOW);
	}
	return RenderModel(light, mdl, anim, o, yaw, pitch, flags, d, a, basetime, basetime2, trans);
}

tAnimRender AnimRender;

tEndRender EndRender;

// Unused Cube::model->EndRender() VMT hook, left in for hEndRender
static auto modelVMT{ std::make_unique<VMTHook>() };

void __fastcall hAnimRender(Cube::model* _this, int anim, int basetime, int basetime2, vec* o, float yaw, float pitch, Cube::dynent* d, Cube::modelattach* a, vec* color, vec* dir, float transparent)
{	
	if (Global::drawinfo.draw && !Global::drawinfo.players.empty() && IsValidModel(_this->name))
	{
		Global::drawinfo.players.pop_front();
		if (Global::cfg)
		{
			if (!Global::cfg->GetState("r_EnemyOnly") || (Global::cfg->GetState("r_EnemyOnly") && IsEnemy(reinterpret_cast<Cube::fpsent*>(d))))
			{
				if (Global::cfg->GetState("r_WallHack"))
				{
					glDisable(GL_DEPTH_TEST);
					Global::drawinfo.state |= DEPTH;
				}

				if (Global::cfg->GetState("r_FullBright"))
				{
					float fullBright{ Global::cfg->GetValue("FullBright").get<float>() };
					_this->setfullbright(fullBright);
					Global::drawinfo.state |= FULLBRIGHT;

				}
				if (Global::cfg->GetState("r_Transparent"))
				{
					transparent = Global::cfg->GetValue("Transparent").get<float>();
				}

				Global::drawinfo.drawFinished = true;
			}
		}
	}
	// Call original render function
	AnimRender(_this, anim, basetime, basetime2, o, yaw, pitch, d, a, color, dir, transparent);

	// Cleanup changed settings for next render call
	if (Global::drawinfo.drawFinished)
	{
		if (Global::drawinfo.state & DEPTH)
		{
			glEnable(GL_DEPTH_TEST);

		}
		if (Global::drawinfo.state & FULLBRIGHT)
			_this->setfullbright(0.0f);

		if (Global::drawinfo.players.empty())
			Global::drawinfo.Reset();

	}
	return;
}


// Unused VMT Hook but left in incase I need this again
void __fastcall hEndRender(Cube::model* _this)
{	
	static auto oEndRender{ modelVMT->GetOriginal<decltype(&hEndRender)>() };
	if (Global::drawinfo.drawFinished)
	{
		if (Global::drawinfo.state & DEPTH)
		{
			glEnable(GL_DEPTH_TEST);
			
		}
		if (Global::drawinfo.state & FULLBRIGHT)
			_this->setfullbright(0.0f);

		Global::drawinfo.Reset();
	}

	return oEndRender(_this);
}

