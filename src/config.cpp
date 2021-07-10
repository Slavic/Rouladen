#include "pch.h"
#include "config.h"
#include "imgui/imgui.h"
#include "console.h"


bool Config::ReadBinds()
{
    try
    {
        for (auto &bind : m_Json.items())
        {
            const std::string& name{ bind.key() };
            // Looks ugly but all bindable objects start with "r_", and name should never be shorter than 2 characters
            if (name[0] == 'r' && name[1] == '_')
            {
                // Technically a narrowing conversion of int64_t to int32_t but keyIndex should never exceed 1000
                int64_t keyIndex{ bind.value()["key"].get<json::number_integer_t>() };
                if (keyIndex != -1)
                    m_Binds[keyIndex].emplace_back(name);
               
            }
        }
    }
    catch (const std::exception& e)
    {
        std::string temp{ "[Error] " };
        temp.append(e.what());
        Global::consoleQueue.emplace_back(temp);
        return false;
    }
    return true;
}

Config::Config(std::string filepath)
    : m_Path{ filepath }, m_Created{ false }, m_Vec4{}, m_Binds{}, m_SavingAllowed{ true }, m_Json{}, m_ReadInput{ false }, m_BindName{}
{
    // Seperate function to better support multiple configs in the future
    if (!ReadConfig())
        Global::consoleQueue.emplace_back("[Error] Could not read config");
    else if (!ReadBinds())
        Global::consoleQueue.emplace_back("[Error] Could not prase binds");
    else
        m_Created = true;

}

Config::~Config()
{
}

