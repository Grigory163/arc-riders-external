 #include "includes/includes.hpp"

#include "driver/driver.hpp"
#include "globals/globals.hpp"
#include "game/game.hpp"
#include "overlay/overlay.hpp"
#include "features/aimbot/aimbot.hpp"

#include "utilities/process/process.hpp"
#include "utilities/unicorn/emulator.hpp"

int main( ) {
	{
		const auto& d3d11 = LoadLibraryA( "d3d11.dll" );
		const auto& d3dx11 = LoadLibraryA( "d3dx11_43.dll" );
		const auto& dwmapi = LoadLibraryA( "dwmapi.dll" );
		const auto& winmm = LoadLibraryA( "winmm.dll" );
		const auto& advapi32 = LoadLibraryA( "advapi32.dll" );
		const auto& d3dcompiler_47 = LoadLibraryA( "d3dcompiler_47.dll" );

		printf( "[ takemenow ] d3d11: 0x%p\n", d3d11 );
		printf( "[ takemenow ] d3dx11: 0x%p\n", d3dx11 );
		printf( "[ takemenow ] dwmapi: 0x%p\n", dwmapi );
		printf( "[ takemenow ] winmm: 0x%p\n", winmm );
		printf( "[ takemenow ] advapi32: 0x%p\n", advapi32 );
		printf( "[ takemenow ] d3dcompiler_47: 0x%p\n", d3dcompiler_47 );
	}

	if ( !driver.initialize( ) ) {
		printf( "[ takemenow ] failed to initalize driver\n" );

		std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
		return 0;
	}

	printf( "[ takemenow ] driver initialized\n" );
	printf( "[ takemenow ] press f5 in lobby\n" );

	while ( !GetAsyncKeyState( VK_F5 ) & 1 )
		Sleep( 100 );

	uint32_t pid{ };
	while ( !( pid = process::get_process_id( "PioneerGame.exe" ) ) )
		Sleep( 100 );

	driver.attach( pid );

	g_game_base = driver.get_base_address( pid );

	uworld_emu = new EmulatorEngine( g_game_base, false );

	uworld_emu->Reset( );
	uworld_emu->Initialize( );

	player_camera_manager_emulator = new EmulatorEngine( g_game_base, false );

	player_camera_manager_emulator->Reset( );
	player_camera_manager_emulator->Initialize( );

	g_width = GetSystemMetrics( SM_CXSCREEN );
	g_height = GetSystemMetrics( SM_CYSCREEN );

	printf( "[ takemenow ] PioneerGame.exe\n" );
	printf( "              PID:  0x%x\n", pid );
	printf( "              MZj   0x%p\n", driver.read< uint64_t >( g_game_base ) );
	printf( "              BASE: 0x%p\n", g_game_base );

	std::thread( game::cache_uworld ).detach( );
	std::thread( game::cache_list ).detach( );
	std::thread( game::update_camera ).detach( );
	std::thread( game::cache_local_player ).detach( );
	std::thread( game::cache_positions ).detach( );
	std::thread( game::cache_player_names ).detach( );
	std::thread( game::cache_healths ).detach( );
	std::thread( game::cache_bone_positions ).detach( );
	std::thread( aimbot::loop ).detach( );

	overlay::initialize( );
	std::thread( overlay::loop ).detach( );

	Sleep( 0xffffffff );

	return 1;
}