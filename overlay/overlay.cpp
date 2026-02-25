#include "../features/features.hpp"
#include "overlay.hpp"

#include "../globals/globals.hpp"
#include "../game/game.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

#include "menu/menu.hpp"

#include "../utilities/process/process.hpp"
#include "../utilities/memory/memory.hpp"
#include "../utilities/importer/importer.hpp"

#include <mmsystem.h>
#include "resource/font.hpp"
#pragma comment( lib, "winmm.lib" )

inline static bool open{ };

namespace overlay {
    bool hijack( ) {
        hwnd = FindWindowA( "Worker Window", 0 );

        if ( !hwnd )
            return false;

        SetMenu( hwnd, 0 );

        MARGINS margin{ -1 };
        LI_FN( DwmExtendFrameIntoClientArea ).get( )( hwnd, &margin );

        const auto style = GetWindowLong( hwnd, GWL_STYLE );
        const auto exstyle = GetWindowLong( hwnd, GWL_EXSTYLE );

        SetWindowLongPtrA( hwnd, GWL_EXSTYLE, exstyle | WS_EX_LAYERED | WS_EX_TRANSPARENT );
        SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, g_width, g_height, SWP_SHOWWINDOW );
        SetLayeredWindowAttributes( hwnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );

        ShowWindow( hwnd, SW_SHOW );
        UpdateWindow( hwnd );

        return true;
    }

    bool create_swap_chain( ) {
        DXGI_RATIONAL refresh_rate{ };
        DXGI_MODE_DESC buffer_desc{ };
        DXGI_SAMPLE_DESC sample_desc{ };
        DXGI_SWAP_CHAIN_DESC swapchain_desc{ };

        refresh_rate.Numerator = 0;
        refresh_rate.Denominator = 1;

        buffer_desc.Width = 0;
        buffer_desc.Height = 0;
        buffer_desc.RefreshRate = refresh_rate;
        buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        buffer_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        buffer_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        sample_desc.Count = 1;
        sample_desc.Quality = 0;
   
        swapchain_desc.BufferDesc = buffer_desc;
        swapchain_desc.SampleDesc = sample_desc;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferCount = 2;
        swapchain_desc.OutputWindow = hwnd;
        swapchain_desc.Windowed = 1;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        D3D_FEATURE_LEVEL w_feature_level{ };
        D3D_FEATURE_LEVEL w_feature_level_array[ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

        const auto& status = LI_FN( D3D11CreateDeviceAndSwapChain ).get( )( 0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, w_feature_level_array, 2, D3D11_SDK_VERSION, &swapchain_desc, &swapchain, &device, &w_feature_level, &device_context );

        if ( FAILED( status ) )
            return false;

        ID3D11Texture2D* render_buffer{ };

        if ( FAILED( swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID* >( &render_buffer ) ) ) )
            return false;
        
        if ( FAILED( device->CreateRenderTargetView( render_buffer, nullptr, &render_target ) ) )
            return false;

        render_buffer->Release( );

        return true;
    }

    bool prepare_imgui( ) {
        ImGui::CreateContext( );

        ImGui_ImplWin32_Init( hwnd );
        ImGui_ImplDX11_Init( device, device_context );

        ImFontConfig font_cfg{ };

        ImGui::GetIO( ).IniFilename = 0;
        ImGui::GetIO( ).LogFilename = 0;

        font_cfg.FontDataOwnedByAtlas = false;

        esp_font = ImGui::GetIO( ).Fonts->AddFontFromMemoryTTF( resources::font::sf_pro, sizeof( resources::font::sf_pro ), 12.0f, &font_cfg, ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );
        
        return true;
    }

	bool initialize( ) {
        if ( !hijack( ) )
            return false;

        if ( !create_swap_chain( ) )
            return false;

        if ( !prepare_imgui( ) )
            return false;

		return true;
	}

    void loop( ) {
        LI_FN( timeBeginPeriod ).get( )( 1 );

        while ( msg.message != WM_QUIT ) {
            if ( PeekMessageA( &msg, hwnd, 0, 0, PM_REMOVE ) ) {
                TranslateMessage( &msg );
                DispatchMessageA( &msg );
            }

            POINT p{ };
            GetCursorPos( &p );

            ImGuiIO& io = ImGui::GetIO( );

            io.MousePos.x = p.x;
            io.MousePos.y = p.y;

            io.MouseDown[ 0 ] = ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0;
            io.MouseDown[ 1 ] = ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) != 0;
            io.MouseDown[ 2 ] = ( GetAsyncKeyState( VK_MBUTTON ) & 0x8000 ) != 0;
            io.MouseDown[ 3 ] = ( GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000 ) != 0;
            io.MouseDown[ 4 ] = ( GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000 ) != 0;
     
            if ( GetAsyncKeyState( VK_LBUTTON ) ) {
                io.MouseDown[ 0 ] = true;
                io.MouseClicked[ 0 ] = true;
                io.MouseClickedPos[ 0 ].x = io.MousePos.x;
                io.MouseClickedPos[ 0 ].x = io.MousePos.y;
            } else {
                io.MouseDown[ 0 ] = false;
            }
            
            io.KeyCtrl = ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0;
            io.KeyShift = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
            io.KeyAlt = ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) != 0;
            io.KeySuper = ( GetAsyncKeyState( VK_LWIN ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_RWIN ) & 0x8000 ) != 0;

            auto framestart = std::chrono::high_resolution_clock::now( );

            if ( GetAsyncKeyState( VK_INSERT ) & 1 )
                open = !open;

            ImGui_ImplDX11_NewFrame( );
            ImGui_ImplWin32_NewFrame( );
            ImGui::NewFrame( );

            if ( config::aimbot_draw_fov )	
                ImGui::GetBackgroundDrawList( )->AddCircle( { g_width / 2.f, g_height / 2.f }, config::aimbot_fov, IM_COL32( 255, 255, 255, 255 ), 100 );

            if ( open )
                menu::loop( );

            features::loop( );
            ImGui::Render( );

            const float color[ ]{ 0, 0, 0, 0 };

            device_context->OMSetRenderTargets( 1, &render_target, nullptr );
            device_context->ClearRenderTargetView( render_target, color );
            ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

            swapchain->Present( false, 0 );

            auto frameend = std::chrono::high_resolution_clock::now( );
            std::chrono::duration< double > elapsed = frameend - framestart;
            double frametime = elapsed.count( );
            double sleeptime = 0.0085 - frametime;

            if ( sleeptime > 0 )
                std::this_thread::sleep_for( std::chrono::duration< double >( sleeptime ) );
        }

        ImGui_ImplDX11_Shutdown( );
        ImGui_ImplWin32_Shutdown( );
        ImGui::DestroyContext( );

        DestroyWindow( hwnd );
        LI_FN( timeEndPeriod ).get( )( 1 );
    }
}