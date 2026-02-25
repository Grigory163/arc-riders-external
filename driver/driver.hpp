#pragma once

#include "../includes/includes.hpp"
#include "../includes/definitions/definitions.hpp"

#include "initialization/initialization.hpp"

#include "authentication/otp.hpp"
#include "authentication/time/time.hpp"

class c_driver {
    HANDLE handle{ };

public:
    uint32_t pid{ };
    uint64_t dirbase{ };
    uint64_t peb{ };

    bool initialize( ) {
        const int maxRetries = 3;
        bool success = false;
        int retryDelay = 1000;

        c_time manager{ };

        manager.warp( );

        for ( int retry = 0; retry < maxRetries; ++retry ) {
            if ( !driver_init( ) )
                continue;

            handle = CreateFileA( "\\\\.\\OracleNeural939", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

            if ( !handle || ( handle == INVALID_HANDLE_VALUE ) )
                continue;

            success = authDriver( );
            break;
        }

        manager.restore( );

        return success;
    }

    uint64_t get_dirbase( ) {
        _GETdirbase request{ };

        request.process_id = pid;
        request.dirbase = 0;

        DeviceIoControl( handle, IOCTL_GET_DIRBASE, &request, sizeof( request ), &request, sizeof( request ), 0, 0 );

        dirbase = request.dirbase;

        return dirbase;
    }

    uint64_t get_base_address( uint32_t process_id ) {
        GET_PROCESS_BASE_ADDRESS_DATA request{ };

        request.process_id = process_id;

        DeviceIoControl( handle, IOCTL_GET_PROCESS_BASE_ADD, &request, sizeof( request ), &request, sizeof( request ), 0, 0 );

        return request.baseAddress;
    }

    bool attach( uint32_t processid ) {
        pid = processid;
        get_dirbase( );

        return true;
    }

    bool authDriver( ) {
        auto otp = GenerateOTP( );

        DWORD bytesReturned;

        auto status = DeviceIoControl( handle, IOCTL_VERIFY, &otp, sizeof( otp ), 0, 0, &bytesReturned, 0 );

        return status;
    }

    auto readvm( uint32_t process_id, uint64_t src_addr, uint64_t dst_addr, size_t size ) -> ULONG {
        if ( process_id == 0 || src_addr == 0 ) return 0;

        MEMORY_OPERATION_DATA request = { process_id, src_addr, dst_addr, size, dirbase };

        return DeviceIoControl(
            handle,
            IOCTL_READ_MEMORY,
            &request,
            sizeof( request ),
            &request,
            sizeof( request ),
            0,
            0
        );
    }

    auto writevm( uint32_t process_id, uint64_t src_addr, uint64_t dst_addr, size_t size ) -> void {
        if ( process_id == 0 || src_addr == 0 ) 
            return;

        MEMORY_OPERATION_DATA request = { process_id, src_addr, dst_addr, size, dirbase };

        DeviceIoControl(
            handle,
            IOCTL_WRITE_MEMORY,
            &request,
            sizeof( request ),
            &request,
            sizeof( request ),
            0,
            0
        );
    }

    template < typename T >
    T read( uint64_t src, size_t size = sizeof( T ) ) {
        T buffer{ };
        readvm( pid, src, ( uint64_t )&buffer, size );

        return buffer;
    }

    template < typename T >
    void write( uint64_t src, T data, size_t size = sizeof( T ) ) {
        writevm( pid, src, ( uint64_t )&data, size );
    }
};

inline c_driver driver;