#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Main.h"
#include "ui/ui.hh"
#include "globals.hh"
#include "socket/msoket.h"
#include "vac-bypass/vbypass.h"

// Main code
int APIENTRY WinMain(HINSTANCE, HMODULE hModule, LPSTR, int)
{

    std::string errstr = "";
    std::string resstr = "";

    if (!mSocket::getHWID(&errstr, &resstr))
    {
        MessageBoxA(0, errstr.c_str(), "", 0);
        exit(0);
    }
    

    static bool setIn�tError = false;
    const char* sError = "";

    if (!mSocket::initSoket(&sError))
    {
        MessageBoxA(0, "Connection was corrupted...", "Connection error", 0);
        exit(0);
    }

    mSocket::cfg::socketThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)mSocket::socketThread, 0, 0, 0);

    CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)BypassLoader::LoaderLoop, hModule, 0, 0));

    //mSocket::cfg::_____jskjensb = true;

    if (setIn�tError)
    {
        mSocket::cleanup();
        MessageBoxA(nullptr, "LOADER INIT ERROR\NTRY AGAIN OR CREATE TICKET ON DISCORD", "ERROR", 0);
        exit(0);
    }

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ui::window_title, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, ui::window_title, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 10;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    static const ImWchar icons_ranges_Turkish[] = {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x00c7, 0x00c7, // �
            0x00e7, 0x00e7, // �
            0x011e, 0x011e, // �
            0x011f, 0x011f, // �
            0x0130, 0x0130,// �
            0x0131, 0x0131, // �
            0x00d6, 0x00d6, // �
            0x00f6, 0x00f6, // �
            0x015e, 0x015e, // �
            0x015f, 0x015f, // �
            0x00dc, 0x00dc, // �
            0x00fc, 0x00fc, // �

            0, };

    ui::font14x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 14, 0, icons_ranges_Turkish);
    ui::font16x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 16, 0, icons_ranges_Turkish);
    ui::font18x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 18, 0, icons_ranges_Turkish);
    ui::font20x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 20, 0, icons_ranges_Turkish);
    ui::font30x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 30, 0, icons_ranges_Turkish);
    ui::font40x = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Verdana.ttf", 40, 0, icons_ranges_Turkish);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            static int init = false;
            if (!init) {
                ui::init(g_pd3dDevice);
                init = true;
            }
            else {
                ui::render();
            }
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!globals.active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}