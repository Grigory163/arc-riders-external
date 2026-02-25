#pragma once

#include "../../imgui/imgui.h"
#include "../../game/game.hpp"
#include "../../driver/driver.hpp"
#include "../../game/sdk/offsets.hpp"

#include "../../utilities/math/math.hpp"

namespace aimbot {
    void loop( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_local_pawn
                || !sdk::g_player_controller )
                continue;

            if ( sdk::g_bones_position.empty( )
                || sdk::g_bones_position.size( ) <= 0 )
                continue;

            const auto& target = game::get_aimbot_target( );

            if ( !target ) {
                Sleep( 100 );
                continue;
            }

            const auto& current_angles = driver.read< vec2_t >( sdk::g_player_controller + sdk::offsets::control_rotation );

            if ( current_angles.x == 0
                || current_angles.y == 0 ) {
                Sleep( 100 );
                continue;
            }

            vec3_t bone_position{ };

            if ( config::aimbot_bone == 0 )
                bone_position = sdk::g_bones_position[ target ][ 7 ];
            else if ( config::aimbot_bone == 1 )
                bone_position = sdk::g_bones_position[ target ][ 6 ];
            else
                bone_position = sdk::g_bones_position[ target ][ 1 ];

            if ( bone_position.x == 0
                || bone_position.y == 0
                || bone_position.z == 0 )
                continue;

            const auto& camera_position = sdk::g_camera_position.m_location;
            const auto& new_view_angles = calculate_view_angles( camera_position, bone_position, current_angles, config::aimbot_smooth );

            if ( new_view_angles.x != 0
                && new_view_angles.y != 0 ) {
                if ( GetAsyncKeyState( config::aimbot_key ) & 0x8000 ) {
                    driver.write< double >( sdk::g_player_controller + 0x3a0, new_view_angles.x );
                    driver.write< double >( sdk::g_player_controller + 0x3a0 + 0x8, new_view_angles.y );
                }
            }
        }
    }
}