bool Config::ReadConfig()
{
    std::ifstream file(m_Path);
    // Check to see if the file can be opened and is not empty
    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
    {
        try
        {
            m_Json = json::parse(file);
            // Seperate out the Vector4s to allow them to be passed by refrence
            for (auto &it : m_Json["Vector4s"].items())
            {
                auto vec{ it.value().get<std::vector<float>>() };
                m_Vec4[it.key()] = ImVec4(vec[0], vec[1], vec[2], vec[3]);
            }
        }
        catch (const std::exception& e)
        {
            std::string temp{ "[Error] " };
            temp.append(e.what());
            Global::consoleQueue.emplace_back(temp);
            return false;
        }

        file.close();
        return true;
    }
    else if (!m_Created)
    {
        // If the file cannot be read and config is not loaded, use the hardcoded backup
        try
        {
            // Backup Hardcoded Config
            m_Json = R"({
	        "Vector4s": {
				"EnemyColor": [ 0.772, 0.050, 0.050, 0.80 ],
				"FovColor": [ 0.0, 1.0, 0.0, 1.0 ],
				"TeamColor": [ 0.078, 0.878, 0.0, 0.5 ]
			},
			"ImGui_Colors":{
				"Border":[0.5,0.5,0.5,0.5],
				"BorderShadow":[0.0,0.0,0.0,0.0],
				"Button":[0.35,0.4,0.61,0.62],
				"ButtonActive":[0.46,0.54,0.8,1.0],
				"ButtonHovered":[0.4,0.48,0.71,0.79],
				"CheckMark":[0.9,0.9,0.9,0.5],
				"ChildBg":[0.0,0.0,0.0,0.0],
				"DragDropTarget":[1.0,1.0,0.0,0.9],
				"FrameBg":[0.43,0.43,0.43,0.39],
				"FrameBgActive":[0.42,0.41,0.64,0.69],
				"FrameBgHovered":[0.47,0.47,0.69,0.4],
				"Header":[0.27,0.27,0.54,0.83],
				"HeaderActive":[0.32,0.32,0.63,0.87],
				"HeaderHovered":[0.45,0.45,0.9,0.8],
				"MenuBarBg":[0.4,0.4,0.55,0.8],
				"ModalWindowDimBg":[0.2,0.2,0.2,0.35],
				"NavHighlight":[0.45,0.45,0.9,0.8],
				"NavWindowingDimBg":[0.8,0.8,0.8,0.2],
				"NavWindowingHighlight":[1.0,1.0,1.0,0.7],
				"PlotHistogram":[0.9,0.7,0.0,1.0],
				"PlotHistogramHovered":[1.0,0.6,0.0,1.0],
				"PlotLines":[1.0,1.0,1.0,1.0],
				"PlotLinesHovered":[0.9,0.7,0.0,1.0],
				"PopupBg":[0.11,0.11,0.14,0.92],
				"ResizeGrip":[1.0,1.0,1.0,0.1],
				"ResizeGripActive":[0.78,0.82,1.0,0.9],
				"ResizeGripHovered":[0.78,0.82,1.0,0.6],
				"ScrollbarBg":[0.2,0.25,0.3,0.6],
				"ScrollbarGrab":[0.4,0.4,0.8,0.3],
				"ScrollbarGrabActive":[0.41,0.39,0.8,0.6],
				"ScrollbarGrabHovered":[0.4,0.4,0.8,0.4],
				"Separator":[0.5,0.5,0.5,0.6],
				"SeparatorActive":[0.7,0.7,0.9,1.0],
				"SeparatorHovered":[0.6,0.6,0.7,1.0],
				"SliderGrab":[1.0,1.0,1.0,0.3],
				"SliderGrabActive":[0.41,0.39,0.8,0.6],
				"Tab":[0.27,0.27,0.54,0.83],
				"TabActive":[0.32,0.32,0.63,0.87],
				"TabHovered":[0.45,0.45,0.9,0.8],
				"TabUnfocused":[0.2832,0.2832,0.56880003,0.8212],
				"TabUnfocusedActive":[0.3504,0.3504,0.6516,0.8372],
				"TableBorderLight":[0.26,0.26,0.28,1.0],
				"TableBorderStrong":[0.31,0.31,0.45,1.0],
				"TableHeaderBg":[0.27,0.27,0.38,1.0],
				"TableRowBg":[0.0,0.0,0.0,0.0],
				"TableRowBgAlt":[1.0,1.0,1.0,0.07],
				"Text":[0.9,0.9,0.9,1.0],
				"TextDisabled":[0.6,0.6,0.6,1.0],
				"TextSelectedBg":[0.0,0.0,1.0,0.35],
				"TitleBg":[0.27,0.27,0.54,0.83],
				"TitleBgActive":[0.32,0.32,0.63,0.87],
				"TitleBgCollapsed":[0.4,0.4,0.8,0.2],
				"WindowBg":[0.0,0.0,0.0,0.85]
			},
			"r_Aimbot": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_Fov": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_Silent": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_AutoShoot": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_MagicBullets": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_IgnoreTeam": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_FovDraw": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"AimbotFov": 20,
			"r_Trigger": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_Esp": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_EspBoxes": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_HealthBar": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_EspEnemy": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_EspVisible": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_EspNames": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"EspWidth": 1.0,
			"r_EnemyOnly": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_WallHack": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_FullBright": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"FullBright": 1.1,
			"r_Transparent": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"Transparent": 0.5,
			"r_NoCull": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_NoRecoil": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_NoSpread": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_ItemPickUp": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_ThirdPerson": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_BunnyHop": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_Menu": {
				"state": true,
				"key": 45,
				"on_hold": false,
				"active": false
			},
			"r_InfoMenu": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_PlayerList": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			},
			"r_Console": {
				"state": false,
				"key": -1,
				"on_hold": false,
				"active": false
			}
            })"_json;

            for (auto &it : m_Json["Vector4s"].items())
            {
                auto vec{ it.value().get<std::vector<float>>() };
                m_Vec4[it.key()] = ImVec4(vec[0], vec[1], vec[2], vec[3]);
            }
        }
        catch (std::exception& e)
        {
            std::string temp{ "[Error] " };
            temp.append(e.what());
            Global::consoleQueue.emplace_back(temp);
            return false;
        }
        return true;
    }
    return false;
    

}

