#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "ui.hh"

#include <algorithm>

#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../socket/msoket.h"

#include "stb_include.h"
#include <stb_image.h>
#include <stb_image_write.h>

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
 
void ui::render() {
    if (!globals.active) return;

	static float go = 0;

	std::string ttext = "";
	static float pb = 0;

	static float elapsedTime = 0;

	elapsedTime += ImGui::GetIO().DeltaTime;

	float pC = elapsedTime / 0.3f;

	static int ststeT = 0;

    if (!mSocket::cfg::socketIsConnected)
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




	pb = TKlerp(pb, go, linearIntepolate(0, 1, pC));

    
	ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin(window_title, &globals.active, window_flags);
    {
		imspaceMacro(pb, 0);
		ImGui::BeginChild("#connecting_state1", window_size, false);
		{
			ImGui::PushFont(ui::font20x);
			{
				imspaceMacro(window_size.x / 2 - 38, window_size.y / 2 - 38);
				Spinner("123", 36, 4, ImColor(40, 130, 40));

				std::string tText = "Connecting to server";
				ImVec2 tSize = ImGui::CalcTextSize(tText.c_str());

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
				ImGui::Text("222");
				if (ImGui::Button("click", ImVec2(100, 50)))
				{
					
				}
			}
		}
		ImGui::EndChild();

    	ImGui::SameLine();

		ImGui::BeginChild("#connecting_state3", window_size);
		{
			if (mSocket::cfg::socketIsConnected && ststeT == 1)
			{
				ImGui::Text("333");
				if (ImGui::Button("click", ImVec2(100, 50)))
				{
					ststeT = 0;
				}
			}
		}
		ImGui::EndChild();
    }
    ImGui::End();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
    // colors
    ImGui::StyleColorsDark();
    ImGuiStyle& styles = ImGui::GetStyle();


    {
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