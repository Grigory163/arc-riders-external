#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS 

#include "../../game/game.hpp"
#include "../../driver/driver.hpp"
#include "../../game/sdk/offsets.hpp"

#include "../../utilities/math/math.hpp"
#include <imgui_internal.h>

namespace visuals {
    char* to_ansi( const wchar_t* unicode ) {
        char output[ 512 ]{ };
        ImTextStrToUtf8( output, sizeof( output ), ( const ImWchar* )unicode, ( const ImWchar* )&unicode[ wcslen( unicode ) ] );
        return output;
    }

    void add_text( const ImVec2& pos, ImU32 col, const char* text, bool outlined, float font_size, ImFont* font ) {
        if ( outlined ) {
            ImGui::GetBackgroundDrawList( )->AddText( font, font_size, ImVec2( pos.x + 1, pos.y ), IM_COL32_BLACK, text );
            ImGui::GetBackgroundDrawList( )->AddText( font, font_size, ImVec2( pos.x - 1, pos.y ), IM_COL32_BLACK, text );
            ImGui::GetBackgroundDrawList( )->AddText( font, font_size, ImVec2( pos.x, pos.y + 1 ), IM_COL32_BLACK, text );
            ImGui::GetBackgroundDrawList( )->AddText( font, font_size, ImVec2( pos.x, pos.y - 1 ), IM_COL32_BLACK, text );
        }

        ImGui::GetBackgroundDrawList( )->AddText( font, font_size, pos, col, text );
    }

    void render_player_name( std::string player_name, ImVec2 top_center ) {
        ImVec2 text_position = top_center;
        ImVec2 text_size = esp_font->CalcTextSizeA( 12.f, FLT_MAX, 0.0f, player_name.c_str( ) );

        if ( text_position.y == top_center.y )
            text_position.y -= text_size.y;

        ImVec2 text_position_aligned = ImVec2( text_position.x - text_size.x / 2.f, text_position.y );
        add_text( text_position_aligned, ImColor{ config::visuals_name_color[ 0 ], config::visuals_name_color[ 1 ], config::visuals_name_color[ 2 ], config::visuals_name_color[ 3 ] }, player_name.c_str( ), true, 12.f, esp_font );
    }

    void render_player_distance( std::string player_distance, ImVec2 bottom_center ) {
        ImVec2 text_position = bottom_center;
        ImVec2 text_size = esp_font->CalcTextSizeA( 12.f, FLT_MAX, 0.0f, player_distance.c_str( ) );
        ImVec2 text_position_aligned = ImVec2( text_position.x - text_size.x / 2.f, text_position.y );

        if ( text_position.y == bottom_center.y )
            text_position.y -= text_size.y;

        add_text( text_position_aligned, ImColor{ config::visuals_distance_color[ 0 ], config::visuals_distance_color[ 1 ], config::visuals_distance_color[ 2 ], config::visuals_distance_color[ 3 ] }, player_distance.c_str( ), true, 12.f, esp_font );
    }

    void render_health_bar( ImVec2 left_top, ImVec2 left_bottom, float health, ImU32 color ) {
        constexpr const float max_health = 100.f;

        ImVec2 bar_left_top = left_top - ImVec2( 8.f, 0.f );
        ImVec2 bar_right_bottom = left_bottom - ImVec2( 4.f, 0.f );

        ImVec2 bar_right_bottom_health = bar_right_bottom - ImVec2( 1.f, -1.f );
        ImVec2 bar_left_top_health = ImVec2( bar_left_top.x + 1.f, bar_right_bottom_health.y + ( bar_left_top.y - bar_right_bottom.y ) * ( health / max_health ) - 1.f );

        ImGui::GetBackgroundDrawList( )->AddRectFilled( bar_left_top, bar_right_bottom, ImColor( 30, 30, 30, 255 ) );
        ImGui::GetBackgroundDrawList( )->AddRectFilled( bar_right_bottom_health, bar_left_top_health, color );
    }