bool Config::SaveConfig()
{
    if (m_Created && m_SavingAllowed)
    {
        // Place the Vector4s back into the json
        for (auto &it : m_Vec4)
        {
            m_Json["Vector4s"][it.first] = json::array({ it.second.x, it.second.y, it.second.z, it.second.w });
        }
        // Place ImGui colors back into the json
        ImGuiStyle& style{ ImGui::GetStyle() };
        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            const char* name = ImGui::GetStyleColorName(i);
            const ImVec4& color = style.Colors[i];
            m_Json["ImGui_Colors"][name] = json::array({ color.x, color.y, color.z, color.w });
        }
        std::ofstream file;

        file.open(m_Path, std::ofstream::out | std::ofstream::trunc);

        if (file.is_open())
        {
            // Write json to file
            file << m_Json;
        }
        else
        {
            // If the path cannot be open, create folders in that path and try again
            std::string folderPath{ m_Path };
            folderPath.erase(folderPath.find_last_of("\\"));
            std::filesystem::path path(folderPath);
            std::filesystem::create_directories(path);
            file.open(m_Path, std::ofstream::out | std::ofstream::trunc);
            if (file.is_open())
            {
                file << m_Json;
            }
            else
                return false;
        }
        file.close();
        return true;
    }
    return false;
}
// Only call this after Menu intililization
bool Config::ReadColors()
{
    // Read and place ImGui colors back into context
    ImGuiStyle& style{ ImGui::GetStyle() };
    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        const char* name = ImGui::GetStyleColorName(i);
        auto vec{ m_Json["ImGui_Colors"][name].get<std::vector<float>>() };
        style.Colors[i] = ImVec4(vec[0], vec[1], vec[2], vec[3]);
    }
    return true;
}

// Get the boolean state of a config option
bool Config::GetState(const std::string& name)
{
    if (m_Created)
    {
        if (m_Json[name]["on_hold"].get<json::boolean_t>())
            return m_Json[name]["state"].get<json::boolean_t>() && m_Json[name]["active"].get<json::boolean_t>();
        else
            return m_Json[name]["state"].get<json::boolean_t>();
    }
    return false;
}

void Config::SetState(const std::string& name, bool state)
{
    if (m_Created)
    {
        if (m_Json[name]["on_hold"].get<json::boolean_t>())
            m_Json[name]["active"] = state;
        else
            m_Json[name]["state"] = state;

    }
}

void Config::Toggle(const std::string& name)
{
    if (m_Created)
    {
        if (m_Json[name]["on_hold"].get<json::boolean_t>())
            m_Json[name]["active"] = true;
        else
            m_Json[name]["state"] = !m_Json[name]["state"].get<json::boolean_t>();
    }
}

void Config::ToggleMode(const std::string& name)
{
    if (m_Created)
    {
        m_Json[name]["on_hold"] = !m_Json[name]["on_hold"].get<json::boolean_t>();
        m_Json[name]["active"] = false;
    }
}

bool Config::IsCommand(const std::string& name)
{
    if (m_Created)
    {
        if (m_Json.find(name) == m_Json.end())
            return false;
        return true;
    }
    return false;
}

bool Config::IsHold(const std::string& name)
{
    return m_Json[name]["on_hold"].get<json::boolean_t>();
}

ImVec4 Config::GetVec4(const std::string& name)
{
    if (m_Created)
    {
        if (m_Vec4.find(name) == m_Vec4.end())
            return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        else
        {
            return m_Vec4[name];
        }
    }
    // Have to return something so empty Vector4 it is
    return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}

// Used to retrieve values that are not booleans and Vector4s, such as floats or ints
json Config::GetValue(const std::string& name)
{
    if (m_Created) 
    {
        if (m_Json.find(name) == m_Json.end())
            throw std::runtime_error("Invalid value requested from Json");
        else
            return m_Json[name];
    }
    // Returning empty json
    return json{};
}

// Initilizes Global::cfg
void LoadConfig()
{
    // Builds path to AppData/Roaming/Rouladen/
    WCHAR* pszPath{ nullptr };
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &pszPath) == S_OK)
    {
        std::filesystem::path folderPath{ pszPath };
      
        folderPath.append("Rouladen\\config.json");
        // Unique pointer so config is automatically destroyed on close
        Global::cfg = std::make_unique<Config>(Config(folderPath.string()));

        CoTaskMemFree(pszPath);
        // To prevent from writing bad configs during testing
        //Global::cfg->AllowSaving(false);
    }
    else
    {
        // With no valid path we use the fallback but disable saving to prevent erroneous folders from being created
        Global::cfg = std::make_unique<Config>(Config(""));
        Global::cfg->AllowSaving(false);
    }
}

void SaveConfig()
{
    if (Global::cfg)
        Global::cfg->SaveConfig();
}
