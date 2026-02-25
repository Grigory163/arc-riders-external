#include "process.hpp"

namespace process {
    uint32_t get_process_id( const char* name ) {
        HANDLE snapshot{ };
        PROCESSENTRY32 pe32{ };
        snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

        if ( snapshot == INVALID_HANDLE_VALUE )
            return 0;

        pe32.dwSize = sizeof( PROCESSENTRY32 ); 
        if ( !Process32First( snapshot, &pe32 ) ) {
            CloseHandle( snapshot );
            return 0;
        }

        do {
            if ( strcmp( name, pe32.szExeFile ) == 0 ) {
                return pe32.th32ProcessID;
                break;
            }
        } while ( Process32Next( snapshot, &pe32 ) );

        CloseHandle( snapshot );
        return 0;
    }

    std::vector< uint32_t > get_process_ids( const char* name ) {
        HANDLE snapshot{ };
        PROCESSENTRY32 pe32{ };
        snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

        std::vector< uint32_t > pids{ };

        if ( snapshot == INVALID_HANDLE_VALUE )
            return { };

        pe32.dwSize = sizeof( PROCESSENTRY32 );
        if ( !Process32First( snapshot, &pe32 ) ) {
            CloseHandle( snapshot );
            return { };
        }

        do {
            if ( strcmp( name, pe32.szExeFile ) == 0 ) {
                pids.emplace_back( pe32.th32ProcessID );
            }
        } while ( Process32Next( snapshot, &pe32 ) );

        CloseHandle( snapshot );
        return pids;
    }

    BOOL __stdcall callback( HWND hwnd, LPARAM lParam ) {
        windows_finder_params_t& params = *( windows_finder_params_t* )lParam;

        unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

        DWORD pid = 0;
        GetWindowThreadProcessId( hwnd, &pid );
        if ( params.pidOwner != NULL )
            if ( params.pidOwner == pid )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        char className[ 255 ] = "";
        GetClassName( hwnd, ( LPSTR )className, 255 );
        std::string classNameWstr = className;
        if ( params.wndClassName != "" )
            if ( params.wndClassName == classNameWstr )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        char windowName[ 255 ] = "";
        GetWindowText( hwnd, ( LPSTR )windowName, 255 );
        std::string windowNameWstr = windowName;
        if ( params.wndName != "" )
            if ( params.wndName == windowNameWstr )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        RECT pos;
        GetWindowRect( hwnd, &pos );
        if ( params.pos.left || params.pos.top || params.pos.right || params.pos.bottom )
            if ( params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        POINT res = { pos.right - pos.left, pos.bottom - pos.top };
        if ( params.res.x || params.res.y )
            if ( res.x == params.res.x && res.y == params.res.y )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        float ratioAllScreensX = res.x / GetSystemMetrics( SM_CXSCREEN );
        float ratioAllScreensY = res.y / GetSystemMetrics( SM_CYSCREEN );
        float percentAllScreens = ratioAllScreensX * ratioAllScreensY * 450;
        if ( params.percentAllScreens != 0.0f )
            if ( percentAllScreens >= params.percentAllScreens )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        RECT desktopRect;
        GetWindowRect( GetDesktopWindow( ), &desktopRect );
        POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
        float ratioMainScreenX = res.x / desktopRes.x;
        float ratioMainScreenY = res.y / desktopRes.y;
        float percentMainScreen = ratioMainScreenX * ratioMainScreenY * 450;
        if ( params.percentMainScreen != 0.0f )
            if ( percentAllScreens >= params.percentMainScreen )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        LONG_PTR style = GetWindowLongPtr( hwnd, GWL_STYLE );
        if ( params.style )
            if ( params.style & style )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        LONG_PTR styleEx = GetWindowLongPtr( hwnd, GWL_EXSTYLE );
        if ( params.styleEx )
            if ( params.styleEx & styleEx )
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        if ( !satisfiedCriteria )
            return TRUE;

        if ( params.satisfyAllCriteria && unSatisfiedCriteria )
            return TRUE;

        params.hwnds.push_back( hwnd );
        return TRUE;
    }

    std::vector< HWND > find( windows_finder_params_t params ) {
        EnumWindows( callback, ( LPARAM )&params );
        return params.hwnds;
    }
}