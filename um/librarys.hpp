#pragma once
namespace librarys
{
	HMODULE user32;
	HMODULE win32u;
	bool init()
	{
		HMODULE user32_lib = LoadLibraryA("user32.dll");
		if (!user32_lib) return false;
		HMODULE win32u_lib = LoadLibraryA("win32u.dll");
		if (!win32u_lib) return false;
		user32 = GetModuleHandleA("user32.dll");
		if (!user32) return false;
		win32u = GetModuleHandleA("win32u.dll");
		if (!win32u) return false;
		return true;
	}
}