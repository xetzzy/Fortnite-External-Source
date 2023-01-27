#include "includes.hpp"
#include <thread>

int main()
{
	driver.process_id = driver.get_process_id(_(L"FortniteClient-Win64-Shipping.exe"));
	if (driver.process_id != 0)
	{
		driver.setup();
		driver.base_address = driver.get_base_address();
		Input::Init();
		create_overlay();
		directx_init();
		render_loop();
	}
	return 0;
}