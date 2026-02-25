#pragma once

#include <windows.h>

class c_time {
private:
    SYSTEMTIME original_time{ };
    bool time_modified{ };

public:
    c_time( ) : time_modified( false ) {
        GetSystemTime( &original_time );
    }

    ~c_time( ) {
        restore( );
    }

    bool warp( ) {
        PrepareNTPSync( );

        SYSTEMTIME current_time{ };
        GetSystemTime( &current_time );

        original_time = current_time;

        SYSTEMTIME new_time = current_time;
        new_time.wYear -= 1;

        if ( new_time.wMonth == 2 && new_time.wDay == 29 
             && !is_leap_year( new_time.wYear ) ) {
            new_time.wDay = 28;
        }

        if ( SetSystemTime( &new_time ) ) {
            time_modified = true;

            if ( validate_time_change( ) ) {
                return true;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

private:
    bool is_leap_year( WORD year ) {
        return ( year % 4 == 0 && year % 100 != 0 ) || ( year % 400 == 0 );
    }

    bool PrepareNTPSync( ) {
        HKEY hKey;
        LONG result;


        HMODULE hAdvapi32 = LoadLibraryW( L"advapi32.dll" );
        auto RegCreateKeyExW = ( decltype( &::RegCreateKeyExW ) )GetProcAddress( hAdvapi32, "RegCreateKeyExW" );
        auto RegSetValueExW = ( decltype( &::RegSetValueExW ) )GetProcAddress( hAdvapi32, "RegSetValueExW" );
        auto RegCloseKey = ( decltype( &::RegCloseKey ) )GetProcAddress( hAdvapi32, "RegCloseKey" );

        result = RegCreateKeyExW( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters", 0, 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, 0, &hKey, 0 );

        if ( result != ERROR_SUCCESS )
            return false;

        const wchar_t* ntpType = L"NTP";
        result = RegSetValueExW( hKey, L"Type", 0, REG_SZ, ( const BYTE* )ntpType, ( wcslen( ntpType ) + 1 ) * sizeof( wchar_t ) );

        RegCloseKey( hKey );
        return result == ERROR_SUCCESS;
    }

    bool validate_time_change( ) {
        DWORD uptime = GetTickCount64( ) / 1000;

        FILETIME ft{ };
        GetSystemTimeAsFileTime( &ft );

        ULARGE_INTEGER current{ };
        current.LowPart = ft.dwLowDateTime;
        current.HighPart = ft.dwHighDateTime;

        ULONG unixTime = ( ULONG )( ( current.QuadPart - 116444736000000000ULL ) / 10000000ULL );

        if ( uptime > unixTime )
            return false;

        return true;
    }

public:
    void restore( ) {
        if ( time_modified ) {
            if ( SetSystemTime( &original_time ) ) {
                time_modified = false;
            }
        }
    }

    void show_current_time( ) {
        SYSTEMTIME current;
        GetSystemTime( &current );
    }

    bool is_time_modified( ) {
        return time_modified;
    }
};