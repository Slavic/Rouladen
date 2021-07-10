#include "pch.h"
#include "menu.h"
#include "offsets.h"
#include "config.h"
#include "util.h"
#include "console.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_internal.h"


inline IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC originalWndProc{ NULL };

tShowCursor SDL_ShowCursor;
tSDL_SetRelativeMouseMode SDL_SetRelativeMouseMode;

LRESULT WINAPI hWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// If menu is open let ImGui read the input information but return true to prevent window from receiving input
	if (Global::cfg && Global::cfg->GetState("r_Menu") && wParam != VK_INSERT && !Global::cfg->m_ReadInput)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
	}
	else if (Global::cfg->m_ReadInput && wParam <= VK_OEM_CLEAR && wParam != 0)
	{
		std::stringstream logFormat{};
		if (wParam == VK_INSERT || wParam == VK_END)
		{
			logFormat << "[Error] Attempted to bind " << Global::cfg->m_BindName << " to Insert\nInsert is a protected bind";
			Global::consoleQueue.emplace_back(logFormat.str());
			Global::cfg->m_ReadInput = false;
			Global::cfg->m_BindName = "";
			return true;
		}
		int64_t bindKey{ Global::cfg->m_Json[Global::cfg->m_BindName]["key"].get<json::number_integer_t>() };
		logFormat << "Rebound " << Global::cfg->m_BindName << " from " << bindKey << " to ";
		for (int32_t i = 0; i < Global::cfg->m_Binds[bindKey].size(); ++i)
		{
			// Like strcmp returns 0 on match
			if (!Global::cfg->m_Binds[bindKey][i].compare(Global::cfg->m_BindName)) 
			{
				Global::cfg->m_Binds[bindKey].erase(Global::cfg->m_Binds[bindKey].begin() + i);
				break;
			}
		}
		if (wParam == VK_ESCAPE)
		{
			Global::cfg->m_Json[Global::cfg->m_BindName]["key"] = -1;
			logFormat << -1;
		}
		else
		{
			int64_t key{ static_cast<int64_t>(wParam) };
			Global::cfg->m_Json[Global::cfg->m_BindName]["key"] = key;
			Global::cfg->m_Binds[key].emplace_back(Global::cfg->m_BindName);
			logFormat << key;
		}
		Global::cfg->m_ReadInput = false;
		Global::cfg->m_BindName = "";
		Global::consoleQueue.emplace_back(logFormat.str());
		return true;
	}
	else if (wParam <= VK_OEM_CLEAR && wParam != 0) // Avoid processing mouse relalted events
	{
		switch (msg)
		{
		case WM_KEYDOWN:
			// 30th bit is on if the key is being held down
			if (lParam & (1 << 30))
				break;
			if (!Global::cfg->m_Binds[wParam].empty())
				for (int32_t i = 0; i < Global::cfg->m_Binds[wParam].size(); ++i)
				{
					Global::cfg->Toggle(Global::cfg->m_Binds[wParam][i]);
				}

			break;
		case WM_KEYUP:
			if (!Global::cfg->m_Binds[wParam].empty())
				for (int32_t i = 0; i < Global::cfg->m_Binds[wParam].size(); ++i)
					if (Global::cfg->IsHold(Global::cfg->m_Binds[wParam][i]))
						Global::cfg->SetState(Global::cfg->m_Binds[wParam][i], false);
			break;
		default:
			break;
		}

	}
	
	
	return CallWindowProc(originalWndProc, hWnd, msg, wParam, lParam);
}

