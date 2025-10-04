#define IMGUI_DEFINE_MATH_OPERATORS
#include <thread>
#include <filesystem>

#include "design/utils/overlay.h"
#include "design/key/keybind.h"
#include "design/imgui/imgui_impl_win32.h"
#include "design/imgui/imgui_impl_dx11.h"
#include "design/utils/xorstr.hpp"
#include "../vars/vars.h"

#include <dwmapi.h>
#include <map>

ID3D11Device* overlay::d3d11_device = nullptr;
ID3D11DeviceContext* overlay::d3d11_device_context = nullptr;
IDXGISwapChain* overlay::dxgi_swap_chain = nullptr;
ID3D11RenderTargetView* overlay::d3d11_render_target_view = nullptr;
bool overlay::fullsc(HWND windowHandle)
{
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	if (GetMonitorInfo(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
	{
		RECT windowRect;
		if (GetWindowRect(windowHandle, &windowRect))
		{
			return windowRect.left == monitorInfo.rcMonitor.left
				&& windowRect.right == monitorInfo.rcMonitor.right
				&& windowRect.top == monitorInfo.rcMonitor.top
				&& windowRect.bottom == monitorInfo.rcMonitor.bottom;
		}
	}
}


void DrawExplorerNode(const Structs::Instance::InstanceManager& instance) {
	if (!instance.isValid()) return;

	std::string label = instance.GetName() + " [" + instance.GetClass() + "]";


	bool opened = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

	if (opened) {
		auto children = instance.GetChildren(true);
		for (const auto& child : children) {
			DrawExplorerNode(child);
		}

		ImGui::TreePop();
	}

}





#include <atomic>
#include <mutex>
#include <string>
#include "../functions/esp/esp.h"
#include "design/fonts/IconsFontAwesome6.h"
#include "design/fonts/FontAwesomeFont.h"

static bool showTabA = false;
static bool showTabB = false;
static bool showTabC = false;
static bool showTabD = false;


bool Keybind65(CKeybind* bind, float custom_width)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(bind->get_name().c_str());
	char display[64] = "None";
	ImVec2 pos = window->DC.CursorPos + ImVec2(15.0f, 0); // move to the right
	ImVec2 display_size = ImGui::CalcTextSize(display, NULL, true);
	float width = custom_width == 0 ? 80.0f : custom_width; // smaller default width
	float height = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
	ImVec2 size(width, height);
	ImRect frame_bb(pos, pos + size);
	ImRect total_bb(pos, frame_bb.Max + ImVec2(size.x, 0));

	ImGui::ItemSize(total_bb);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered = ImGui::ItemHoverable(frame_bb, id, false);
	if (hovered)
	{
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_Hand;
	}

	bool pressed = hovered && io.MouseClicked[0];
	bool right_click = hovered && io.MouseClicked[1];
	bool released_elsewhere = io.MouseClicked[0] && !hovered;

	if (pressed)
	{
		if (g.ActiveId != id)
		{
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			bind->key = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
		bind->waiting_for_input = true;
	}
	else if (released_elsewhere && g.ActiveId == id)
	{
		ImGui::ClearActiveID();
		bind->waiting_for_input = false;
	}
	else if (right_click)
		ImGui::OpenPopup(bind->get_name().c_str());

	bool changed = false;
	int key = bind->key;

	if (bind->waiting_for_input && g.ActiveId == id)
	{
		if (io.MouseClicked[0] && !hovered)
		{
			key = VK_LBUTTON;
			bind->waiting_for_input = false;
			ImGui::ClearActiveID();
			changed = true;
		}
		else
		{
			if (bind->set_key())
			{
				key = bind->key;
				bind->waiting_for_input = false;
				ImGui::ClearActiveID();
				changed = true;
			}
		}
	}

	bind->key = key;

	if (bind->waiting_for_input)
		strcpy_s(display, sizeof display, "...");
	else if (bind->key != 0)
		strcpy_s(display, sizeof display, bind->get_key_name().c_str());
	else
		strcpy_s(display, sizeof display, "None");

	window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_ChildBg), style.FrameRounding); // match child background
	if (style.FrameBorderSize > 0.0f)
	{
		window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, style.FrameBorderSize);
	}

	ImGui::RenderNavHighlight(frame_bb, id);

	ImVec2 text_pos = frame_bb.Min + (frame_bb.Max - frame_bb.Min) * 0.5f - ImGui::CalcTextSize(display, NULL, true) * 0.5f;
	window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), display);

	if (ImGui::BeginPopup(bind->get_name().c_str()))
	{
		if (ImGui::Selectable("Hold", bind->type == CKeybind::HOLD))
			bind->type = CKeybind::HOLD;
		if (ImGui::Selectable("Always", bind->type == CKeybind::ALWAYS))
			bind->type = CKeybind::ALWAYS;
		if (ImGui::Selectable("Toggle", bind->type == CKeybind::TOGGLE))
			bind->type = CKeybind::TOGGLE;
		ImGui::EndPopup();
	}
	return changed;
}

