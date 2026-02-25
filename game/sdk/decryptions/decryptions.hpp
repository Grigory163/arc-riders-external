#pragma once

#include "../../../includes/includes.hpp"

#include "../../../driver/driver.hpp"
#include "../../../globals/globals.hpp"

#include "../../../utilities/memory/memory.hpp"
#include "../../../utilities/unicorn/emulator.hpp"
#include <emmintrin.h>

namespace sdk::decryptions {
    inline uint64_t game_instance( uint64_t uworld ) {
        __m128i v1;

        uint64_t mem_data_1_addr = uworld + ( 0x32 * sizeof( __m128i ) );
        __m128i temp_mem_data = driver.read< __m128i >( mem_data_1_addr );
        v1 = _mm_shufflelo_epi16( temp_mem_data, 0x72 );

        __m128i slli_result = _mm_slli_epi16( v1, 0xFu );
        __m128i srli_result = _mm_srli_epi16( v1, 1u );
        __m128i rotated_v1 = _mm_or_si128( slli_result, srli_result );
        uint64_t temp_mask_u64 = driver.read< uint64_t >( g_game_base + 0xB09A400 );
        __m128i mask_v = _mm_loadl_epi64( ( const __m128i* ) & temp_mask_u64 );
        __m128i shuffled_result = _mm_shuffle_epi8( rotated_v1, mask_v );
        uint64_t game_instance = shuffled_result.m128i_u64[ 0 ];

        return game_instance;
    }

    inline uint64_t uworld( ) {
        struct DecryptParam {
            uint64_t padding[ 3 ];
            uint64_t inner_ptr;
        };

        struct InnerStruct {
            uint64_t padding[ 6 ];
            uint64_t value;
        };

        InnerStruct inner{ };
        inner.value = 0;

        DecryptParam param{ };

        uint64_t inner_addr = 0x7FFF0000 + EMU_STACK_SIZE - 0x400;
        uint64_t param_addr = 0x7FFF0000 + EMU_STACK_SIZE - 0x300;

        param.inner_ptr = inner_addr;

        auto uc = uworld_emu->GetEmulator( );

        uc_mem_write( uc, inner_addr, &inner, sizeof( inner ) );
        uc_mem_write( uc, param_addr, &param, sizeof( param ) );

        EmulationResult result = uworld_emu->EmulateFunction( 0x2A576FE, { param_addr } );

        if ( !result.success ) {
            printf( "[+] emulation failed: %s\n", result.error_message.c_str( ) );

            uworld_emu->Reset( );
            uworld_emu->Initialize( );
            return 0;
        }

        return result.rax;
    }

    inline uint64_t player_camera_manager( uint64_t player_controller ) {
        player_camera_manager_emulator->Reset( );
        player_camera_manager_emulator->Initialize( );

        const auto& uc = player_camera_manager_emulator->GetEmulator( );

        uc_reg_write( uc, UC_X86_REG_RAX, &player_controller );

        const auto& status = player_camera_manager_emulator->EmulateFunction( 0x2A9403E );

        if ( !status.success )
            return 0;

        uint64_t rax{ };
        uc_reg_read( uc, UC_X86_REG_RAX, &rax );

        return rax;
    }

    inline uint64_t decrypt_bone_array( uint64_t mesh ) {
        if ( !mesh )
            return 0;

        unsigned int v3;
        __m128i si128;
        unsigned __int64 v5;
        __m128i v6;
        __int64 v7;
        __int64 v8;

        v3 = ( 0xB4EE5C2 )
            + 16777619
            * ( ( unsigned int )( ( 0xB4EE5C2 )
                + 16777619
                * __ROL4__(
                    ( 0xB4EE5C2 )
                    + ( ( unsigned __int64 )( mesh + 1920 ) >> 32 )
                    + 16777619 * __ROL4__( ( 0xB4EE5C2 ) + 16777619 * __ROL4__( mesh + 1920, 13 ), 24 ),
                    13 ) ) >> 8 );

        uint8_t block1[ 16 ];
        if ( !driver.readvm( driver.pid, mesh + 16 * ( ( ( unsigned __int8 )v3 ^ BYTE2( v3 ) ) & 7u ) + 1920, ( uint64_t )block1, 16 ) )
            return 0;

        si128 = _mm_loadu_si128( ( const __m128i* )block1 );
        v5 = _mm_cvtsi128_si64( _mm_or_si128( _mm_slli_epi16( si128, 7u ), _mm_srli_epi16( si128, 9u ) ) ) ^ 0x672DC3D08AD4878ALL;

        uint8_t block2[ 16 ];
        if ( !driver.readvm( driver.pid, mesh + 16 * ( ( ( ( unsigned __int8 )v3 ^ BYTE2( v3 ) ) + 1 ) & 7u ) + 1920, ( uint64_t )block2, 16 ) )
            return 0;
        v6 = _mm_loadu_si128( ( const __m128i* )block2 );

        v7 = __ROL8__( v5, 27 )
            + ( __ROL8__( _mm_cvtsi128_si64( _mm_or_si128( _mm_slli_epi16( v6, 7u ), _mm_srli_epi16( v6, 9u ) ) ) ^ 0x672DC3D08AD4878ALL, 27 )
                ^ ( 0x100000001B3LL * __ROL8__( 0x100000001B3LL * __ROL8__( v5, 57 ) + 0x4A477A8D5E37482LL, 33 ) + 0x4A477A8D5E37482LL ) );

        int32_t offset_multiplier = 0;
        if ( !driver.readvm( driver.pid, mesh + 2180, ( uint64_t )&offset_multiplier, sizeof( int32_t ) ) )
            return 0;

        v8 = 16LL * offset_multiplier;
        return ( uint64_t )( v8 + v7 + 168 );
    }
}