UI::Menu::Menu()
	: m_Focused{ false }, m_hWnd{ NULL }, m_KeyNames{}, m_DebugConsole{}
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_KeyNames[-2] = "?";
	m_KeyNames[-1] = "  ";
	m_KeyNames[VK_LBUTTON] = "M1";
	m_KeyNames[VK_RBUTTON] = "M2";
	m_KeyNames[VK_CANCEL] = "Cancel";
	m_KeyNames[VK_MBUTTON] = "M3";
	m_KeyNames[VK_BACK] = "Back";
	m_KeyNames[VK_TAB] = "Tab";
	m_KeyNames[VK_CLEAR] = "Clear";
	m_KeyNames[VK_RETURN] = "Enter";
	m_KeyNames[VK_SHIFT] = "Shift";
	m_KeyNames[VK_CONTROL] = "Ctrl";
	m_KeyNames[VK_PAUSE] = "Pause";
	m_KeyNames[VK_CAPITAL] = "Cap";
	m_KeyNames[VK_SPACE] = "Space";
	m_KeyNames[VK_PRIOR] = "PgUp";
	m_KeyNames[VK_NEXT] = "PgDn";
	m_KeyNames[VK_HOME] = "Home";
	m_KeyNames[VK_LEFT] = "Left";
	m_KeyNames[VK_RIGHT] = "Right";
	m_KeyNames[VK_UP] = "Up";
	m_KeyNames[VK_DOWN] = "Down";
	m_KeyNames[VK_DELETE] = "Del";
	m_KeyNames[0x30] = "0";
	m_KeyNames[0x31] = "1";
	m_KeyNames[0x32] = "2";
	m_KeyNames[0x33] = "3";
	m_KeyNames[0x34] = "4";
	m_KeyNames[0x35] = "5";
	m_KeyNames[0x36] = "6";
	m_KeyNames[0x37] = "7";
	m_KeyNames[0x38] = "8";
	m_KeyNames[0x39] = "9";

	for (char i = 0; i <= 0x5A - 0x41; ++i)
		m_KeyNames[i + 0x41] = 'A' + i;
	
	// I'm to lazy to add the rest
}

UI::Menu::~Menu()
{
	// ImGui destructors and unhook this windows callback function
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));

	// Tell the game to hide the cursor and start reading input again
	SDL_ShowCursor(0);
	*Global::Offsets::grabInput = true;
	*Global::Offsets::shouldInput = true;
	SDL_SetRelativeMouseMode(1);
}

void UI::Menu::Initilize()
{
	// Find window handle to construct ImGui
	HWND hWnd{ NULL };
	while (hWnd == NULL)
	{
		hWnd = FindWindowA(NULL, "Cube 2: Sauerbraten");
	}
	m_hWnd = hWnd;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL2_Init();

	Global::cfg->ReadColors();
	ImGui::SetNextWindowSize(ImVec2{ 1000, 800 }, ImGuiCond_Once);
	// Hook WndProc to let give ImGui input
	originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hWndProc)));
}

