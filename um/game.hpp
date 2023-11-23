#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;

HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = 0;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 1;
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Border] = ImColor(8, 8, 8, 250);
	style->Colors[ImGuiCol_TitleBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_WindowBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_FrameBg] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_Button] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_CheckMark] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_Header] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_PopupBg] = ImColor(38, 38, 38, 250);
	p_object->Release();
	return S_OK;
}

void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"GayWindowClass",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"GayWindowTitle", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}

void aimbot(uintptr_t target_mesh)
{
	if (!target_mesh) return;
	if (!is_visible(target_mesh)) return;
	Vector3 head3d = get_entity_bone(target_mesh, 109);
	Vector2 head2d = project_world_to_screen(head3d);
	Vector2 target{};
	if (head2d.x != 0)
	{
		if (head2d.x > settings::screen_center_x)
		{
			target.x = -(settings::screen_center_x - head2d.x);
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x > settings::screen_center_x * 2) target.x = 0;
		}
		if (head2d.x < settings::screen_center_x)
		{
			target.x = head2d.x - settings::screen_center_x;
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x < 0) target.x = 0;
		}
	}
	if (head2d.y != 0)
	{
		if (head2d.y > settings::screen_center_y)
		{
			target.y = -(settings::screen_center_y - head2d.y);
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y > settings::screen_center_y * 2) target.y = 0;
		}
		if (head2d.y < settings::screen_center_y)
		{
			target.y = head2d.y - settings::screen_center_y;
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y < 0) target.y = 0;
		}
	}
	input::move_mouse(target.x, target.y);
}

void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void draw_line(Vector2 target, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(Vector2 location, float distance, const ImColor color)
{
	char dist[64];
	sprintf_s(dist, "%.fm", distance);
	ImVec2 text_size = ImGui::CalcTextSize(dist);
	ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

void game_loop()
{
	cache::uworld = driver.read<uintptr_t>(driver.base_address + UWORLD);
	cache::game_instance = driver.read<uintptr_t>(cache::uworld + GAME_INSTANCE);
	cache::local_players = driver.read<uintptr_t>(driver.read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
	cache::player_controller = driver.read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
	cache::local_pawn = driver.read<uintptr_t>(cache::player_controller + LOCAL_PAWN);
	if (cache::local_pawn != 0)
	{
		cache::root_component = driver.read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
		cache::relative_location = driver.read<Vector3>(cache::root_component + RELATIVE_LOCATION);
		cache::player_state = driver.read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
		cache::my_team_id = driver.read<int>(cache::player_state + TEAM_INDEX);
	}
	cache::game_state = driver.read<uintptr_t>(cache::uworld + GAME_STATE);
	cache::player_array = driver.read<uintptr_t>(cache::game_state + PLAYER_ARRAY);
	cache::player_count = driver.read<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
	cache::closest_distance = FLT_MAX;
	cache::closest_mesh = NULL;
	for (int i = 0; i < cache::player_count; i++)
	{
		uintptr_t player_state = driver.read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
		if (!player_state) continue;
		int player_team_id = driver.read<int>(player_state + TEAM_INDEX);
		if (player_team_id == cache::my_team_id) continue;
		uintptr_t pawn_private = driver.read<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private) continue;
		if (pawn_private == cache::local_pawn) continue;
		uintptr_t mesh = driver.read<uintptr_t>(pawn_private + MESH);
		if (!mesh) continue;
		Vector3 head3d = get_entity_bone(mesh, 109);
		Vector2 head2d = project_world_to_screen(head3d);
		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = project_world_to_screen(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = cache::relative_location.distance(bottom3d) / 100;
		if (settings::visuals::enable)
		{
			if (settings::visuals::box)
			{
				if (is_visible(mesh))
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
				}
				else
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
				}
				if (settings::visuals::fill_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
			}
			if (settings::visuals::line)
			{
				if (is_visible(mesh))
				{
					draw_line(bottom2d, ImColor(250, 250, 250, 250));
				}
				else
				{
					draw_line(bottom2d, ImColor(250, 0, 0, 250));
				}
			}
			if (settings::visuals::distance)
			{
				draw_distance(bottom2d, distance, ImColor(250, 250, 250, 250));
			}
		}
		double dx = head2d.x - settings::screen_center_x;
		double dy = head2d.y - settings::screen_center_y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist <= settings::aimbot::fov && dist < cache::closest_distance)
		{
			cache::closest_distance = dist;
			cache::closest_mesh = mesh;
		}
	}
	if (settings::aimbot::enable)
	{
		if (GetAsyncKeyState(settings::aimbot::current_key)) aimbot(cache::closest_mesh);
	}
}

void render_menu()
{
	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
	}
	if (settings::aimbot::show_fov) ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(250, 250, 250, 250), 100, 1.0f);
	if (GetAsyncKeyState(VK_INSERT) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)
	{
		ImGui::SetNextWindowSize({ 620, 350 });
		ImGui::Begin("Fortnite", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		if (ImGui::Button("Aimbot", { 196, 20 })) settings::tab = 0;
		ImGui::SameLine();
		if (ImGui::Button("Visuals", { 196, 20 })) settings::tab = 1;
		ImGui::SameLine();
		if (ImGui::Button("Misc", { 196, 20 })) settings::tab = 2;
		switch (settings::tab)
		{
			case 0:
			{
				ImGui::Checkbox("Enable", &settings::aimbot::enable);
				ImGui::Checkbox("Show Fov", &settings::aimbot::show_fov);
				ImGui::SliderFloat("##Fov", &settings::aimbot::fov, 50, 300, "Fov: %.2f");
				ImGui::SliderFloat("##Smoothness", &settings::aimbot::smoothness, 1, 10, "Smoothness: %.2f");
				ImGui::Combo("##Aimkey", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, sizeof(settings::aimbot::aimkey) / sizeof(*settings::aimbot::aimkey));
				break;
			}
			case 1:
			{
				ImGui::Checkbox("Enable", &settings::visuals::enable);
				ImGui::Checkbox("Box", &settings::visuals::box);
				ImGui::SameLine();
				ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
				ImGui::Checkbox("Line", &settings::visuals::line);
				ImGui::Checkbox("Distance", &settings::visuals::distance);
				break;
			}
			case 2:
			{
				if (ImGui::Button("Unload", { 120, 20 })) exit(0);
				break;
			}
		}
		ImGui::End();
	}
}

HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

WPARAM render_loop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		if (game_wnd == NULL) exit(0);
		HWND active_wnd = GetForegroundWindow();
		if (active_wnd == game_wnd)
		{
			HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
			SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			game_wnd = get_process_wnd(driver.process_id);
			Sleep(250);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		game_loop();
		render_menu();
		ImGui::EndFrame();
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != 0) p_object->Release();
	DestroyWindow(my_wnd);
	return messager.wParam;
}