    void render_skeleton( uint64_t player ) {
        vec2_t positions[ 100 ]{ };

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 7 ], &positions[ 7 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 6 ], &positions[ 6 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 5 ], &positions[ 5 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 9 ], &positions[ 9 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 10 ], &positions[ 10 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 80 ], &positions[ 80 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 43 ], &positions[ 43 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 44 ], &positions[ 44 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 81 ], &positions[ 81 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 3 ], &positions[ 3 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 2 ], &positions[ 2 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 1 ], &positions[ 1 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 65 ], &positions[ 65 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 66 ], &positions[ 66 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 84 ], &positions[ 84 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 85 ], &positions[ 85 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 69 ], &positions[ 69 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 70 ], &positions[ 70 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 86 ], &positions[ 86 ] ) )
            return;

        if ( !world_to_screen( sdk::g_camera_position, sdk::g_bones_position[ player ][ 87 ], &positions[ 87 ] ) )
            return;

        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 7 ].x, positions[ 7 ].y ), ImVec2( positions[ 6 ].x, positions[ 6 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // Head → neck_02
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 6 ].x, positions[ 6 ].y ), ImVec2( positions[ 5 ].x, positions[ 5 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // neck_02 → neck_01
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 5 ].x, positions[ 5 ].y ), ImVec2( positions[ 9 ].x, positions[ 9 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // neck_01 (lower neck) → upperarm_l (left shoulder)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 9 ].x, positions[ 9 ].y ), ImVec2( positions[ 10 ].x, positions[ 10 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // upperarm_l (left shoulder) → lowerarm_l (left elbow)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 10 ].x, positions[ 10 ].y ), ImVec2( positions[ 80 ].x, positions[ 80 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // lowerarm_l (left elbow) → left wrist (предполагаемый 80)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 5 ].x, positions[ 5 ].y ), ImVec2( positions[ 43 ].x, positions[ 43 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // neck_01 (lower neck) → upperarm_r (right shoulder)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 43 ].x, positions[ 43 ].y ), ImVec2( positions[ 44 ].x, positions[ 44 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // upperarm_r (right shoulder) → lowerarm_r (right elbow)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 44 ].x, positions[ 44 ].y ), ImVec2( positions[ 81 ].x, positions[ 81 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // lowerarm_r (right elbow) → right wrist (предполагаемый 81)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 5 ].x, positions[ 5 ].y ), ImVec2( positions[ 3 ].x, positions[ 3 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // neck_01 (lower neck) → spine_02 (abdomen)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 3 ].x, positions[ 3 ].y ), ImVec2( positions[ 2 ].x, positions[ 2 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // spine_02 (abdomen) → spine_01 (mid spine)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 2 ].x, positions[ 2 ].y ), ImVec2( positions[ 1 ].x, positions[ 1 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // spine_01 (mid spine) → pelvis
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 1 ].x, positions[ 1 ].y ), ImVec2( positions[ 65 ].x, positions[ 65 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // pelvis → thigh_l (left hip)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 65 ].x, positions[ 65 ].y ), ImVec2( positions[ 66 ].x, positions[ 66 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // thigh_l (left hip) → calf_l (left knee)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 66 ].x, positions[ 66 ].y ), ImVec2( positions[ 84 ].x, positions[ 84 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // calf_l (left knee) → left ankle (предполагаемый 84)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 84 ].x, positions[ 84 ].y ), ImVec2( positions[ 85 ].x, positions[ 85 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // left ankle (предполагаемый 84) → left foot (предполагаемый 85)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 1 ].x, positions[ 1 ].y ), ImVec2( positions[ 69 ].x, positions[ 69 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // pelvis → thigh_r (right hip)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 69 ].x, positions[ 69 ].y ), ImVec2( positions[ 70 ].x, positions[ 70 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // thigh_r (right hip) → calf_r (right knee)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 70 ].x, positions[ 70 ].y ), ImVec2( positions[ 86 ].x, positions[ 86 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // calf_r (right knee) → right ankle (предполагаемый 86)
        ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( positions[ 86 ].x, positions[ 86 ].y ), ImVec2( positions[ 87 ].x, positions[ 87 ].y ), ImColor{ config::visuals_skeleton_color[ 0 ], config::visuals_skeleton_color[ 1 ],config::visuals_skeleton_color[ 2 ],config::visuals_skeleton_color[ 3 ] } ); // right ankle (предполагаемый 86) → right foot (предполагаемый 87)
    }

    void loop( ) {
        if ( !config::visuals_enabled )
            return;

        for ( int idx{ }; idx < sdk::g_players_array.size( ); ++idx ) {
            auto player = sdk::g_players_array[ idx ];

            if ( !player )
                continue;

            auto location = sdk::g_players_positions[ player ][ 0 ];

            if ( location.x == 0
                || location.y == 0
                || location.z == 0 )
                continue;

            auto distance = static_cast< int >( sdk::g_local_position.distance( location ) / 100.f );

            if ( distance > config::max_distance
                || distance < 0 )
                continue;

            auto health = sdk::g_players_health[ player ][ 0 ];

            if ( sdk::g_players_state[ player ][ 0 ]
                || health > 101 )
                continue;

            auto distance_text = std::to_string( distance ).append( "m" );
            auto head_position = sdk::g_bones_position[ player ][ 7 ];

            vec2_t screen_location{ };
            vec2_t head_location{ };

            if ( !world_to_screen( sdk::g_camera_position, location, &screen_location ) )
                continue;

            if ( !world_to_screen( sdk::g_camera_position, head_position, &head_location ) )
                continue;

            if ( sdk::g_players_names[ player ][ 0 ].empty( ) )
                continue;

            float box_height = std::abs( screen_location.y - head_location.y );
            box_height += box_height / 6.f;

            const float box_width = box_height / 1.f;
            const float half_width = box_width / 2.f;

            const ImVec2 left_top = ImVec2( head_location.x - half_width, screen_location.y - box_height );
            const ImVec2 right_top = ImVec2( head_location.x + half_width, screen_location.y - box_height );

            const ImVec2 left_bottom = ImVec2( head_location.x - half_width, screen_location.y + box_height );
            const ImVec2 right_bottom = ImVec2( head_location.x + half_width, screen_location.y + box_height );

            const ImVec2 top_center = ImVec2( head_location.x, screen_location.y - box_height );
            const ImVec2 bottom_center = ImVec2( head_location.x, screen_location.y + box_height );

            if ( config::visuals_box )
                ImGui::GetBackgroundDrawList( )->AddRect( left_top, right_bottom, ImColor{ config::visuals_box_color[ 0 ], config::visuals_box_color[ 1 ], config::visuals_box_color[ 2 ], config::visuals_box_color[ 3 ] } );

            const auto& converted = to_ansi( sdk::g_players_names[ player ][ 0 ].c_str( ) );

            if ( config::visuals_name ) {
                if ( !converted
                    || sdk::g_players_names[ player ][ 0 ].empty( ) )
                    render_player_name( "player", top_center );
                else
                    render_player_name( converted, top_center );
            }

            if ( config::visuals_distance )
                render_player_distance( distance_text, bottom_center );

            if ( config::visuals_health_bar )
                render_health_bar( left_top, left_bottom, health, ImColor{ config::visuals_health_bar_color[ 0 ], config::visuals_health_bar_color[ 1 ], config::visuals_health_bar_color[ 2 ], config::visuals_health_bar_color[ 3 ] } );
            
            if ( config::visuals_skeleton )
                render_skeleton( player );
        }
    }
}