void UI::Menu::Render()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool swap{ false };
	if (Global::cfg && Global::cfg->GetState("r_InfoMenu"))
	{
		ImGui::Begin("Info:", static_cast<bool*>(0), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		
		ImGui::BulletText("Pos: %f, %f, %f", Global::Offsets::localPlayer->o.x, Global::Offsets::localPlayer->o.y, Global::Offsets::localPlayer->o.z);
		ImGui::BulletText("Angle: %f. %f. %f", Global::Offsets::localPlayer->yaw, Global::Offsets::localPlayer->pitch, Global::Offsets::localPlayer->roll);
		ImGui::BulletText("Radius: %f, xRadius: %f, yRadius: %f", Global::Offsets::localPlayer->radius, Global::Offsets::localPlayer->xradius, Global::Offsets::localPlayer->yradius);
		
		ImGui::End();
	}

	if (Global::cfg && Global::cfg->GetState("r_Menu"))
	{
		// Show cursor and prevent game from receiving input 
		SDL_ShowCursor(1);
		*Global::Offsets::grabInput = false;
		*Global::Offsets::shouldInput = false;
		SDL_SetRelativeMouseMode(0);

		{
			ImGui::SetNextWindowSize(ImVec2{ 600, 300 }, ImGuiCond_Once);
			ImGui::Begin("Rouladen v1.0 - Slavic Cat", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			const float FooterHeightToReserve{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };
			ImGui::BeginChild("TabRegion", ImVec2(0, -FooterHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

			float total_w{ ImGui::GetContentRegionAvail().x };
			ImGuiContext& g{ *ImGui::GetCurrentContext() };
			ImGuiStyle& style{ ImGui::GetStyle() };
			auto framePadding{ style.FramePadding };

			static float numTabs{ 6.0f };
			static float maxTabWidth{ std::min({total_w / numTabs, g.FontSize * 20.0f}) };
			static float maxTabHeight{ (ImGui::CalcTextSize("Trigger", nullptr, true).y - framePadding.y) / 2.0f };

			ImVec4 inactiveColor{ ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) };
			// No idea why but this centers the tab text
			style.FramePadding.x = maxTabWidth / 3.60f;
			ImGui::BeginTabBar("##TabBar", ImGuiTabBarFlags_::ImGuiTabBarFlags_NoTooltip);
		
			static float aimbotPad{ (maxTabWidth - ImGui::CalcTextSize("Aimbot", nullptr, true).x) / 1.9f };
			style.FramePadding.x = aimbotPad;
			if (ImGui::BeginTabItem("Aimbot"))
			{
				style.FramePadding = framePadding;
				bool bRestoreColor{ false };
				if (!Global::cfg->m_Json["r_Aimbot"]["state"].get<json::boolean_t>())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
					bRestoreColor = true;
				}
				RenderToggle("Aimbot", "r_Aimbot");
				RenderToggle("Fov", "r_Fov");
				RenderToggle("Draw Fov", "r_FovDraw");
				RenderToggle("Silent", "r_Silent");
				RenderToggle("Shoot", "r_AutoShoot");
				RenderToggle("Magic Bullets", "r_MagicBullets");
				ImGui::SliderInt("Aimbot Fov", reinterpret_cast<int*>(&Global::cfg->m_Json["AimbotFov"].get_ref<json::number_integer_t&>()), 0, 180);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
				ImGui::ColorEdit4("Fov Color", reinterpret_cast<float*>(&Global::cfg->m_Vec4["FovColor"]), ImGuiColorEditFlags_NoInputs);
				ImGui::PopStyleVar();
				ImGui::EndTabItem();
				if (bRestoreColor)
					ImGui::PopStyleColor();
			}

			static float triggerPad{ (maxTabWidth - ImGui::CalcTextSize("Trigger", nullptr, true).x) / 1.9f };
			style.FramePadding.x = triggerPad;
			if (ImGui::BeginTabItem("Trigger"))
			{
				style.FramePadding = framePadding;
				RenderToggle("Trigger bot", "r_Trigger");
				ImGui::EndTabItem();
			}

			static float espPad{ (maxTabWidth - ImGui::CalcTextSize("Esp", nullptr, true).x) / 1.9f };
			style.FramePadding.x = espPad;
			if (ImGui::BeginTabItem("Esp"))
			{
				style.FramePadding = framePadding;
				bool bRestoreColor{ false };
				if (!Global::cfg->m_Json["r_Esp"]["state"].get<json::boolean_t>())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
					bRestoreColor = true;
				}
				ImGui::BeginColumns("##esp_col", 2, ImGuiColumnsFlags_NoResize);
				ImGui::SetColumnWidth(1, total_w / 2.0f);
				RenderToggle("Esp", "r_Esp");
				RenderToggle("Boxes", "r_EspBoxes");
				RenderToggle("HealthBars", "r_HealthBar");
				RenderToggle("Names", "r_EspNames");
				ImGui::NextColumn();
				RenderToggle("Enemy Only", "r_EspEnemy");
				RenderToggle("Visible Only", "r_EspVisible");
				ImGui::EndColumns();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
				ImGui::ColorEdit4("Team ESP Color", reinterpret_cast<float*>(&Global::cfg->m_Vec4["TeamColor"]), ImGuiColorEditFlags_NoInputs);
				ImGui::ColorEdit4("Enemy ESP Color", reinterpret_cast<float*>(&Global::cfg->m_Vec4["EnemyColor"]), ImGuiColorEditFlags_NoInputs);
				ImGui::PopStyleVar();
				ImGui::SliderFloat("Esp Width", &Global::cfg->m_Json["EspWidth"].get_ref<float&>(), 0.1f, 20.0f);
				
				ImGui::EndTabItem();

				if (bRestoreColor)
					ImGui::PopStyleColor();
			}

			static float visualsPad{ (maxTabWidth - ImGui::CalcTextSize("Visuals", nullptr, true).x) / 1.9f };
			style.FramePadding.x = visualsPad;
			if (ImGui::BeginTabItem("Visuals"))
			{
				style.FramePadding = framePadding;
				RenderToggle("Enemy Only", "r_EnemyOnly");
				RenderToggle("WallHack", "r_WallHack");
				bool bRestoreCullColor{ false };
				if (!Global::cfg->m_Json["r_WallHack"]["state"].get<json::boolean_t>())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
					bRestoreCullColor = true;
				}
				RenderToggle("No Model Culling", "r_NoCull");
				if (bRestoreCullColor)
					ImGui::PopStyleColor();
				RenderToggle("FullBright", "r_FullBright");

				bool bRestoreBrightColor{ false };
				if (!Global::cfg->m_Json["r_FullBright"]["state"].get<json::boolean_t>())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
					bRestoreBrightColor = true;
				}
				ImGui::SliderFloat("FullBright Level", &Global::cfg->m_Json["FullBright"].get_ref<float&>(), 0.1f, 20.0f);
				if (bRestoreBrightColor)
					ImGui::PopStyleColor();

				RenderToggle("Transparency", "r_Transparent");
				bool bRestoreTransColor{ false };
				if (!Global::cfg->m_Json["r_Transparent"]["state"].get<json::boolean_t>())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
					bRestoreTransColor = true;
				}
				ImGui::SliderFloat("Transparency Level", &Global::cfg->m_Json["Transparent"].get_ref<float&>(), 0.1f, 1.0f);
				if (bRestoreTransColor)
					ImGui::PopStyleColor();
				ImGui::EndTabItem();
			}

			static float miscPad{ (maxTabWidth - ImGui::CalcTextSize("Misc", nullptr, true).x) / 1.9f };
			style.FramePadding.x = miscPad;
			if (ImGui::BeginTabItem("Misc"))
			{
				style.FramePadding = framePadding;
				ImGui::BeginColumns("##misc_col", 2, ImGuiColumnsFlags_NoResize);
				ImGui::SetColumnWidth(1, total_w / 2.0f);
				RenderToggle("Info Menu", "r_InfoMenu");
				RenderToggle("Player List", "r_PlayerList");
				RenderToggle("Debug Console", "r_Console");
				RenderToggle("No Recoil", "r_NoRecoil");
				RenderToggle("No Spread", "r_NoSpread");
				ImGui::NextColumn();
				RenderToggle("Pick Up Items", "r_ItemPickUp");
				RenderToggle("Thirdperson", "r_ThirdPerson");
				RenderToggle("Bunny Hop", "r_BunnyHop");
				RenderToggle("Ignore TeamCheck", "r_IgnoreTeam");
				ImGui::EndColumns();
				ImGui::EndTabItem();
			}

			static float menuPad{ (maxTabWidth - ImGui::CalcTextSize("Menu", nullptr, true).x) };
			style.FramePadding.x = menuPad;
			if (ImGui::BeginTabItem("Menu"))
			{
				style.FramePadding = framePadding;
				static float sliderSize((total_w / 2.0f) * 0.4);
				static bool bShowAllColors{ false };
				if (ImGui::BeginPopupContextWindow())
				{
					ImGui::Text("Colors only:");
					ImGui::Indent();
					if (ImGui::Selectable("Classic Theme")) ImGui::StyleColorsClassic();
					if (ImGui::Selectable("Dark Theme")) ImGui::StyleColorsDark();
					if (ImGui::Selectable("Light Theme")) ImGui::StyleColorsLight();
					ImGui::Unindent();
					if (ImGui::Selectable("Show all Colors")) bShowAllColors = true;
					ImGui::EndPopup();
				}
				if (bShowAllColors)
				{
					static ImGuiTextFilter filter;
					filter.Draw("Filter colors", ImGui::GetFontSize() * 16);
					static float closeOffset{ ImGui::CalcTextSize("Close").x * 1.26f };
					ImGui::SameLine(total_w - closeOffset);
					if (ImGui::Button("Close")) bShowAllColors = false;
					ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);

					ImGui::PushItemWidth(-160);
					for (int i = 0; i < ImGuiCol_COUNT; ++i)
					{
						const char* name = ImGui::GetStyleColorName(i);
						if (!filter.PassFilter(name))
							continue;
						ImGui::PushID(i);
						ImGui::ColorEdit4("##color", reinterpret_cast<float*>(&style.Colors[i]), ImGuiColorEditFlags_AlphaBar);
						ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
						ImGui::TextUnformatted(name);
						ImGui::PopID();
					}
					ImGui::PopItemWidth();
					ImGui::EndChild();
				}
				else
				{
					ImGui::BeginColumns("##menu_col", 2, ImGuiColumnsFlags_NoResize);
					ImGui::SetColumnWidth(1, total_w / 2.0f);
					ImGui::PushItemWidth(sliderSize);
					ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
					ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
					ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
					ImGui::ColorEdit4("Menu Color", reinterpret_cast<float*>(&style.Colors[ImGuiCol_TitleBg]), ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4("Active Color", reinterpret_cast<float*>(&style.Colors[ImGuiCol_TitleBgActive]), ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4("Hovered Color", reinterpret_cast<float*>(&style.Colors[ImGuiCol_HeaderHovered]), ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4("Background Color", reinterpret_cast<float*>(&style.Colors[ImGuiCol_WindowBg]), ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4("Item Background Color", reinterpret_cast<float*>(&style.Colors[ImGuiCol_FrameBg]), ImGuiColorEditFlags_NoInputs);
					ImGui::PopStyleVar();
					ImGui::PopItemWidth();
					style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_TitleBg];
					style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_TitleBgActive];
					style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_Header];
					style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
					style.Colors[ImGuiCol_TabActive] = style.Colors[ImGuiCol_HeaderActive];
					ImGui::NextColumn();
					ImGui::PushItemWidth(sliderSize);
					ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
					ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
					ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
					ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
					ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
					ImGui::PopItemWidth();
					ImGui::EndColumns();

				}
				ImGui::EndTabItem();
			}
			style.FramePadding = framePadding;
			ImGui::EndTabBar();
			ImGui::EndChild();
			ImGui::Separator();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			auto t{ std::time(nullptr) };
			std::tm bt{};
			localtime_s(&bt, &t);
			char timeStamp[100];
			asctime_s(timeStamp, &bt);
			static float timeSize{ ImGui::CalcTextSize(timeStamp).x };
			ImGui::SameLine(total_w - timeSize);
			ImGui::Text(timeStamp);

			ImGui::End();

		}

		if (Global::cfg->GetState("r_PlayerList"))
		{
			static ImVec4 enemyColor{ 0.741f, 0.0f, 0.0f, 1.0f };
			static ImVec4 teamColor{ 0.501f, 0.501f, 0.501f, 1.0f };

			ImGui::Begin("PlayerList", static_cast<bool*>(0), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
			if (Global::Offsets::entList->length() > 1)
			{
				if (ImGui::BeginTable("List", 3))
				{
					for (int row = 1; row < Global::Offsets::entList->length(); ++row)
					{
						Cube::fpsent* e{ Global::Offsets::entList->buf[row] };
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (IsEnemy(e))
							ImGui::TextColored(enemyColor, "%d. %s%s", row, (row < 10 ? " " : ""), e->name);
						else
							ImGui::TextColored(teamColor, "%d. %s%s", row, (row < 10 ? " " : ""), e->name);

						ImGui::TableSetColumnIndex(1);
						ImGui::Text("Health: %d", e->health);
						ImGui::TableSetColumnIndex(2);
						std::string btnName{ "Debug Info##" };
						btnName.append(std::to_string(row));
						static ImVec2 btnSize{ ImGui::CalcTextSize(btnName.c_str(), nullptr, true).x * 1.1f, ImGui::CalcTextSize(btnName.c_str(), nullptr, true).y * 1.2f };
						if (ImGui::SmallButtonSized(btnName.c_str(), btnSize))
						{
							std::stringstream format;
							format << "Name: " << e->name << ", id: " << e->clientnum << ", info: " << e->info;
							Global::consoleQueue.emplace_back(format.str());
						}

					}
					ImGui::EndTable();
				}
			}
			else
				ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.8f), "No other players");
			ImGui::End();
		}
		if (!Global::consoleQueue.empty())
		{
			for (auto& text : Global::consoleQueue)
			{
				m_DebugConsole.AddLog(text.data());
			}
			Global::consoleQueue.clear();
		}
		if (Global::cfg->GetState("r_Console"))
		{
			m_DebugConsole.Draw("Debug Console");
		}
		// Returning early as letting both paths merge has caused problems in the past
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		swap = true;
		return;
	}
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	if (swap)
	{
		//restore input
		swap = false;
		SDL_ShowCursor(0);

		*Global::Offsets::grabInput = true;
		*Global::Offsets::shouldInput = true;
		SDL_SetRelativeMouseMode(1);
	}

	return;

}

