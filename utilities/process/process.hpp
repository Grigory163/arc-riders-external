#pragma once

#include "../../includes/includes.hpp"

struct windows_finder_params_t {
    DWORD pidOwner{ };

    std::string wndClassName = "";
    std::string wndName = "";

    RECT pos = { };
    POINT res = { };

    float percentAllScreens = 0.0f;
    float percentMainScreen = 0.0f;

    DWORD style = { };
    DWORD styleEx = { };

    bool satisfyAllCriteria = false;
    std::vector< HWND > hwnds = { };
};

namespace process {
	uint32_t get_process_id( const char* );

	std::vector< uint32_t > get_process_ids( const char* );
    std::vector< HWND > find( windows_finder_params_t );
}