#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//#ifndef PROXI_SELF_DEBUG
//#define PROXI_SELF_DEBUG
//#endif

#include "ui.hh"

#include <algorithm>
#include <nlohmann/json.hpp>

#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../socket/msoket.h"
#include "../socket/packet/Packet.h"

using nlohmann::json;

void imspaceMacro(float x, float y) {
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + x, ImGui::GetCursorPos().y + y));
}

bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
}

float linearIntepolate(float currentLocation, float Goal, float time) {

	return (1 - time) * currentLocation + time * Goal;
}

float TKlerp(float a, float b, float f)
{
	float goat = 0;
	goat = a + f * (b - a);

	if(a > b)
		goat = std::clamp(goat, b, a);
	else
		goat = std::clamp(goat, a, b);

	return goat;
}

void ui::renderDebugWindow()
{
	ImGui::SetNextWindowSize(ImVec2(1920, 300));
	ImGui::SetNextWindowPos(ImVec2(0, 50));
	ImGui::SetNextWindowBgAlpha(0.6f);

	ImGui::Begin("imgui debug window");
	{
		if (ImGui::Button("CLEAR LIST", ImVec2(100, 40)))
		{
			mSocket::cfg::debugLogList.clear();
		}

		for (auto item : mSocket::cfg::debugLogList)
		{
			imspaceMacro(30, 6);
			ImGui::Text(item.c_str());
		}
	}
	ImGui::End();
}

