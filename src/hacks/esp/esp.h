#pragma once
#include "cube.h"

static constexpr int VIRTUAL_SCREEN_WIDTH{ 800 };
static constexpr int VIRTUAL_SCREEN_HEIGHT{ 600 };

constexpr int GAME_UNIT_MAGIC{ 1300 };

constexpr float PLAYER_HEIGHT{ 6.0f };
constexpr float PLAYER_WIDTH{ 2.5f };
constexpr float EYE_HEIGHT{ 4.5f };

constexpr float PLAYER_ASPECT_RATIO{ PLAYER_HEIGHT / PLAYER_WIDTH };

enum ESPStates
{
	DEPTH = 1 << 0,
	FULLBRIGHT = 1 << 1

};

struct ModelInfo
{
	Cube::fpsent* e;
	std::string name;
	ModelInfo() : e{nullptr}, name{} {}
	ModelInfo(Cube::fpsent* ent, std::string entName) : e{ ent }, name{ entName } {}
};

struct DrawInfo
{
	bool draw;
	bool drawFinished;
	std::deque<ModelInfo> players;
	Cube::Shader* shader;
	int8_t state;



	
	DrawInfo() : draw{ false }, drawFinished{ false }, players{}, shader{ nullptr }, state{ 0 } {}

	void DrawReset()
	{
		draw = false;
	}

	void Reset()
	{
		draw = false;
		drawFinished = false;
		players.empty();
		state = 0;
	}

	bool IsDuplicate(Cube::fpsent* ent)
	{
		for (auto info : players)
		{
			if (info.e == ent)
				return true;
		}
		return false;
	}
};

namespace Global
{
	inline DrawInfo drawinfo;
}

namespace Cube
{
	typedef Cube::model* (__fastcall* tLoadModel)(const char* name, int i, bool msg);
	extern tLoadModel LoadModel;

	typedef Cube::playermodelinfo* (__fastcall* tGetPlayerModelInfo)(Cube::fpsent* d);
	extern tGetPlayerModelInfo GetPlayerModelInfo;

	typedef void(__fastcall* tRotateBB)(vec* center, vec* radius, int yaw);
	extern tRotateBB RotateBB;

	typedef Cube::Shader* (__fastcall* tLookUpShader)(const char* name);

	extern tLookUpShader LookUpShader;

	typedef void(__fastcall* tSetShader)(Cube::Shader* _this);

	extern tSetShader SetShader;

	typedef void(__fastcall* tVColor)(float* r, float* g, float* b);

	extern tVColor vColor;

	typedef Cube::MSlot* (__fastcall* tLookUpMaterials)(int index, bool load);

	extern tLookUpMaterials LookUpMaterials;

	typedef Cube::VSlot* (__fastcall* tLookUpVSlot)(int index, bool load);

	extern tLookUpVSlot LookUpVSlot;
}

class ESP
{
private:
	bool m_built;
	int m_viewport[4];

public:
	ESP();

	void Run();

	bool CalcEspBox(Cube::fpsent* e, Vec3& left, Vec3& right);
	void DrawHealthBar(Cube::fpsent* e, float x, float y, float width, float height);
	void Draw(Cube::fpsent* e, const Vector3& cords, const Vector3& offsets);
};

