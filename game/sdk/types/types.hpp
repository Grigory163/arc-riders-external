#pragma once

#include "../../../includes/includes.hpp"

namespace sdk {
    class array_t {
    public:
        int length( ) const {
            return m_count;
        }

        bool is_valid( ) const {
            if ( m_count > m_max )
                return false;

            if ( !m_data )
                return false;

            return true;
        }

        int size( ) const {
            return m_count;
        }

        std::vector< uint64_t > get_data_list( ) {
            std::vector< uint64_t > actor_list( m_count );

            driver.readvm( driver.pid, m_data, ( uint64_t )actor_list.data( ), m_count * sizeof( uint64_t ) );

            return actor_list;
        }

        uint64_t get_address( ) const {
            return m_data;
        }
    protected:
        uint64_t m_data;
        uint32_t m_count;
        uint32_t m_max;
    };

    struct string_t : public array_t {
        std::wstring to_wstring( ) const {
            if ( !m_data )
                return std::wstring( L"" );

            wchar_t buffer[ 64 ]{ };
            driver.readvm( driver.pid, m_data, ( uint64_t )buffer, 64 * sizeof( wchar_t ) );

            return std::wstring( buffer );
        }

        std::string to_string( ) const {
            auto ws = to_wstring( );
            auto str = std::string( ws.begin( ), ws.end( ) );

            return str;
        }
    };

    enum e_bones : int {
        Root = 0,
        pelvis = 1,
        spine_01 = 2,
        spine_02 = 3,
        spine_03 = 4,
        neck_01 = 5,
        neck_02 = 6,
        Head = 7,
        clavicle_l = 8,
        upperarm_l = 9,
        lowerarm_l = 10,
        hand_l = 11,
        index_01_l = 12,
        index_02_l = 13,
        index_03_l = 14,
        index_04_l = 15,
        ring_01_l = 16,
        ring_02_l = 17,
        ring_03_l = 18,
        ring_04_l = 19,
        pinky_01_l = 20,
        pinky_02_l = 21,
        pinky_03_l = 22,
        pinky_04_l = 23,
        middle_01_l = 24,
        middle_02_l = 25,
        middle_03_l = 26,
        middle_04_l = 27,
        thumb_01_l = 28,
        thumb_02_l = 29,
        thumb_03_l = 30,
        weapon_buffer = 31,
        weapon_root = 32,
        weapon_mag = 33,
        weapon_belt_01 = 34,
        weapon_belt_02 = 35,
        weapon_belt_03 = 36,
        weapon_belt_04 = 37,
        weapon_belt_05 = 38,
        weapon_belt_06 = 39,
        weapon_dynamic_part_attach_01 = 40,
        prop_01 = 41,
        clavicle_r = 42,
        upperarm_r = 43,
        lowerarm_r = 44,
        hand_r = 45,
        index_01_r = 46,
        index_02_r = 47,
        index_03_r = 48,
        index_04_r = 49,
        ring_01_r = 50,
        ring_02_r = 51,
        ring_03_r = 52,
        ring_04_r = 53,
        pinky_01_r = 54,
        pinky_02_r = 55,
        pinky_03_r = 56,
        pinky_04_r = 57,
        middle_01_r = 58,
        middle_02_r = 59,
        middle_03_r = 60,
        middle_04_r = 61,
        thumb_01_r = 62,
        thumb_02_r = 63,
        thumb_03_r = 64,
        thigh_l = 65,
        calf_l = 66,
        Foot_L = 67,
        ball_l = 68,
        thigh_r = 69,
        calf_r = 70,
        Foot_R = 71,
        ball_r = 72,
        ik_foot_root = 73,
        ik_foot_l = 74,
        ik_foot_r = 75,
        auxroot = 76,
        ik_hand_root = 77,
        ik_hand_l = 78,
        ik_hand_r = 79
    };
}