void UI::Menu::RenderToggle(const std::string& name, const std::string& id)
{
	// For right alignment
	static ImVec2 toggleSize(58.0f, 16.0f);
	float total_w{ ImGui::GetContentRegionAvail().x };

	// Shrink checkbox size
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
	ImGui::Checkbox(name.c_str(), &Global::cfg->m_Json[id.c_str()]["state"].get_ref<json::boolean_t&>());

	// Get the keystate, if we are changing the bind render "..."
	std::string keyName{ GetKeyName(Global::cfg->m_Json[id.c_str()]["key"].get<json::number_integer_t>()) };
	if (!id.compare(Global::cfg->m_BindName))
		keyName = "...";
	// Get Width
	static ImVec2 bindWidth{ ImGui::CalcTextSize("Cancel").x, ImGui::CalcTextSize("Cancel").y + 1.5f };

	keyName.append("##");
	keyName.append(id);
	ImGui::SameLine(total_w - toggleSize.x - bindWidth.x * 1.2f);
	ImGui::SetNextItemWidth(total_w);
	if (ImGui::SmallButtonSized(keyName.c_str(), bindWidth))
	{
		Global::cfg->m_BindName = id;
		Global::cfg->m_ReadInput = true;
	}
	bool bOnHold{ Global::cfg->m_Json[id.c_str()]["on_hold"].get<json::boolean_t>() };
	std::string stateName{ (bOnHold ? "On Hold##" : "Toggle##") };
	stateName.append(id);
	
	ImGui::SameLine(total_w - toggleSize.x);
	ImGui::SetNextItemWidth(total_w);
	if (ImGui::SmallButtonSized(stateName.c_str(), toggleSize))
	{
		Global::cfg->ToggleMode(id);
	}
	ImGui::PopStyleVar();
}