bool overlay::render() {
	static bool priority_set = false;
	if (!priority_set) {
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		priority_set = true;
	}
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = nullptr;

	wc.lpszClassName = TEXT("jerkingoff");
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		DWORD error = GetLastError();
		char errMsg[128];
		sprintf_s(errMsg, "RegisterClassEx failed: GetLastError = %lu", error);
		MessageBoxA(nullptr, errMsg, "error", MB_OK | MB_ICONERROR);
		return false;
	}

	const HWND hw = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
		wc.lpszClassName,
		TEXT("kdswepkjgklegwjkegw"),
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (!hw) {
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
	const MARGINS margin = { -1 };
	if (FAILED(DwmExtendFrameIntoClientArea(hw, &margin))) {
		DestroyWindow(hw);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	if (!create_device_d3d(hw)) {
		cleanup_device_d3d();
		DestroyWindow(hw);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	ShowWindow(hw, SW_SHOW);
	UpdateWindow(hw);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui::GetIO().IniFilename = nullptr;
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hw);
	ImGui_ImplDX11_Init(d3d11_device, d3d11_device_context);

	ImFontConfig config;
	config.MergeMode = true;
	config.PixelSnapH = true;


	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = 12.f * 2.0f / 3.0f;
	io.Fonts->AddFontDefault();
	ImFont* 
	icons_font = io.Fonts->AddFontFromMemoryTTF(FontAwesomeFont, sizeof(FontAwesomeFont), 12.f, &icons_config, icons_ranges);


	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Did you forget to call io.Fonts->Build() or ImGui_ImplDX11_CreateFontsTexture()?");
	const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	io.Fonts->AddFontDefault();

	bool draw = true;
	bool done = false;
	ImGuiIO& i2342432o = ImGui::GetIO();



	while (!done) {
		static bool priority_setA = false;
		if (!priority_setA) {
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
			priority_setA = true;
		}

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				done = true;
			}
		}

		if (done)
			break;

		move_window(hw);

		if (GetAsyncKeyState(VK_F1) & 0x1) {

			draw = !draw;

		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();



		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::GetIO().LogFilename = NULL;

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;



	

		if (GetForegroundWindow() == FindWindowA(0, XorStr("Roblox")) || GetForegroundWindow() == hw) {

			ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ 0, 0 }, ImGuiCond_Always);

			if (ImGui::Begin(XorStr("ESP"), nullptr,
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoInputs |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoNav |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoMouseInputs)) {

				static bool priority_setBD = false;
				if (!priority_setBD) {
					::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
					priority_setBD = true;
				}

				visuals::init::Load_1();  

				ImGui::End();
			}
		}


		if (draw) {
			if (ImGui::Begin("##NavigationBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar)) {
				ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowSize().x / 2, 10));

				if (ImGui::Button("M " ICON_FA_MEDAL)) showTabA = !showTabA;
				ImGui::SameLine();
				if (ImGui::Button("L")) showTabB = !showTabB;
				ImGui::SameLine();
				if (ImGui::Button("S")) showTabC = !showTabC;
				ImGui::SameLine();
				if (ImGui::Button("C")) showTabD = !showTabD;
				ImGui::SameLine();

				ImGui::End();
			}

			if (draw) {
				ImGui::Begin("MENU");
				if (ImGui::BeginTabBar("Tabs")) {
					if (ImGui::BeginTabItem("Aimbot " ICON_FA_GUN)) {
						ImGui::Checkbox("Enabled", &vars::Aimbot::enabled);
						ImGui::SameLine();
						Keybind65(&vars::Aimbot::aimbot_bind, 80.0f);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Visuals " ICON_FA_EYE)) {
						ImGui::Checkbox("Enabled", &vars::Visuals::masterswitch);

						ImGui::Separator();

						ImGui::Checkbox("Box ESP", &vars::Visuals::Box);
						ImGui::SameLine();
						ImGui::ColorEdit4("##BoxColor", vars::Visuals::Colors::BoxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						ImGui::SameLine();
						ImGui::ColorEdit4("##BoxOutline", vars::Visuals::Colors::BoxOutlineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

						ImGui::Checkbox("Box Fill", &vars::Visuals::Box_fill);

						ImGui::Checkbox("Name", &vars::Visuals::Name);
						ImGui::SameLine();
						ImGui::ColorEdit4("##NameColor", vars::Visuals::Colors::NameColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);


						ImGui::Checkbox("chams", &vars::Visuals::Chams);
						ImGui::SameLine();
						ImGui::ColorEdit4("##chamsColor", vars::Visuals::Colors::ChamsColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

						ImGui::Checkbox("Distance", &vars::Visuals::Distance);
						ImGui::SameLine();
						ImGui::ColorEdit3("##DistanceColor", vars::Visuals::Colors::distance_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						ImGui::Checkbox("Movement Status", &vars::Visuals::MovementStatus);
						ImGui::SameLine();
						ImGui::ColorEdit3("##FlagsColor", vars::Visuals::Colors::flags, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

						ImGui::Separator();

						ImGui::Checkbox("Box Gradient", &vars::Visuals::Box_gradient);
						ImGui::SameLine();
						ImGui::ColorEdit4("##GradientTop", vars::Visuals::Colors::BoxGradientTop, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						ImGui::SameLine();
						ImGui::ColorEdit4("##GradientBottom", vars::Visuals::Colors::BoxGradientBottom, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

						ImGui::Checkbox("Auto Rotate", &vars::Visuals::Box_gradient_autorotate);
						ImGui::SliderFloat("Rotation Speed", &vars::Visuals::Box_gradient_rotation_speed, 0.1f, 5.f, "%.1f");

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Lighting " ICON_FA_SAILBOAT)) {
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Exploits " ICON_FA_EXPLOSION)) {

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Explorer " ICON_FA_DUMPSTER)) {
						if (vars::General::datamodel.isValid()) {
							DrawExplorerNode(vars::General::datamodel);
						}
						else {
							ImGui::TextDisabled("DataModel not found or invalid.");
						}

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Settings " ICON_FA_WRENCH)) {
						ImGui::SliderInt("Player Cache Delay", &vars::cache::PlayerCacheDelay, 33, 1000);
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::End();
			}
		}

		if (draw) {
			SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
			const MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(hw, &margin);
			SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

			BringWindowToTop(hw);
			SetForegroundWindow(hw);
		}
		else {
			SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
			const MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(hw, &margin);
		}

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		d3d11_device_context->OMSetRenderTargets(1, &d3d11_render_target_view, nullptr);
		d3d11_device_context->ClearRenderTargetView(d3d11_render_target_view, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
		dxgi_swap_chain->Present(vars::UI::vsync, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(!vars::UI::highcpusage)));
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d3d();
	DestroyWindow(hw);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}
bool robloxClosed = false;

void overlay::move_window(HWND hw)
{
	HWND target = FindWindowA(nullptr, "roblox");
	HWND foregroundWindow = GetForegroundWindow();

	if (target != foregroundWindow && hw != foregroundWindow)
	{
		MoveWindow(hw, 0, 0, 0, 0, true);
		return;
	}

	RECT rect;

	if (!GetWindowRect(target, &rect)) {
		if (!robloxClosed) {
			robloxClosed = true;
			system("pause");

		}
		return;
	}
	else {
		robloxClosed = false;

	}

	int rsize_x = rect.right - rect.left;
	int rsize_y = rect.bottom - rect.top;

	if (fullsc(target))
	{
		rsize_x += 16;
	}
	else
	{
		rsize_y -= 39;
		rect.left += 4;
		rect.top += 31;
	}

	if (!MoveWindow(hw, rect.left, rect.top, rsize_x, rsize_y, TRUE))
	{
		std::cerr << "failed to move window." << std::endl;
	}
}

bool overlay::create_device_d3d(HWND hw) {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hw;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL selected_level;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &sd, &dxgiSwapChain, &d3d11Device, &selected_level, &d3d11DeviceContext);
	HRESULT hr2 = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		feature_levels,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&dxgi_swap_chain,
		&d3d11_device,
		&selected_level,
		&d3d11_device_context);


	if (FAILED(hr2)) {
		if (hr2 == DXGI_ERROR_UNSUPPORTED) {
			hr2 = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &sd, &dxgi_swap_chain, &d3d11_device, &selected_level, &d3d11_device_context);
		}
	}

	create_render_target();
	return true;
}
void overlay::cleanup_device_d3d()
{
	cleanup_render_target();

	if (dxgi_swap_chain)
	{
		dxgi_swap_chain->Release();
		dxgi_swap_chain = nullptr;
	}

	if (d3d11_device)
	{
		d3d11_device_context->Release();
		d3d11_device_context = nullptr;
	}

	if (d3d11_device)
	{
		d3d11_device->Release();
		d3d11_device = nullptr;
	}
}

void overlay::create_render_target()
{
	ID3D11Texture2D* back_buffer = nullptr;
	if (SUCCEEDED(dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))))
	{
		d3d11_device->CreateRenderTargetView(back_buffer, nullptr, &d3d11_render_target_view);
		back_buffer->Release();
	}
	else
	{
		std::cerr << "failed to get back buffer" << std::endl;
	}
}

void overlay::cleanup_render_target()
{
	if (d3d11_render_target_view)
	{
		d3d11_render_target_view->Release();
		d3d11_render_target_view = nullptr;
	}
}

LRESULT __stdcall overlay::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (d3d11_device != nullptr && wParam != SIZE_MINIMIZED)
		{
			cleanup_render_target();
			dxgi_swap_chain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			create_render_target();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}