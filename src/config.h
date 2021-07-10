#pragma once
#include "imgui/imgui.h"
#include "json/json.hpp"

// Polluting the global namespace for readability
using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;

// Forward declaration to prevent looping headers
namespace UI {
	class Menu;
};

// Class that reads and saves configs as .json
class Config
{
private:
	// Due to nlohmann::json using nesting "json" containers it is near impossible to get a ptr to the raw data of an array, hence the need for an unordered map of Vector4's
	std::unordered_map<std::string, ImVec4> m_Vec4;
	std::string m_Path;
	bool m_Created;
	bool m_SavingAllowed;
	json m_Json;

	// Binds map, needs better encapsulation but I don't know how
	std::unordered_map<int64_t, std::vector<std::string>> m_Binds;

	// Used to temporarily store the name of a bind, essentially glorified globals, which is why they are public
	std::string m_BindName;
	bool m_ReadInput;

	bool ReadBinds();

public:
	Config(std::string filepath);
	~Config();

	bool ReadConfig();
	bool SaveConfig();
	bool ReadColors();

	bool GetState(const std::string& name);
	void SetState(const std::string& name, bool state);
	void Toggle(const std::string& name);
	void ToggleMode(const std::string& name);
	bool IsCommand(const std::string& name);
	bool IsHold(const std::string& name);
	ImVec4 GetVec4(const std::string& name);
	json GetValue(const std::string& name);

	// In case a valid path to the AppData/Roaming folder cannot be attained
	void AllowSaving(bool state) { m_SavingAllowed = state; }

	// Cause everyone needs a friend, and ImGui needs a pointer/refrence to data to handle checkboxes
	friend class UI::Menu;

	friend LRESULT WINAPI hWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
};
namespace Global 
{
	// Global cfg that everything can access
	inline std::unique_ptr<Config> cfg;
}
// Functions that handle creation and saving
void LoadConfig();
void SaveConfig();