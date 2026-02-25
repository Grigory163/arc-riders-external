#pragma once

#include "../../includes/includes.hpp"

namespace memory {
	__forceinline bool is_valid( const uint64_t address ) {
		if ( address >= 0x10000 && address <= 0x7FFFFFFEFFFF )
			return true;

		return false;
	}

	__forceinline bool is_valid( const void* address ) {
		if ( reinterpret_cast< uint64_t >( address ) >= 0x10000 && reinterpret_cast< uint64_t >( address ) <= 0x7FFFFFFEFFFF )
			return true;

		return false;
	}
}