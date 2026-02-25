#pragma once

#include "../../globals/globals.hpp"
#include "../../driver/driver.hpp"
#include <d3dtypes.h>

#define M_PI (float)3.1415926535

struct vec3_t {
    double x, y, z;

    vec3_t operator-( const vec3_t& other ) const {
        return vec3_t{ x - other.x, y - other.y, z - other.z };
    }

    float dot( const vec3_t& other ) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float distance( const vec3_t& other ) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;

        return sqrtf( dx * dx + dy * dy + dz * dz );
    }
};

struct quat_t {
    double x;
    double y;
    double z;
    double w;
};

struct transform_t {
    quat_t rot;
    vec3_t translation;
    char pad[ 4 ];
    vec3_t scale;
    char pad1[ 4 ];

    D3DMATRIX to_matrix( ) const {
        D3DMATRIX m;

        m._41 = translation.x;
        m._42 = translation.y;
        m._43 = translation.z;

        double x2 = rot.x + rot.x;
        double y2 = rot.y + rot.y;
        double z2 = rot.z + rot.z;

        double xx2 = rot.x * x2;
        double yy2 = rot.y * y2;
        double zz2 = rot.z * z2;

        m._11 = ( 1.0 - ( yy2 + zz2 ) ) * scale.x;
        m._22 = ( 1.0 - ( xx2 + zz2 ) ) * scale.y;
        m._33 = ( 1.0 - ( xx2 + yy2 ) ) * scale.z;

        double yz2 = rot.y * z2;
        double wx2 = rot.w * x2;
        m._32 = ( yz2 - wx2 ) * scale.z;
        m._23 = ( yz2 + wx2 ) * scale.y;

        double xy2 = rot.x * y2;
        double wz2 = rot.w * z2;
        m._21 = ( xy2 - wz2 ) * scale.y;
        m._12 = ( xy2 + wz2 ) * scale.x;

        double xz2 = rot.x * z2;
        double wy2 = rot.w * y2;
        m._31 = ( xz2 + wy2 ) * scale.z;
        m._13 = ( xz2 - wy2 ) * scale.x;

        m._14 = 0.0;
        m._24 = 0.0;
        m._34 = 0.0;
        m._44 = 1.0;

        return m;
    }
};

inline D3DMATRIX matrix_multiplication( D3DMATRIX pM1, D3DMATRIX pM2 ) {
    D3DMATRIX pOut;
    pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
    pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
    pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
    pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
    pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
    pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
    pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
    pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
    pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
    pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
    pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
    pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
    pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
    pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
    pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
    pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

    return pOut;
}

inline vec3_t get_bone( uint64_t bone_array, uint64_t mesh, int index ) {
    auto component_to_world = driver.read< transform_t >( mesh + 0x330 );
    auto bone = driver.read< transform_t >( bone_array + ( index * 0x60 ) );

    const auto& created_matrix = matrix_multiplication( bone.to_matrix( ), component_to_world.to_matrix( ) );
    return vec3_t( created_matrix._41, created_matrix._42, created_matrix._43 );
}

struct rotator_t {
    double pitch, yaw, roll;

    rotator_t operator-( const rotator_t& other ) const {
        return rotator_t{ pitch - other.pitch, yaw - other.yaw, roll - other.roll };
    }
};

struct vec2_t {
    double x, y;
};

struct minimal_view_info_t {
    vec3_t m_location;
    rotator_t m_rotation;

    float m_fov;
};

class matx4x4_t {
public:
    double M[ 4 ][ 4 ];

    matx4x4_t( ) {
        SetIdentity( );
    }