void ui::render() {
#ifdef	PROXI_SELF_DEBUG
	renderDebugWindow();
#endif

    if (!globals.active) return;

	static float go = 0;

	std::string ttext = "";
	static float pb = 0;

	static float elapsedTime = 0;

	elapsedTime += ImGui::GetIO().DeltaTime;

	float pC = elapsedTime / 0.3f;

	int ststeT = (int)mSocket::cfg::logging_in_successfully;

    if (!mSocket::cfg::socketIsConnected || mSocket::cfg::socketNeedProxiAuth)
    {
		elapsedTime = 0;
		go = 0;
    }
	else
    {
	    if(ststeT == 0)
	    {
			elapsedTime = 0;
			go = -400;
	    }
		else if (ststeT == 1)
		{
			elapsedTime = 0;
			go = -800;
		}
    }

	ImGuiStyle& imguiStyles = ImGui::GetStyle();
	{
		auto getZtkColor__inside_of_bload = [](int red, int green, int blue, int alpha = 255)
		{
			return ImVec4((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, (float)alpha / 255.0f);
		};

		imguiStyles.Colors[ImGuiCol_WindowBg] = getZtkColor__inside_of_bload(20, 20, 20, 255);

		imguiStyles.Colors[ImGuiCol_Button] = getZtkColor__inside_of_bload(40, 40, 40);
		imguiStyles.Colors[ImGuiCol_ButtonHovered] = getZtkColor__inside_of_bload(35, 120, 30);
		imguiStyles.Colors[ImGuiCol_ButtonActive] = getZtkColor__inside_of_bload(35, 90, 30);


		imguiStyles.Colors[ImGuiCol_CheckMark] = getZtkColor__inside_of_bload(30, 220, 30);

		imguiStyles.Colors[ImGuiCol_FrameBg] = getZtkColor__inside_of_bload(40, 40, 40);
		imguiStyles.Colors[ImGuiCol_FrameBgHovered] = getZtkColor__inside_of_bload(50, 50, 50);
		imguiStyles.Colors[ImGuiCol_FrameBgActive] = getZtkColor__inside_of_bload(50, 110, 50);



		imguiStyles.Colors[ImGuiCol_Header] = getZtkColor__inside_of_bload(50, 180, 50);
		imguiStyles.Colors[ImGuiCol_HeaderHovered] = getZtkColor__inside_of_bload(35, 120, 30);
		imguiStyles.Colors[ImGuiCol_HeaderActive] = getZtkColor__inside_of_bload(35, 90, 30);


		imguiStyles.Colors[ImGuiCol_SliderGrab] = getZtkColor__inside_of_bload(30, 220, 30);
		imguiStyles.Colors[ImGuiCol_SliderGrabActive] = getZtkColor__inside_of_bload(30, 220, 30);
	}


	pb = TKlerp(pb, go, linearIntepolate(0, 1, pC));

    
	ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(1.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.0f);
	{
		ImGui::Begin(window_title, &globals.active, window_flags);
	    {
			imspaceMacro(pb, 0);
			ImGui::BeginChild("#connecting_state1", window_size, false);
			{
				ImGui::PushFont(ui::font20x);
				{
					std::string tText = "Connecting to server";
					ImVec2 tSize = ImGui::CalcTextSize(tText.c_str());

					imspaceMacro(window_size.x / 2 - 30, (window_size.y / 2) - ((15 + tSize.y + (30 * 2)) / 2));
					Spinner("##connection_spinner", 28, 4, ImColor(40, 130, 40));
				

					imspaceMacro((window_size.x / 2) - (tSize.x / 2), 15);
					ImGui::Text(tText.c_str());
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("#connecting_state2", window_size);
			{
				if (mSocket::cfg::socketIsConnected && ststeT == 0)
				{
					if (mSocket::cfg::logging_in_hwid)
					{
						std::string tText = "Checking your hwid...";
						ImVec2 tSize = ImGui::CalcTextSize(tText.c_str());

						imspaceMacro(window_size.x / 2 - 30, (window_size.y / 2) - ((15 + tSize.y + (30 * 2)) / 2));
						Spinner("##loggingin_spinner", 28, 4, ImColor(40, 130, 40));



						imspaceMacro((window_size.x / 2) - (tSize.x / 2), 15);
						ImGui::Text(tText.c_str());
					}
					else if (mSocket::cfg::logging_in)
					{
						std::string tText = "Loggin in to your account";
						ImVec2 tSize = ImGui::CalcTextSize(tText.c_str());

						imspaceMacro(window_size.x / 2 - 30, (window_size.y / 2) - ((15 + tSize.y + (30 * 2)) / 2));
						Spinner("##loggingin_spinner", 28, 4, ImColor(40, 130, 40));



						imspaceMacro((window_size.x / 2) - (tSize.x / 2), 15);
						ImGui::Text(tText.c_str());
					}
					else
					{
						ImGui::PushItemWidth(window_size.x - 100);
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.6f);
							{
								if (mSocket::cfg::logging_in_err == "")
								{

									ImGui::PushFont(ui::font40x);
									{
										std::string tText = "PROXIMITY";
										ImVec2 tSize = ImGui::CalcTextSize(tText.c_str());
										imspaceMacro(window_size.x / 2 - tSize.x / 2, 30);
										ImGui::Text(tText.c_str());
									}
									ImGui::PopFont();

									ImGui::PushFont(ui::font18x);
									{
										imspaceMacro(50, window_size.y - 274);
										ImGui::Text("Username:");
										imspaceMacro(50, 8);
									}
									ImGui::PopFont();

									ImGui::PushFont(ui::font16x);
									{
										ImGui::InputText("##username_input", mSocket::cfg::logging_in_username, IM_ARRAYSIZE(mSocket::cfg::logging_in_username));
									}
									ImGui::PopFont();

									ImGui::PushFont(ui::font18x);
									{
										imspaceMacro(50, 20);
										ImGui::Text("Password:");
										imspaceMacro(50, 8);
									}
									ImGui::PopFont();

									ImGui::PushFont(ui::font16x);
									{
										ImGui::InputText("##password_input", mSocket::cfg::logging_in_password, IM_ARRAYSIZE(mSocket::cfg::logging_in_password), ImGuiInputTextFlags_Password);
									}
									ImGui::PopFont();


									ImGui::PushFont(ui::font16x);
									{
										imspaceMacro(window_size.x - 150, 20);
										
										if (ImGui::Button("Login", ImVec2(100, 40)))
										{
											if (std::string(mSocket::cfg::logging_in_username) == "" || std::string(mSocket::cfg::logging_in_password) == "")
											{
												mSocket::cfg::logging_in = false;
												mSocket::cfg::logging_in_err = "Nice iq. (Blank Input)";
											}
											else
											{
												mSocket::cfg::logging_in = true;

												std::string cHwidErr = "";
												std::string cHwid = "";
												if (!mSocket::getHWID(&cHwidErr, &cHwid))
												{
													mSocket::cfg::logging_in_err = cHwidErr;

													MessageBoxA(0, "Hwid Generator Error", "HWID PARSER", 0);
													mSocket::cleanup();
													exit(0);
												}
												else
												{
													json loginJsonData = json();

													loginJsonData["who_i_am"] = "loader";
													loginJsonData["packet_id"] = Packets::NClientPackets::EFromClientToServer::USER_AUTH;
													loginJsonData["data"]["username"] = std::string(mSocket::cfg::logging_in_username);
													loginJsonData["data"]["password"] = std::string(mSocket::cfg::logging_in_password);
													loginJsonData["data"]["hwid"] = cHwid;

													std::string sendLon = loginJsonData.dump();

													const char* sError = "";
													if (!mSocket::sendPacketToServer(sendLon.c_str(), &sError))
													{
														mSocket::cfg::logging_in = false;
														mSocket::cfg::logging_in_err = sError;

														MessageBoxA(0, "Server Packing Error", "SERVER ERROR", 0);
														mSocket::cleanup();
														exit(0);
													}
													else
													{
														
													}
												}
											}
										}
									}
									ImGui::PopFont();

								}
								else
								{
									ImVec2 tSize = ImGui::CalcTextSize(mSocket::cfg::logging_in_err.c_str());

									imspaceMacro((window_size.x / 2) - (tSize.x / 2), (window_size.y / 2) - ((tSize.y + 40) / 2));

									ImGui::Text(mSocket::cfg::logging_in_err.c_str());

									imspaceMacro((window_size.x / 2) - (100 / 2), 100);
									if (ImGui::Button("Exit", ImVec2(100, 40)))
									{
										mSocket::cleanup();
										exit(0);
									}
								}
							}
							ImGui::PopStyleVar();
							ImGui::PopStyleVar();
						}
						ImGui::PopItemWidth();
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("#connecting_state3", window_size);
			{
				if (mSocket::cfg::socketIsConnected && ststeT == 1 && mSocket::cfg::logging_in_successfully)
				{
					ImGui::PushFont(ui::font14x);
					{
						imspaceMacro(10, 10);
						ImGui::Text(std::string(std::string("Welcome: ") + mSocket::cfg::success_cheat_user).c_str());
						imspaceMacro(10, 3);
						ImGui::Text(std::string(std::string("Till: ") + mSocket::cfg::success_cheat_till).c_str());
					} 
					ImGui::PopFont();


					if (mSocket::cfg::loading_cheat_state == "")
					{
						imspaceMacro((window_size.x / 2) - (100 / 2), ((window_size.y - 41) / 2) - (50 / 2));

						if (ImGui::Button("Load", ImVec2(100, 50)))
						{
							mSocket::cfg::_____jskjensb = true;
							mSocket::cfg::loading_cheat_state = "Please Wait";
						}
					}
					else
					{ 
						ImVec2 tSize = ImGui::CalcTextSize(mSocket::cfg::loading_cheat_state.c_str());
						imspaceMacro((window_size.x / 2) - (tSize.x / 2), ((window_size.y - 41) / 2) - (tSize.y / 2));
						ImGui::Text(mSocket::cfg::loading_cheat_state.c_str());
					}
					
				}
			}
			ImGui::EndChild();
	    }
    	ImGui::End();
	}

	ImGui::PopStyleVar();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
    // colors
    ImGui::StyleColorsDark();
    ImGuiStyle& styles = ImGui::GetStyle();


    {
		styles.WindowBorderSize = 3;
        styles.WindowPadding = ImVec2(0, 0);
        styles.ChildBorderSize = 3;
        styles.FramePadding = ImVec2(0, 0);
        styles.CellPadding = ImVec2(0, 0);
        styles.ItemInnerSpacing = ImVec2(0, 0);
        styles.ItemSpacing = ImVec2(0, 0);
    }
	
	


	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;

		// init images here
	}
}