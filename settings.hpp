#pragma once
namespace settings
{
	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);
	inline int tab = 0;
	inline bool show_menu = true;
	namespace aimbot
	{
		inline bool enable = false;
		inline bool show_fov = false;
		inline bool filled_fov = false;
		inline float fov = 80.0f;
		inline float smoothness = 5.0f;
	}
	namespace visuals
	{
		inline bool enable = false;
		inline bool cornered_box = false;
		inline bool filled_box = false;
		inline bool line = false;
	}
	namespace misc
	{
		inline bool enable = false;
		inline bool no_recoil = false;
	}
}