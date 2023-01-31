#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
HWND game_wnd = NULL;
RECT game_rect = { NULL };
HWND my_wnd = NULL;
MSG messager = { NULL };

HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
		exit(3);
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
	io.IniFilename = NULL;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->FrameBorderSize = 1.0f;
	style->ChildBorderSize = 1.0f;
	style->ItemSpacing = ImVec2(8, 8);
	style->Colors[ImGuiCol_Text] = ImColor(250, 250, 250, 255);
	style->Colors[ImGuiCol_TitleBg] = ImColor(10, 10, 10, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(10, 10, 10, 255);
	style->Colors[ImGuiCol_WindowBg] = ImColor(12, 12, 12, 255);
	style->Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Button] = ImColor(10, 10, 10, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(10, 10, 10, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(10, 10, 10, 255);
	style->Colors[ImGuiCol_FrameBg] = ImColor(12, 12, 12, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(12, 12, 12, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(12, 12, 12, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(46, 46, 46, 255);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(46, 46, 46, 255);
	style->Colors[ImGuiCol_CheckMark] = ImColor(46, 46, 46, 255);
	style->Colors[ImGuiCol_ChildBg] = ImColor(10, 10, 10, 255);
	p_object->Release();
	return S_OK;
}

void create_overlay()
{
	WNDCLASSEXA wcsex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		(_("A Not Suspect Window Class")),
		LoadIcon(nullptr, IDI_APPLICATION)
	};
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	RegisterClassExA(&wcsex);
	my_wnd = CreateWindowExA(NULL, (_("A Not Suspect Window Class")), (_("A Not Suspect Window Title")), WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, wcsex.hInstance, NULL);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	SetWindowPos(my_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	UpdateWindow(my_wnd);
}

void cleanup_d3d()
{
	if (p_device != NULL)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != NULL)
	{
		p_object->Release();
	}
}

void aimbot(uintptr_t target_pawn, uintptr_t closest_distance)
{
	if (!target_pawn) return;
	uintptr_t mesh = driver.read<uintptr_t>(target_pawn + 0x310);
	if (!mesh)
	{
		closest_distance = FLT_MAX;
		target_pawn = NULL;
	}
	Vector3 head3d = GetBoneWithRotation(mesh, 106);
	Vector2 head2d = ProjectWorldToScreen(head3d);
	double dx = head2d.x - (settings::width / 2);
	double dy = head2d.y - (settings::height / 2);
	float dist = sqrtf(dx * dx + dy * dy);
	if (dist > settings::aimbot::fov or head2d.x == 0 or head2d.y == 0)
	{
		closest_distance = FLT_MAX;
		target_pawn = NULL;
	}
	Vector2 screen_center = { (double)settings::width / 2, (double)settings::height / 2 };
	Vector2 target{};
	if (IsVisible(mesh))
	{
		if (head2d.x != 0)
		{
			if (head2d.x > screen_center.x)
			{
				target.x = -(screen_center.x - head2d.x);
				target.x /= settings::aimbot::smoothness;
				if (target.x + screen_center.x > screen_center.x * 2) target.x = 0;
			}
			if (head2d.x < screen_center.x)
			{
				target.x = head2d.x - screen_center.x;
				target.x /= settings::aimbot::smoothness;
				if (target.x + screen_center.x < 0) target.x = 0;
			}
		}
		if (head2d.y != 0)
		{
			if (head2d.y > screen_center.y)
			{
				target.y = -(screen_center.y - head2d.y);
				target.y /= settings::aimbot::smoothness;
				if (target.y + screen_center.y > screen_center.y * 2) target.y = 0;
			}
			if (head2d.y < screen_center.y)
			{
				target.y = head2d.y - screen_center.y;
				target.y /= settings::aimbot::smoothness;
				if (target.y + screen_center.y < 0) target.y = 0;
			}
		}
		Input::MoveMouse(target.x, target.y);
	}
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

void draw_line(Vector2 target, ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::width / 2, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void game_loop()
{
	pointer::uworld = driver.read<uintptr_t>(driver.base_address + 0xEC18148);
	pointer::game_instance = driver.read<uintptr_t>(pointer::uworld + 0x1B8);
	pointer::local_players = driver.read<uintptr_t>(driver.read<uintptr_t>(pointer::game_instance + 0x38));
	pointer::player_controller = driver.read<uintptr_t>(pointer::local_players + 0x30);
	pointer::local_pawn = driver.read<uintptr_t>(pointer::player_controller + 0x330);
	pointer::root_component = driver.read<uintptr_t>(pointer::local_pawn + 0x190);
	pointer::player_state = driver.read<uintptr_t>(pointer::local_pawn + 0x2A8);
	pointer::game_state = driver.read<uintptr_t>(pointer::uworld + 0x158);
	pointer::player_array = driver.read<uintptr_t>(pointer::game_state + 0x2A0);
	pointer::closest_distance = FLT_MAX;
	pointer::closest_pawn = NULL;
	int count = driver.read<int>(pointer::game_state + (0x2A0 + sizeof(uintptr_t)));
	for (int i = 0; i < count; i++)
	{
		uintptr_t current_player_state = driver.read<uintptr_t>(pointer::player_array + (i * sizeof(uintptr_t)));
		if (!current_player_state) continue;
		int my_team_id = driver.read<int>(pointer::player_state + 0x1100);
		int current_actor_team_id = driver.read<int>(current_player_state + 0x1100);
		if (my_team_id == current_actor_team_id) continue;
		uintptr_t current_local_pawn_private = driver.read<uintptr_t>(current_player_state + 0x300);
		if (!current_local_pawn_private) continue;
		if (current_local_pawn_private == pointer::local_pawn) continue;
		uintptr_t current_mesh = driver.read<uintptr_t>(current_local_pawn_private + 0x310);
		if (!current_mesh) continue;
		Vector3 head3d = GetBoneWithRotation(current_mesh, 106);
		Vector2 head2d = ProjectWorldToScreen(head3d);
		Vector3 bottom3d = GetBoneWithRotation(current_mesh, 0);
		Vector2 bottom2d = ProjectWorldToScreen(bottom3d);
		float corner_height = abs(head2d.y - bottom2d.y);
		float corner_width = corner_height * 0.80;
		double dx = head2d.x - (settings::width / 2);
		double dy = head2d.y - (settings::height / 2);
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist < settings::aimbot::fov && dist < pointer::closest_distance)
		{
			pointer::closest_distance = dist;
			pointer::closest_pawn = current_local_pawn_private;
		}
		if (settings::visuals::enable)
		{
			if (settings::visuals::cornered_box)
			{
				if (IsVisible(current_mesh))
				{
					draw_cornered_box(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, ImColor(255, 255, 255, 255), 1);
					if (settings::visuals::filled_box) draw_filled_rect(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, ImColor(255, 255, 255, 20));
				}
				else
				{
					draw_cornered_box(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, ImColor(255, 0, 0, 255), 1);
					if (settings::visuals::filled_box) draw_filled_rect(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, ImColor(255, 0, 0, 20));
				}
			}
			if (settings::visuals::line)
			{
				if (IsVisible(current_mesh))
				{
					draw_line(bottom2d, ImColor(255, 255, 255, 255));
				}
				else
				{
					draw_line(bottom2d, ImColor(255, 0, 0, 255));
				}
			}
		}
	}
	if (settings::aimbot::enable)
	{
		if (Input::GetAsyncKeyState(VK_RBUTTON))
		{
			aimbot(pointer::closest_pawn, pointer::closest_distance);
		}
	}
	if (settings::misc::enable)
	{
		if (settings::misc::no_recoil)
		{
			if (Input::GetAsyncKeyState(VK_LBUTTON))
			{
				driver.write<float>(pointer::player_controller + 0x64, -1);
			}
		}
	}
}

void render_menu()
{
	if (settings::aimbot::show_fov)
	{
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(255, 255, 255, 255), 100, 1.0f);
		if (settings::aimbot::filled_fov) ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(255, 255, 255, 20), 100);
	}
	if (Input::GetAsyncKeyState(VK_INSERT) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)
	{
		ImGui::SetNextWindowSize({ 620.f, 350.f });
		ImGui::Begin(_("Fortnite"), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetCursorPos({ 22.f, 56.f });
		if (ImGui::Button(_("Aimbot"), { 89.f, 32.f })) settings::tab = 0;
		ImGui::SetCursorPos({ 22.f, 93.f });
		if (ImGui::Button(_("Visuals"), { 89.f, 32.f })) settings::tab = 1;
		ImGui::SetCursorPos({ 22.f, 130.f });
		if (ImGui::Button(_("Misc"), { 89.f, 32.f })) settings::tab = 2;
		ImGui::SetCursorPos({ 22.f, 269.f });
		if (ImGui::Button(_("Unload"), { 89.f, 32.f })) exit(0);
		switch (settings::tab)
		{
		case 0:
		{
			ImGui::SetCursorPos({ 137.f, 39.f });
			ImGui::BeginChild(_("##Aimbot"), { 450.f, 279.f }, true);
			ImGui::Checkbox(_("Enable Aimbot"), &settings::aimbot::enable);
			ImGui::Checkbox(_("Show Fov"), &settings::aimbot::show_fov);
			ImGui::SameLine();
			ImGui::Checkbox(_("Filled Fov"), &settings::aimbot::filled_fov);
			ImGui::SliderFloat(_("##Fov"), &settings::aimbot::fov, 50.0f, 600.0f, _("FOV: %.3f"));
			ImGui::SliderFloat(_("##Smooth"), &settings::aimbot::smoothness, 1.0f, 20.0f, _("Smoothness: %.3f"));
			break;
		}
		case 1:
		{
			ImGui::SetCursorPos({ 137.f, 39.f });
			ImGui::BeginChild(_("##Visuals"), { 450.f, 279.f }, true);
			ImGui::Checkbox(_("Enable Visuals"), &settings::visuals::enable);
			ImGui::Checkbox(_("Cornered Box"), &settings::visuals::cornered_box);
			ImGui::SameLine();
			ImGui::Checkbox(_("Filled Box"), &settings::visuals::filled_box);
			ImGui::Checkbox(_("Line"), &settings::visuals::line);
			break;
		}
		case 2:
			ImGui::SetCursorPos({ 137.f, 39.f });
			ImGui::BeginChild(_("##Misc"), { 450.f, 279.f }, true);
			ImGui::Checkbox(_("Enable Misc"), &settings::misc::enable);
			ImGui::Checkbox(_("No Recoil"), &settings::misc::no_recoil);
			break;
		}
		ImGui::EndChild();
		ImGui::End();
	}
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
		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);
		if (hwnd_active == game_wnd)
		{
			HWND hwnd_test = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(my_wnd, hwnd_test, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(game_wnd, &rc);
		ClientToScreen(game_wnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = game_wnd;
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
		p_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(NULL, NULL, NULL, NULL);
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
	cleanup_d3d();
	DestroyWindow(my_wnd);
	return messager.wParam;
}