const std::string& UI::Menu::GetKeyName(int64_t index) const
{
	// Doesn't let me use [] so .at() instead???
	if (m_KeyNames.find(index) == m_KeyNames.end())
		return m_KeyNames.at(-2);
	return m_KeyNames.at(index);
}

// Extension of ImGui::SmallButton which allows for a set size to be given
// Should be placed in custom.h but msvc complains about namespacing for some dumb reason
bool ImGui::SmallButtonSized(const char* label, const ImVec2& size)
{
	ImGuiContext& g = *GImGui;
	float backup_padding_y = g.Style.FramePadding.y;
	g.Style.FramePadding.y = 0.0f;
	bool pressed = ButtonEx(label, size, ImGuiButtonFlags_AlignTextBaseLine);
	g.Style.FramePadding.y = backup_padding_y;
	return pressed;
}

bool UI::Console::SaveLog()
{
	if (m_Path.empty())
	{
		WCHAR* pszPath{ nullptr };
		if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &pszPath) == S_OK)
		{
			std::filesystem::path test{ pszPath };

			test.append("Rouladen\\Logs");
			m_Path = test;
		}
		else
		{
			Global::consoleQueue.emplace_back("[Error] Could not generate AppData Path");
			return false;
		}
	}

	std::stringstream format{};
	auto t{ std::time(nullptr) };
	std::tm bt{};
	localtime_s(&bt, &t);

	format << m_Path.string() << '\\' << std::put_time(&bt, "%d-%m-%Y %H-%M-%S") << ".txt";

	std::ofstream file;
		
	file.open(format.str(), std::ofstream::out | std::ofstream::trunc);

	if (file.is_open())
	{
		for (auto& line : m_Items)
		{
			file << line << '\n';
		}
	}
	else
	{
		std::filesystem::create_directories(m_Path);
		file.open(format.str(), std::ofstream::out | std::ofstream::trunc);

		if (file.is_open())
		{
			for (auto& line : m_Items)
			{
				file << line << '\n';
			}
		}
		else
		{
			std::stringstream errFormat{};
			errFormat << "[Error] Could not generate log " << format.str();
			Global::consoleQueue.emplace_back(errFormat.str());
			return false;
		}
			
	}
	file.close();
	return true;
}

