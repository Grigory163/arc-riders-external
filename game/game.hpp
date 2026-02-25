#pragma once

#include "../includes/includes.hpp"

namespace game {
	void cache_list( );
	void cache_uworld( );
	void update_camera( );
    void cache_positions( );
    void cache_local_player( );
    void cache_player_names( );
	void cache_healths( );
	void cache_bone_positions( );

	uint64_t get_aimbot_target( );
}