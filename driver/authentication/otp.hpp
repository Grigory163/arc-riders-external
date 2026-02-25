#pragma once

#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>

const WCHAR g_Seed[ ] = L"EclipseMeridian";

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

#define HASH_LENGTH 32
#define ROL32(x,n)  ( ((x) << (n)) | ((x) >> (32 - (n))) )

//------------------------------------------------------------------------------
inline NTSTATUS
GenerateHash(
    _In_reads_bytes_( dataSize )  BYTE* data,
    _In_                        ULONG dataSize,
    _Out_writes_bytes_( hashBufferSize ) BYTE* hashBuffer,
    _In_                        ULONG hashBufferSize
) {
    if ( hashBufferSize < HASH_LENGTH || !data || !hashBuffer )
        return STATUS_INVALID_PARAMETER;

    ULONG h0 = 0x6a09e667, h1 = 0xbb67ae85,
        h2 = 0x3c6ef372, h3 = 0xa54ff53a,
        h4 = 0x510e527f, h5 = 0x9b05688c,
        h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

    for ( ULONG i = 0; i < dataSize; i++ ) {
        ULONG v = data[ i ];
        h0 = ROL32( h0 ^ v, 7 ) + h7;
        h1 = ROL32( h1 ^ v, 11 ) + h0;
        h2 = ROL32( h2 ^ v, 13 ) + h1;
        h3 = ROL32( h3 ^ v, 17 ) + h2;
        h4 = ROL32( h4 ^ v, 19 ) + h3;
        h5 = ROL32( h5 ^ v, 23 ) + h4;
        h6 = ROL32( h6 ^ v, 29 ) + h5;
        h7 = ROL32( h7 ^ v, 31 ) + h6;
    }

    ( ( ULONG* )hashBuffer )[ 0 ] = h0;
    ( ( ULONG* )hashBuffer )[ 1 ] = h1;
    ( ( ULONG* )hashBuffer )[ 2 ] = h2;
    ( ( ULONG* )hashBuffer )[ 3 ] = h3;
    ( ( ULONG* )hashBuffer )[ 4 ] = h4;
    ( ( ULONG* )hashBuffer )[ 5 ] = h5;
    ( ( ULONG* )hashBuffer )[ 6 ] = h6;
    ( ( ULONG* )hashBuffer )[ 7 ] = h7;

    return STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
// dynamic RtlGetVersion
inline void
GetWindowsBuildNumber(
    _Out_ WCHAR* buildNumber,
    _In_  ULONG  bufferChars
) {
    typedef NTSTATUS( WINAPI* RtlGetVersionPtr )( PRTL_OSVERSIONINFOW );
    HMODULE h = GetModuleHandleW( L"ntdll.dll" );
    if ( h ) {
        auto fn = ( RtlGetVersionPtr )GetProcAddress( h, "RtlGetVersion" );
        if ( fn ) {
            RTL_OSVERSIONINFOW os = { .dwOSVersionInfoSize = sizeof( os ) };
            if ( fn( &os ) == STATUS_SUCCESS ) {
                swprintf_s( buildNumber,
                            bufferChars,
                            L"%u",
                            os.dwBuildNumber );
                return;
            }
        }
    }
    swprintf_s( buildNumber, bufferChars, L"UNKNOWN" );
}

//------------------------------------------------------------------------------
// simple GetComputerName
inline void
GetComputerNameWstr(
    _Out_ WCHAR* computerName,
    _In_  ULONG  bufferChars
) {
    DWORD len = bufferChars;
    GetComputerNameW( computerName, &len );
}

//------------------------------------------------------------------------------
// build-and-hash exactly like kernel
inline ULONG
GenerateOTPWithTime(
    _In_ ULONG timeStamp
) {
    BYTE   hash[ HASH_LENGTH ] = { 0 };
    WCHAR  buildNumber[ 16 ] = { 0 };
    WCHAR  computerName[ 64 ] = { 0 };
    WCHAR  combined[ 256 ] = { 0 };

    // pieces
    GetWindowsBuildNumber( buildNumber, ARRAYSIZE( buildNumber ) );
    // printf("Build Number: %ws\n", buildNumber);

    GetComputerNameWstr( computerName, ARRAYSIZE( computerName ) );
    // printf("Computer Name: %ws\n", computerName);

    // printf("Seed: %ws\n", g_Seed);

    // combine
    swprintf_s( combined,
                ARRAYSIZE( combined ),
                L"%ws%ws%lu%ws",
                buildNumber,
                computerName,
                timeStamp,
                g_Seed );
    // printf("Combined Data: %ws\n", combined);

    // exact byte-count
    ULONG dataSize = ( ULONG )( wcslen( combined ) * sizeof( WCHAR ) );
    // printf("Combined length: %u chars, %u bytes\n",
    //    (ULONG)wcslen(combined),
    //    dataSize);

    // hash
    NTSTATUS hs = GenerateHash( ( BYTE* )combined,
                                dataSize,
                                hash,
                                sizeof( hash ) );
    // printf("GenerateHash status: 0x%08lx\n", hs);

    if ( hs == STATUS_SUCCESS ) {
        // dump state
        ULONG* st = ( ULONG* )hash;
        // printf("Hash State: %08x %08x %08x %08x %08x %08x %08x %08x\n",
        //    st[0], st[1], st[2], st[3],
        //    st[4], st[5], st[6], st[7]);
    }

    // OTP
    ULONG otp = *( ULONG* )hash;
    // printf("Generated OTP: %lu\n", otp);
    return otp;
}

//------------------------------------------------------------------------------
// 30-sec window
inline ULONG
GenerateOTP( ) {
    FILETIME ft;
    GetSystemTimeAsFileTime( &ft );
    ULARGE_INTEGER uli = { ft.dwLowDateTime, ft.dwHighDateTime };

    ULONG seconds = ( ULONG )( uli.QuadPart / 10000000ULL );
    ULONG window = seconds / 30;

    return GenerateOTPWithTime( window );
}
