#include "menu.hpp"

#include <imgui.h>

inline bool is_waiting_for_key{};
inline int waiting_button_id{};

inline static const char* keyNames[ ] =
{
	"...",
	"Left Mouse",
	"Right Mouse",
	"Cancel",
	"Middle Mouse",
	"Mouse 5",
	"Mouse 4",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
};

inline static bool Items_ArrayGetter( void* data, int idx, const char** out_text ) {
	const char* const* items = ( const char* const* )data;
	if ( out_text )
		*out_text = items[ idx ];
	return true;
}

inline void ChangeKey( int* key, int button_id ) {
	if ( !key ) return;

	if ( waiting_button_id != button_id ) return;

	is_waiting_for_key = true;
	while ( is_waiting_for_key ) {
		for ( int i = 0; i < 0x87; ++i ) {
			if ( GetAsyncKeyState( i ) & 0x8000 ) {
				*key = i;
				is_waiting_for_key = false;
				waiting_button_id = -1;
				return;
			}
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

inline void hotkey( int* key, int button_id ) {
	if ( !key ) return;

	ImGui::PushID( button_id );

	const char* preview_value = "Select a key";
	if ( *key >= 0 && *key < IM_ARRAYSIZE( keyNames ) ) {
		Items_ArrayGetter( keyNames, *key, &preview_value );
	}

	std::string button_text = ( is_waiting_for_key && waiting_button_id == button_id ) ? "Press a key" : preview_value;
	if ( ImGui::Button( button_text.c_str( ), ImVec2( 80, 17 ) ) && !is_waiting_for_key ) {
		waiting_button_id = button_id;
		std::thread( &ChangeKey, key, button_id ).detach( );
	}
	ImGui::PopID( );
}

namespace menu {
	void loop( ) {
		ImGui::SetNextWindowSize( { 400, 400 } );
		ImGui::Begin( "Nexup" );

		ImGuiStyle& style = ImGui::GetStyle();
		{
			style.Alpha = 0.90f;
			style.WindowPadding = ImVec2(10, 10);
			style.WindowRounding = 5.0f;
			style.FramePadding = ImVec2(3, 3);
			style.FrameRounding = 3.0f;
			style.ItemSpacing = ImVec2(12, 8);
			style.ItemInnerSpacing = ImVec2(8, 6);
			style.IndentSpacing = 25.0f;
			style.ScrollbarSize = 15.0f;
			style.ScrollbarRounding = 9.0f;
			style.GrabMinSize = 5.0f;
			style.GrabRounding = 3.0f;

			ImVec4 red = ImVec4(0.788f, 0.106f, 0.055f, 1.0f); // #c91b0e
			ImVec4 black = ImVec4(0.059f, 0.059f, 0.059f, 1.0f); // #0f0f0f

			style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.0f);
			style.Colors[ImGuiCol_WindowBg] = black;
			style.Colors[ImGuiCol_ChildBg] = black;
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);

			style.Colors[ImGuiCol_Border] = red;
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
			style.Colors[ImGuiCol_FrameBgHovered] = red;
			style.Colors[ImGuiCol_FrameBgActive] = red;

			style.Colors[ImGuiCol_TitleBg] = black;
			style.Colors[ImGuiCol_TitleBgCollapsed] = black;
			style.Colors[ImGuiCol_TitleBgActive] = red;

			style.Colors[ImGuiCol_MenuBarBg] = black;

			style.Colors[ImGuiCol_Button] = black;
			style.Colors[ImGuiCol_ButtonHovered] = red;
			style.Colors[ImGuiCol_ButtonActive] = red;

			style.Colors[ImGuiCol_Header] = black;
			style.Colors[ImGuiCol_HeaderHovered] = red;
			style.Colors[ImGuiCol_HeaderActive] = red;

			style.Colors[ImGuiCol_Tab] = black;
			style.Colors[ImGuiCol_TabHovered] = red;
			style.Colors[ImGuiCol_TabActive] = red;

			ImGuiIO& io = ImGui::GetIO();

		}

		if ( ImGui::CollapsingHeader( "aimbot##main" ) ) {
			ImGui::Checkbox( "enable aim", &config::aimbot_enabled ); { ImGui::SameLine( ); hotkey( &config::aimbot_key, 0 ); }
			ImGui::Checkbox( "draw fov", &config::aimbot_draw_fov );
			ImGui::SliderFloat( "fov", &config::aimbot_fov, 0.f, 800.f );
			ImGui::SliderFloat( "smooth", &config::aimbot_smooth, 0.f, 30.f );

			const char* bones_list[ ]{ "head", "neck", "pelvis" };

			ImGui::Combo( "bone", &config::aimbot_bone, bones_list, ARRAYSIZE( bones_list ) );
		}

		if ( ImGui::CollapsingHeader( "visuals##main" ) ) {
			ImGui::Checkbox( "enable esp", &config::visuals_enabled );

			ImGui::Checkbox( "box", &config::visuals_box ); { ImGui::SameLine( ); ImGui::ColorEdit4( "##box_color", config::visuals_box_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ); }
			ImGui::Checkbox( "name", &config::visuals_name ); { ImGui::SameLine( ); ImGui::ColorEdit4( "##name_color", config::visuals_name_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ); }
			ImGui::Checkbox( "distance", &config::visuals_distance ); { ImGui::SameLine( ); ImGui::ColorEdit4( "##distance_color", config::visuals_distance_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ); }
			ImGui::Checkbox( "health bar", &config::visuals_health_bar ); { ImGui::SameLine( ); ImGui::ColorEdit4( "##health_bar_color", config::visuals_health_bar_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ); }
			ImGui::Checkbox( "skeleton", &config::visuals_skeleton ); { ImGui::SameLine( ); ImGui::ColorEdit4( "##skeleton_color", config::visuals_skeleton_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ); }
			ImGui::SliderFloat( "max distance", &config::max_distance, 0.f, 1000.f );
		}

		ImGui::End( );
	}
}