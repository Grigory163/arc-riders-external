#pragma once

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

#include <vector>
#include <chrono>
#include <thread>

#include <dwmapi.h>
#include <intrin.h>
#include <d3d11.h>
#include <imgui.h>

inline ImFont* esp_font{ };

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "unicorn2.lib" )

namespace config {
	inline bool aimbot_enabled{ };
	inline bool aimbot_draw_fov{ };
	inline int aimbot_key{ };
	inline int aimbot_bone{ };
	inline float aimbot_smooth{ };
	inline float aimbot_fov{ };

	inline bool visuals_enabled{ };
	inline bool visuals_box{ };
	inline bool visuals_distance{ };
	inline bool visuals_skeleton{ };
	inline bool visuals_health_bar{ };
	inline bool visuals_name{ };
	inline float max_distance{ };

	inline float visuals_box_color[ 4 ]{ 255, 255, 255, 255 };
	inline float visuals_distance_color[ 4 ]{ 255, 255, 255, 255 };
	inline float visuals_name_color[ 4 ]{ 255, 255, 255, 255 };
	inline float visuals_health_bar_color[ 4 ]{ 255, 255, 255, 255 };
	inline float visuals_skeleton_color[ 4 ]{ 255, 255, 255, 255 };
}