    vec3_t Transform( const vec3_t vector ) {
        vec3_t transformed;

        transformed.x = vector.y * M[ 0 ][ 1 ] + vector.x * M[ 0 ][ 0 ] + vector.z * M[ 0 ][ 2 ] + M[ 0 ][ 3 ];
        transformed.y = vector.y * M[ 1 ][ 1 ] + vector.x * M[ 1 ][ 0 ] + vector.z * M[ 1 ][ 2 ] + M[ 1 ][ 3 ];
        transformed.z = vector.y * M[ 3 ][ 1 ] + vector.x * M[ 3 ][ 0 ] + vector.z * M[ 3 ][ 2 ] + M[ 3 ][ 3 ];

        return transformed;

    }

    matx4x4_t( double m00, double m01, double m02, double m03,
        double m10, double m11, double m12, double m13,
        double m20, double m21, double m22, double m23,
        double m30, double m31, double m32, double m33 ) {
        M[ 0 ][ 0 ] = m00; M[ 0 ][ 1 ] = m01; M[ 0 ][ 2 ] = m02; M[ 0 ][ 3 ] = m03;
        M[ 1 ][ 0 ] = m10; M[ 1 ][ 1 ] = m11; M[ 1 ][ 2 ] = m12; M[ 1 ][ 3 ] = m13;
        M[ 2 ][ 0 ] = m20; M[ 2 ][ 1 ] = m21; M[ 2 ][ 2 ] = m22; M[ 2 ][ 3 ] = m23;
        M[ 3 ][ 0 ] = m30; M[ 3 ][ 1 ] = m31; M[ 3 ][ 2 ] = m32; M[ 3 ][ 3 ] = m33;
    }

    void SetIdentity( ) {
        M[ 0 ][ 0 ] = 1.f; M[ 0 ][ 1 ] = 0.f; M[ 0 ][ 2 ] = 0.f; M[ 0 ][ 3 ] = 0.f;
        M[ 1 ][ 0 ] = 0.f; M[ 1 ][ 1 ] = 1.f; M[ 1 ][ 2 ] = 0.f; M[ 1 ][ 3 ] = 0.f;
        M[ 2 ][ 0 ] = 0.f; M[ 2 ][ 1 ] = 0.f; M[ 2 ][ 2 ] = 1.f; M[ 2 ][ 3 ] = 0.f;
        M[ 3 ][ 0 ] = 0.f; M[ 3 ][ 1 ] = 0.f; M[ 3 ][ 2 ] = 0.f; M[ 3 ][ 3 ] = 1.f;
    }

    matx4x4_t operator*( const matx4x4_t& Other ) const {
        matx4x4_t Result;
        for ( int i = 0; i < 4; ++i ) {
            for ( int j = 0; j < 4; ++j ) {
                Result.M[ i ][ j ] = M[ i ][ 0 ] * Other.M[ 0 ][ j ] +
                    M[ i ][ 1 ] * Other.M[ 1 ][ j ] +
                    M[ i ][ 2 ] * Other.M[ 2 ][ j ] +
                    M[ i ][ 3 ] * Other.M[ 3 ][ j ];
            }
        }
        return Result;
    }

    double operator()( int Row, int Col ) const {
        return M[ Row ][ Col ];
    }

    double& operator()( int Row, int Col ) {
        return M[ Row ][ Col ];
    }

    static matx4x4_t CreateTranslation( double X, double Y, double Z ) {
        matx4x4_t Result;
        Result.M[ 0 ][ 3 ] = X;
        Result.M[ 1 ][ 3 ] = Y;
        Result.M[ 2 ][ 3 ] = Z;
        return Result;
    }

    static matx4x4_t CreateRotationZ( double Radians ) {
        matx4x4_t Result;
        double Cos = std::cos( Radians );
        double Sin = std::sin( Radians );
        Result.M[ 0 ][ 0 ] = Cos;  Result.M[ 0 ][ 1 ] = -Sin;
        Result.M[ 1 ][ 0 ] = Sin;  Result.M[ 1 ][ 1 ] = Cos;
        return Result;
    }

