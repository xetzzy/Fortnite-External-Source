#include "includes.hpp"
#include <iostream>

int main()
{
	printf("Waiting for Fortnite...");
	while (game_wnd == 0)
	{
		Sleep(1);
		driver.process_id = driver.get_process_id(L"FortniteClient-Win64-Shipping.exe");
		game_wnd = get_process_wnd(driver.process_id);
		Sleep(1);
	}
	system("cls");
	if (!librarys::init())
	{
		printf("The librarys was not initialized");
		Sleep(3000);
		exit(0);
	}
	if (!input::init())
	{
		printf("The input was not initialized");
		Sleep(3000);
		exit(0);
	}
	if (!gui::init())
	{
		printf("The gui was not initialized");
		Sleep(3000);
		exit(0);
	}
	if (!driver.setup())
	{
		printf("The driver was not initialized");
		Sleep(3000);
		exit(0);
	}
	driver.base_address = driver.get_base_address();
	if (!driver.base_address)
	{
		printf("The driver couldn't get the base address");
		Sleep(3000);
		exit(0);
	}
	create_overlay();
	directx_init();
	render_loop();
	exit(0);
}