void UI::Console::Draw(const std::string& title)
{
	ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginPopup("Options"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	if (ImGui::SmallButton("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();

	if (ImGui::SmallButton("Clear")) { ClearLog(); }
	ImGui::SameLine();
	static std::chrono::system_clock::time_point saveTime{};
	static std::string saveMessage{};
	static ImVec4 saveColor{};
	if (ImGui::SmallButton("Save to file")) 
	{ 
		saveTime = std::chrono::system_clock::now();
		if (SaveLog())
		{
			saveMessage = "Saved to Rouladen/Logs";
			saveColor = ImVec4(0.078f, 0.878f, 0.0f, 1.0f);
		}
		else
		{
			saveMessage = "Failed to save";
			saveColor = ImVec4(0.741f, 0.0f, 0.0f, 1.0f);
		}
	}
	double timeDiff{ std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - saveTime).count() };
	constexpr double displayTime{ 2500.0 };
	if (timeDiff < displayTime)
	{
		ImGui::SameLine();
		saveColor.w = static_cast<float>((displayTime - timeDiff) / displayTime);
		ImGui::TextColored(saveColor, saveMessage.c_str());
	}	

	ImGui::Separator();

	const float FooterHeightToReserve{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -FooterHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		if (ImGui::Selectable("Scroll to Bottom")) m_ScrollToBottom = true;
		ImGui::EndPopup();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (int i = 0; i < m_Items.size(); i++)
	{
		const char* item = m_Items[i];

		// Normally you would store more information in your item than just a string.
		// (e.g. make Items[] an array of structure, store color/type etc.)
		ImVec4 color;
		if (strstr(item, "[Error]")) 
		{ 
			color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();
		}
		else
			ImGui::TextUnformatted(item);

	}
	if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	m_ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}