    static matx4x4_t CreateScale( double X, double Y, double Z ) {
        matx4x4_t Result;
        Result.M[ 0 ][ 0 ] = X;
        Result.M[ 1 ][ 1 ] = Y;
        Result.M[ 2 ][ 2 ] = Z;
        return Result;
    }

    matx4x4_t GetTransposed( ) const {
        matx4x4_t Result;
        for ( int i = 0; i < 4; ++i ) {
            for ( int j = 0; j < 4; ++j ) {
                Result.M[ i ][ j ] = M[ j ][ i ];
            }
        }
        return Result;
    }

    vec3_t GetForward( ) {
        return vec3_t( M[ 0 ][ 0 ], M[ 0 ][ 1 ], M[ 0 ][ 2 ] );
    }

    vec3_t GetRight( ) {
        return vec3_t( M[ 1 ][ 0 ], M[ 1 ][ 1 ], M[ 1 ][ 2 ] );
    }

    vec3_t GetUp( ) {
        return vec3_t( M[ 2 ][ 0 ], M[ 2 ][ 1 ], M[ 2 ][ 2 ] );
    }

    vec3_t GetPositionFromMatrix( )
    {
        return vec3_t( M[ 3 ][ 0 ], M[ 3 ][ 1 ], M[ 3 ][ 2 ] );
    }

    vec3_t GetRotationFromMatrix( ) const {
        double pitch = -asinf( M[ 2 ][ 1 ] ); // -Z.x
        double yaw = atan2f( M[ 2 ][ 0 ], M[ 2 ][ 2 ] ); // Z.y, Z.z
        double roll = atan2f( M[ 0 ][ 1 ], M[ 1 ][ 1 ] ); // X.y, Y.y

        // Convert to degrees
        pitch = pitch * ( 180.0f / 3.14159265359f );
        yaw = yaw * ( 180.0f / 3.14159265359f );
        roll = roll * ( 180.0f / 3.14159265359f );

        return { pitch, yaw, roll };
    }
};

inline matx4x4_t create_matrix( vec3_t rot, vec3_t origin ) {
    const double radPitch = rot.x * 0.0174532925199f;
    const double radYaw = rot.y * 0.0174532925199f;
    const double radRoll = rot.z * 0.0174532925199f;

    const double SP = sinf( radPitch );
    const double CP = cosf( radPitch );
    const double SY = sinf( radYaw );
    const double CY = cosf( radYaw );
    const double SR = sinf( radRoll );
    const double CR = cosf( radRoll );

    matx4x4_t matrix;
    matrix.M[ 0 ][ 0 ] = CP * CY;
    matrix.M[ 0 ][ 1 ] = CP * SY;
    matrix.M[ 0 ][ 2 ] = SP;
    matrix.M[ 0 ][ 3 ] = 0.f;

    matrix.M[ 1 ][ 0 ] = SR * SP * CY - CR * SY;
    matrix.M[ 1 ][ 1 ] = SR * SP * SY + CR * CY;
    matrix.M[ 1 ][ 2 ] = -SR * CP;
    matrix.M[ 1 ][ 3 ] = 0.f;

    matrix.M[ 2 ][ 0 ] = -( CR * SP * CY + SR * SY );
    matrix.M[ 2 ][ 1 ] = CY * SR - CR * SP * SY;
    matrix.M[ 2 ][ 2 ] = CR * CP;
    matrix.M[ 2 ][ 3 ] = 0.f;

    matrix.M[ 3 ][ 0 ] = origin.x;
    matrix.M[ 3 ][ 1 ] = origin.y;
    matrix.M[ 3 ][ 2 ] = origin.z;
    matrix.M[ 3 ][ 3 ] = 1.f;

    return matrix;
}

