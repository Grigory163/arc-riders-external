#pragma once

#include "../../includes/includes.hpp"
#include "../../utilities/math/math.hpp"
#include <unordered_set>
#include <unordered_map>

namespace sdk {
	namespace offsets {
		inline uint64_t local_players{ 0x80 };
		inline uint64_t player_controller{ 0x58 };
		inline uint64_t pawn{ 0x368 };
		inline uint64_t relative_location{ 0x218 };
		inline uint64_t control_rotation{ 0x3a0 };
		inline uint64_t root_component{ 0x1e0 };
		inline uint64_t mesh{ 0x3b0 };
		inline uint64_t index{ 0x68 };
		inline uint64_t actor_array{ 0xd0 };
		inline uint64_t persistent{ 0xd0 };
		inline uint64_t minimal_view_info{ 0xc10 };
		inline uint64_t player_state{ 0x348 };
		inline uint64_t player_name{ 0x3c0 };
		inline uint64_t health_component{ 0xCA0 };
		inline uint64_t health_table{ 0x620 };
	}

	inline std::vector< uint64_t > g_players_array{ };
	inline std::vector< uint64_t > g_players_pawn_array{ };

	inline uint64_t g_player_controller{ };
	inline uint64_t g_local_pawn{ };
	inline uint64_t g_player_camera{ };
	inline uint64_t g_uworld{ };

	inline vec3_t g_local_position{ };
	inline minimal_view_info_t g_camera_position{ };

	inline std::unordered_set< uint64_t > ignore_list{ };

	inline inline std::unordered_map< uint64_t, std::unordered_map< int, vec3_t > > g_players_positions = { };
	inline inline std::unordered_map< uint64_t, std::unordered_map< int, vec3_t > > g_bones_position = { };
	inline inline std::unordered_map< uint64_t, std::unordered_map< int, std::wstring > > g_players_names = { };
	inline inline std::unordered_map< uint64_t, std::unordered_map< int, int > > g_players_health = { };
	inline inline std::unordered_map< uint64_t, std::unordered_map< int, bool > > g_players_state = { };

	inline void clear( ) {
		sdk::g_players_array.clear( );
		sdk::g_bones_position.clear( );
		sdk::g_players_pawn_array.clear( );
		sdk::g_players_names.clear( );
		sdk::g_players_state.clear( );
		sdk::g_players_health.clear( );
		sdk::ignore_list.clear( );

		sdk::g_local_pawn = 0;
		sdk::g_uworld = 0;
		sdk::g_camera_position = { };
		sdk::g_local_position = { };
	}
}