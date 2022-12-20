#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma once
#include <d3d9.h>
#include "../imgui/imgui.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

namespace ui {

	inline IDirect3DTexture9* imguiLoaderGIF[60] = {}; // creating the texture

	// Inside of your createtexture do this


	inline LPDIRECT3DDEVICE9 dev;
	inline const char* window_title = "Loader base";

	inline ImFont* font20x = nullptr;
	inline ImFont* font30x = nullptr;
	inline ImFont* font40x = nullptr;

	inline ImVec2 screen_res{ 000, 000 };
	inline ImVec2 window_pos{ 0, 0 };
	inline ImVec2 window_size{ 400, 300 };
	inline DWORD  window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;


	void init(LPDIRECT3DDEVICE9);
	void render();
}