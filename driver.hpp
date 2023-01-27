#pragma once
#include <TlHelp32.h>
#include <cstdint>
class _driver
{
public:
	uintptr_t base_address;
	INT32 process_id;
	int get_process_id(LPCTSTR process_name)
	{
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt))
		{
			do {
				if (!lstrcmpi(pt.szExeFile, process_name))
				{
					CloseHandle(hsnap);
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);
		return { NULL };
	}
	bool setup()
	{
		return true;
	}
	uintptr_t get_base_address()
	{
		uintptr_t base_address{};
		return base_address;
	}
	template<typename T> T write(uint64_t address, T buffer)
	{
		return buffer;
	}
	template<typename T> T read(uint64_t address)
	{
		T buffer{};
		return buffer;
	}
};
_driver driver;