inline bool world_to_screen( minimal_view_info_t cameraManager, vec3_t WorldLocation, vec2_t* ScreenPosition ) {
    vec3_t Screenlocation( 0, 0, 0 );
    vec3_t rot = vec3_t( cameraManager.m_rotation.pitch, cameraManager.m_rotation.yaw, cameraManager.m_rotation.roll );

    const matx4x4_t tempMatrix = create_matrix( rot, vec3_t( 0, 0, 0 ) );

    vec3_t vAxisX( tempMatrix.M[ 0 ][ 0 ], tempMatrix.M[ 0 ][ 1 ], tempMatrix.M[ 0 ][ 2 ] );
    vec3_t vAxisY( tempMatrix.M[ 1 ][ 0 ], tempMatrix.M[ 1 ][ 1 ], tempMatrix.M[ 1 ][ 2 ] );
    vec3_t vAxisZ( tempMatrix.M[ 2 ][ 0 ], tempMatrix.M[ 2 ][ 1 ], tempMatrix.M[ 2 ][ 2 ] );

    vec3_t vDelta = WorldLocation - cameraManager.m_location;
    vec3_t vTransformed = vec3_t( vDelta.dot( vAxisY ), vDelta.dot( vAxisZ ), vDelta.dot( vAxisX ) );

    if ( vTransformed.z < 1.f )
        vTransformed.z = 1.f;

    int centrex = g_width / 2;
    int centrey = g_height / 2;

    double fov = cameraManager.m_fov;

    const double FOV_DEG_TO_RAD = static_cast< float >( 3.14159265358979323846 ) / 360.f;

    Screenlocation.x = centrex + vTransformed.x * ( centrey / tanf( fov * FOV_DEG_TO_RAD ) ) / vTransformed.z;
    Screenlocation.y = centrey - vTransformed.y * ( centrey / tanf( fov * FOV_DEG_TO_RAD ) ) / vTransformed.z;

    ScreenPosition->x = Screenlocation.x;
    ScreenPosition->y = Screenlocation.y;

    return true;
}

inline bool in_fov( int fovX, int fovY, int aimFOV, int x, int y ) {
    float dist = sqrt( abs( fovX - x ) * abs( fovX - x ) + abs( fovY - y ) * abs( fovY - y ) );
    return ( dist < aimFOV ) ? true : false;
}

inline vec2_t last = vec2_t( 0.0, 0.0 );

inline vec2_t calculate_view_angles( const vec3_t& src, const vec3_t& dst, const vec2_t currentAngles, float smoothness ) {
    auto delta = dst - src;

    double hypotenuse = sqrt( delta.x * delta.x + delta.y * delta.y );
    double pitch = atan( delta.z / hypotenuse ) * ( 180.0 / 3.14159265358979323846 );
    double yaw = atan2( delta.y, delta.x ) * ( 180.0 / 3.14159265358979323846 );

    auto ret = currentAngles;

    last = currentAngles;
    last.x = fmod( last.x + 180.0, 360.0 ) - 180.0;
    last.y = fmod( last.y + 180.0, 360.0 ) - 180.0;

    pitch = fmod( pitch + 180.0, 360.0 ) - 180.0;
    yaw = fmod( yaw + 180.0, 360.0 ) - 180.0;

    double delta_x = pitch - currentAngles.x;
    double delta_y = yaw - currentAngles.y;

    if ( delta_x > 180.0 ) delta_x -= 360.0;
    if ( delta_x < -180.0 ) delta_x += 360.0;
    if ( delta_y > 180.0 ) delta_y -= 360.0;
    if ( delta_y < -180.0 ) delta_y += 360.0;

    double max_step = 3.0;
    delta_x = std::clamp( delta_x, -max_step, max_step );
    delta_y = std::clamp( delta_y, -max_step, max_step );

    float factor = 1.0f - ( smoothness / 100.0f );
    ret.x += delta_x * factor;
    ret.y += delta_y * factor;

    ret.x = fmod( ret.x + 180.0, 360.0 ) - 180.0;
    ret.y = fmod( ret.y + 180.0, 360.0 ) - 180.0;

    return ret;
}
