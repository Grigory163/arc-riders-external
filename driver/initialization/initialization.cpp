#include "../../includes/includes.hpp"
#include "resources.hpp"

LPCWSTR serviceName = L"cocacola";
LPCWSTR driverPath = L"C:\\Windows\\System32\\drivers\\cocacola.sys";

bool writeDriverFile( const wchar_t* filePath, const unsigned char* data, size_t dataSize ) {
    FILE* file = _wfopen( filePath, L"wb" );

    if ( !file )       
        return false;
    
    fwrite( data, 1, dataSize, file );
    fclose( file );
    return true;
}

bool driver_init( ) {
    SC_HANDLE hSCManager{ };
    SC_HANDLE hService{ };

    bool result{ };

    if ( GetFileAttributesW( driverPath ) == INVALID_FILE_ATTRIBUTES ) {
        if ( !writeDriverFile( driverPath, rawData, sizeof( rawData ) ) ) {
            return false;
        }
    }

    hSCManager = OpenSCManagerW( 0, 0, SC_MANAGER_ALL_ACCESS );

    if ( !hSCManager )
        return false;

    hService = OpenServiceW( hSCManager, serviceName, SERVICE_ALL_ACCESS );

    if ( !hService ) {
        hService = CreateServiceW(
            hSCManager,
            serviceName,
            serviceName,
            SERVICE_ALL_ACCESS,
            SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_NORMAL,
            driverPath,
            0,
            0,
            0,
            0,
            0
        );

        if ( !hService ) {
           CloseServiceHandle( hSCManager );
            return false;
        }
    }

    SERVICE_STATUS_PROCESS ssp{ };
    DWORD bytes{ };

    if ( QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, ( LPBYTE )&ssp, sizeof( SERVICE_STATUS_PROCESS ), &bytes ) ) {
        if ( ssp.dwCurrentState != SERVICE_RUNNING ) {
            if ( StartServiceW( hService, 0, 0 ) ) {
                result = true;
            } 
        } else {
            result = true;
        }
    }

    if ( hService ) 
       CloseServiceHandle( hService );

    if ( hSCManager )
        CloseServiceHandle( hSCManager );

    return result;
}