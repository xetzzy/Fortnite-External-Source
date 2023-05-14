#pragma once
namespace input
{
	static BYTE ntusersendinput_bytes[30];
	static BOOLEAN WINAPI init()
	{
		const HMODULE user32 = LoadLibraryA("user32.dll");
		if (!user32) return FALSE;
		LPVOID ntusersendinput_addr = GetProcAddress(user32, "NtUserSendInput");
		if (!ntusersendinput_addr)
		{
			const HMODULE win32u = LoadLibraryA("win32u.dll");
			if (!win32u) return FALSE;
			ntusersendinput_addr = GetProcAddress(win32u, "NtUserSendInput");
			if (!ntusersendinput_addr) return FALSE;
		}
		memcpy(ntusersendinput_bytes, ntusersendinput_addr, 30);
		return TRUE;
	}
	static BOOLEAN WINAPI ntusersendinput(UINT cinputs, LPINPUT pinputs, int cbsize)
	{
		LPVOID ntusersendinput_spoof = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!ntusersendinput_spoof) return FALSE;
		memcpy(ntusersendinput_spoof, ntusersendinput_bytes, 30);
		NTSTATUS result = reinterpret_cast<NTSTATUS(NTAPI*)(UINT, LPINPUT, int)>(ntusersendinput_spoof)(cinputs, pinputs, cbsize);
		ZeroMemory(ntusersendinput_spoof, 0x1000);
		VirtualFree(ntusersendinput_spoof, 0, MEM_RELEASE);
		return (result > 0);
	}
	static BOOLEAN WINAPI move_mouse(int x, int y)
	{
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dx = x;
		input.mi.dy = y;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
		return ntusersendinput(1, &input, sizeof(input));
	}
}