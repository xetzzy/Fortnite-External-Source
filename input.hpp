#pragma once
namespace Input
{
	static BYTE NtUserSendInput_Bytes[30];
	static BYTE NtUserGetAsyncKeyState_Bytes[30];
	static BOOLEAN WINAPI Init()
	{
		LPVOID NtUserSendInput_Addr = GetProcAddress(GetModuleHandle(_(L"win32u")), _("NtUserSendInput"));
		if (!NtUserSendInput_Addr)
		{
			NtUserSendInput_Addr = GetProcAddress(GetModuleHandle(_(L"user32")), _("NtUserSendInput"));
			if (!NtUserSendInput_Addr)
			{
				NtUserSendInput_Addr = GetProcAddress(GetModuleHandle(_(L"user32")), _("SendInput"));
				if (!NtUserSendInput_Addr)
					return FALSE;
			}
		}
		LPVOID NtUserGetAsyncKeyState_Addr = GetProcAddress(GetModuleHandle(_(L"win32u")), _("NtUserGetAsyncKeyState"));
		if (!NtUserGetAsyncKeyState_Addr)
		{
			NtUserGetAsyncKeyState_Addr = GetProcAddress(GetModuleHandle(_(L"user32")), _("NtUserGetAsyncKeyState"));
			if (!NtUserGetAsyncKeyState_Addr)
			{
				NtUserGetAsyncKeyState_Addr = GetProcAddress(GetModuleHandle(_(L"user32")), _("GetAsyncKeyState"));
				if (!NtUserGetAsyncKeyState_Addr)
					return FALSE;
			}
		}
		memcpy(NtUserSendInput_Bytes, NtUserSendInput_Addr, 30);
		memcpy(NtUserGetAsyncKeyState_Bytes, NtUserGetAsyncKeyState_Addr, 30);
		return TRUE;
	}
	static BOOLEAN WINAPI NtUserSendInput(UINT cInputs, LPINPUT pInputs, int cbSize)
	{
		LPVOID NtUserSendInput_Spoof = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NtUserSendInput_Spoof)
			return FALSE;
		memcpy(NtUserSendInput_Spoof, NtUserSendInput_Bytes, 30);
		NTSTATUS Result = reinterpret_cast<NTSTATUS(NTAPI*)(UINT, LPINPUT, int)>(NtUserSendInput_Spoof)(cInputs, pInputs, cbSize);
		ZeroMemory(NtUserSendInput_Spoof, 0x1000);
		VirtualFree(NtUserSendInput_Spoof, 0, MEM_RELEASE);
		return (Result > 0);
	}
	static UINT WINAPI NtUserGetAsyncKeyState(UINT Key)
	{
		LPVOID NtUserGetAsyncKeyState_Spoof = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NtUserGetAsyncKeyState_Spoof)
			return FALSE;
		memcpy(NtUserGetAsyncKeyState_Spoof, NtUserGetAsyncKeyState_Bytes, 30);
		NTSTATUS Result = reinterpret_cast<NTSTATUS(NTAPI*)(UINT)>(NtUserGetAsyncKeyState_Spoof)(Key);
		ZeroMemory(NtUserGetAsyncKeyState_Spoof, 0x1000);
		VirtualFree(NtUserGetAsyncKeyState_Spoof, 0, MEM_RELEASE);
		return Result;
	}
	static BOOLEAN WINAPI MoveMouse(int X, int Y)
	{
		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dx = X;
		input.mi.dy = Y;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
		return NtUserSendInput(1, &input, sizeof(input));
	}
	static UINT WINAPI GetAsyncKeyState(UINT Key)
	{
		return NtUserGetAsyncKeyState(Key);
	}
}