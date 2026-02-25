#include "game.hpp"

#include "../utilities/math/math.hpp"
#include "../globals/globals.hpp"

#include "../driver/driver.hpp"

#include "sdk/decryptions/decryptions.hpp"
#include "sdk/types/types.hpp"
#include "sdk/offsets.hpp"

namespace game {
    void update_camera( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_player_camera )
                continue;

            auto pov = driver.read< minimal_view_info_t >( sdk::g_player_camera + sdk::offsets::minimal_view_info );

            sdk::g_camera_position.m_rotation.pitch = pov.m_rotation.pitch;
            sdk::g_camera_position.m_rotation.yaw = pov.m_rotation.yaw;
            sdk::g_camera_position.m_rotation.roll = pov.m_rotation.roll;

            sdk::g_camera_position.m_location.x = ( float )pov.m_location.x;
            sdk::g_camera_position.m_location.y = ( float )pov.m_location.y;
            sdk::g_camera_position.m_location.z = ( float )pov.m_location.z;

            sdk::g_camera_position.m_fov = ( double )pov.m_fov;
        }
    }

    void cache_healths( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_player_camera )
                continue;

            if ( sdk::g_players_pawn_array.empty( ) )
                continue;

            for ( int idx{ }; idx < sdk::g_players_pawn_array.size( ); ++idx ) {
                const auto& player = sdk::g_players_pawn_array[ idx ];

                if ( !player )
                    continue;

                const auto& root_component = driver.read< uint64_t >( player + sdk::offsets::root_component );

                if ( !root_component )
                    continue;

                const auto& health_component = driver.read< uint64_t >( player + sdk::offsets::health_component );

                if ( !health_component )
                    continue;

                const auto& status = driver.read< quat_t >( health_component + sdk::offsets::health_table );
                float current{ };

                if ( status.y != 0.f ) {
                    current = ( float )( status.x / status.y );

                    if ( current <= 0.f ) {
                        sdk::g_players_state[ root_component ][ 0 ] = true;
                        sdk::g_players_health[ root_component ][ 0 ] = 0;
                    }
                    else {
                        sdk::g_players_state[ root_component ][ 0 ] = false;
                        sdk::g_players_health[ root_component ][ 0 ] = static_cast< int >( current * 100 );
                    }
                }
            }

            Sleep( 5 );
        }
    }

    void cache_bone_positions( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_player_camera )
                continue;

            for ( int idx{ }; idx < sdk::g_players_pawn_array.size( ); ++idx ) {
                const auto& player = sdk::g_players_pawn_array[ idx ];
 
                if ( !player )
                    continue;

                const auto& root_component = driver.read< uint64_t >( player + sdk::offsets::root_component );

                if ( !root_component )
                    continue;

                const auto& mesh = driver.read< uint64_t >( player + sdk::offsets::mesh );

                if ( !mesh )
                    continue;

                const auto& bone_array = driver.read< uint64_t >( sdk::decryptions::decrypt_bone_array( mesh ) );

                if ( !bone_array )
                    continue;

                const auto& health = sdk::g_players_health[ root_component ][ 0 ];

                if ( health <= 0 )
                    continue;
                
                sdk::g_bones_position[ root_component ][ 7 ] = get_bone( bone_array, mesh, 7 );
                sdk::g_bones_position[ root_component ][ 6 ] = get_bone( bone_array, mesh, 6 );
                sdk::g_bones_position[ root_component ][ 5 ] = get_bone( bone_array, mesh, 5 );
                sdk::g_bones_position[ root_component ][ 9 ] = get_bone( bone_array, mesh, 9 );
                sdk::g_bones_position[ root_component ][ 10 ] = get_bone( bone_array, mesh, 10 );
                sdk::g_bones_position[ root_component ][ 80 ] = get_bone( bone_array, mesh, 80 );
                sdk::g_bones_position[ root_component ][ 43 ] = get_bone( bone_array, mesh, 43 );
                sdk::g_bones_position[ root_component ][ 44 ] = get_bone( bone_array, mesh, 44 );
                sdk::g_bones_position[ root_component ][ 81 ] = get_bone( bone_array, mesh, 81 );
                sdk::g_bones_position[ root_component ][ 3 ] = get_bone( bone_array, mesh, 3 );
                sdk::g_bones_position[ root_component ][ 2 ] = get_bone( bone_array, mesh, 2 );
                sdk::g_bones_position[ root_component ][ 1 ] = get_bone( bone_array, mesh, 1 );
                sdk::g_bones_position[ root_component ][ 65 ] = get_bone( bone_array, mesh, 65 );
                sdk::g_bones_position[ root_component ][ 66 ] = get_bone( bone_array, mesh, 66 );
                sdk::g_bones_position[ root_component ][ 84 ] = get_bone( bone_array, mesh, 84 );
                sdk::g_bones_position[ root_component ][ 85 ] = get_bone( bone_array, mesh, 85 );
                sdk::g_bones_position[ root_component ][ 69 ] = get_bone( bone_array, mesh, 69 );
                sdk::g_bones_position[ root_component ][ 70 ] = get_bone( bone_array, mesh, 70 );
                sdk::g_bones_position[ root_component ][ 86 ] = get_bone( bone_array, mesh, 86 );
                sdk::g_bones_position[ root_component ][ 87 ] = get_bone( bone_array, mesh, 87 );
            }
        }
    }

    void cache_player_names( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_player_camera )
                continue;

            for ( int idx{ }; idx < sdk::g_players_pawn_array.size( ); ++idx ) {
                const auto& player = sdk::g_players_pawn_array[ idx ];

                if ( !player )
                    continue;

                const auto& root_component = driver.read< uint64_t >( player + sdk::offsets::root_component );

                if ( !root_component )
                    continue;

                const auto& player_state = driver.read< uint64_t >( player + sdk::offsets::player_state );

                if ( !player_state )
                    continue;

                const auto& player_name = driver.read< sdk::string_t >( player_state + sdk::offsets::player_name );

                if ( !player_name.is_valid( )
                    || !player_name.length( ) )
                    continue;

                sdk::g_players_names[ root_component ][ 0 ] = player_name.to_wstring( );
            }

            Sleep( 5000 );
        }
    }

    uint64_t get_aimbot_target( ) {
        if ( !sdk::g_uworld
            || !sdk::g_player_camera )
            return 0;

        uint64_t best_target{ };
        float distance{ FLT_MAX };

        for ( int idx{ }; idx < sdk::g_players_array.size( ); ++idx ) {
            const auto& player = sdk::g_players_array[ idx ];

            if ( !player )
                continue;

            const auto& world_position = sdk::g_players_positions[ player ][ 0 ];
            const auto& player_distance = sdk::g_local_position.distance( world_position ) / 100.f;

            vec2_t screen_position{ };

            if ( !world_to_screen( sdk::g_camera_position, world_position, &screen_position ) )
                continue;

            if ( in_fov( g_width / 2, g_height / 2, config::aimbot_fov, screen_position.x, screen_position.y ) ) {
                if ( player_distance < distance ) {
                    best_target = player;
                    distance = player_distance;
                }
            }
        }

        return best_target;
    }

    void cache_local_player( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_local_pawn )
                continue;

            sdk::g_local_position = driver.read< vec3_t >( driver.read< uint64_t >( sdk::g_local_pawn + sdk::offsets::root_component ) + sdk::offsets::relative_location );

            Sleep( 20 );
        }
    }

    void cache_list( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_local_pawn )
                continue;

            const auto& persistent = driver.read< uint64_t >( sdk::g_uworld + sdk::offsets::persistent );

            if ( !persistent )
                continue;

            auto player_array = driver.read< sdk::array_t >( persistent + sdk::offsets::actor_array );

            if ( !player_array.is_valid( ) )
                continue;

            auto player_list = player_array.get_data_list( );
            auto player_count = player_list.size( );
                                                                                    
            sdk::g_players_array.reserve( player_count );
            sdk::ignore_list.insert( sdk::g_local_pawn );

            for ( int idx{ }; idx < player_count; ++idx ) {
                auto pawn_private = player_list[ idx ];

                if ( !pawn_private )
                    continue;

                if ( pawn_private == sdk::g_local_pawn )
                    continue;

                if ( sdk::ignore_list.find( pawn_private ) != sdk::ignore_list.end( ) )
                    continue;

                const auto& index = driver.read< uint64_t >( pawn_private + sdk::offsets::index );

                if ( index != 17694720 ) {
                    sdk::ignore_list.insert( pawn_private );
                    continue;
                }

                auto root_component = driver.read< uint64_t >( pawn_private + sdk::offsets::root_component );
 
                if ( !root_component )
                    continue;

                if ( sdk::g_players_state[ root_component ][ 0 ] ) {
                    sdk::g_players_array.erase( std::find( sdk::g_players_array.begin( ), sdk::g_players_array.end( ), root_component ) );
                    sdk::g_players_pawn_array.erase( std::find( sdk::g_players_pawn_array.begin( ), sdk::g_players_pawn_array.end( ), pawn_private ) );
                    sdk::ignore_list.erase( root_component );

                    continue;
                }
 
                sdk::ignore_list.insert( pawn_private );
                sdk::g_players_array.push_back( root_component );
                sdk::g_players_pawn_array.push_back( pawn_private );
            }

            Sleep( 500 );
        }
    }

    void cache_positions( ) {
        while ( 1 ) {
            if ( !sdk::g_uworld
                || !sdk::g_local_pawn )
                continue;

            if ( sdk::g_players_array.empty( ) )
                continue;

            for ( int idx{ }; idx < sdk::g_players_array.size( ); ++idx ) {
                const auto& player = sdk::g_players_array[ idx ];

                if ( !player )
                    continue;

                sdk::g_players_positions[ player ][ 0 ] = driver.read< vec3_t >( player + sdk::offsets::relative_location );
            }
        }
    }

    void cache_uworld( ) {
        while ( 1 ) {
            uworld_emu->Reset( );
            uworld_emu->Initialize( );

            player_camera_manager_emulator->Reset( );
            player_camera_manager_emulator->Initialize( );

            const auto& uworld = sdk::decryptions::uworld( );

            if ( !uworld ) {
                sdk::clear( );
                continue;
            }

            sdk::g_uworld = uworld;

            const auto& game_instance = sdk::decryptions::game_instance( uworld );

            if ( !game_instance ) {
                sdk::clear( );
                continue;
            }

            const auto& local_players = driver.read< uint64_t >( driver.read< uint64_t >( game_instance + sdk::offsets::local_players ) );

            if ( !local_players ) {
                sdk::clear( );
                continue;
            }

            const auto& player_controller = driver.read< uint64_t >( local_players + sdk::offsets::player_controller );

            if ( !player_controller ) {
                sdk::clear( );
                continue;
            }

            sdk::g_player_controller = player_controller;

            const auto& local_pawn = driver.read< uint64_t >( player_controller + sdk::offsets::pawn );

            if ( !local_pawn )
                continue;

            sdk::g_local_pawn = local_pawn;

            const auto& player_camera = sdk::decryptions::player_camera_manager( sdk::g_player_controller );

            if ( !player_camera ) 
                continue;

            sdk::g_player_camera = player_camera;

            Sleep( 1000 );
        }
    }
}