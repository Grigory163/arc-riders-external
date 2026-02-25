#pragma once
#include "unicorn/unicorn.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>
//#include "../ex/exploit.hpp"

// Memory layout for emulation
constexpr size_t EMU_STACK_SIZE = 0x10000;  // 64KB stack

// Structure to hold emulation results
struct EmulationResult {
    bool success;
    uint64_t rax;  // Return value
    uint64_t rcx, rdx, rbx, rsi, rdi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rsp, rbp, rip;
    std::string error_message;
};

// Context passed to hooks
struct EmulationContext {
    uint64_t process;
    std::unordered_map<uint64_t, bool> mapped_pages;
    std::function<bool(uint64_t)> intercept_callback;  // Optional callback for intercepting calls
};

class EmulatorEngine {
private:
    uc_engine* uc;
    uint64_t process;
    std::unique_ptr<EmulationContext> ctx;

    // Caching
    std::unordered_map<uint64_t, std::vector<uint8_t>> page_cache;
    std::unordered_map<uintptr_t, std::vector<uint8_t>> function_cache;
    std::mutex cache_mutex;

    uintptr_t STACK_BASE;
    bool initialized;
    bool use_cache;
    uint64_t emulation_counter;

    // Stack management
    uint64_t stack_pointer;

    // Hooks
    uc_hook hook_mem;
    uc_hook hook_code;

    static bool dynamic_memory_hook(uc_engine* uc, uc_mem_type type, uint64_t address,
        int size, int64_t value, void* user_data);
    static void code_hook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data);

public:
    EmulatorEngine(uint64_t proc, bool enable_cache = true, uintptr_t STACK_BASE = 0x7FFF0000);
    bool setup( uint64_t base );
    ~EmulatorEngine();

    // Initialize emulator (call once)
    bool Initialize();

    // Cleanup and reset
    void Reset();

    // Clear cache
    void ClearCache();

    // Emulate a function with given parameters
    EmulationResult EmulateFunction(uintptr_t func_offset,
        const std::vector<uint64_t>& params = {},
        uint32_t max_instructions = 50000);

    uint64_t EmulateCameraManager(uintptr_t func_offset);
    std::string EmulateGetGname(uintptr_t func_offset, uint64_t size, int index);

    // Emulate with custom struct parameter
    EmulationResult EmulateFunctionWithStruct(uintptr_t func_offset,
        const void* struct_data,
        size_t struct_size,
        uint32_t max_instructions = 50000);

    // Set intercept callback (for hooking specific function calls)
    void SetInterceptCallback(std::function<bool(uint64_t)> callback);

    // Utility methods
    uint64_t GetModuleBase() const { return process; }
    bool IsInitialized() const { return initialized; }

    // Cache statistics
    size_t GetCacheSize() const { return page_cache.size(); }
    uint64_t GetEmulationCount() const { return emulation_counter; }

    // Enable/disable debug output
    void SetDebugMode(bool enable) { debug_mode = enable; }

    // Get internal emulator for advanced usage
    uc_engine* GetEmulator() { return uc; }

private:
    bool debug_mode = true;

    // Internal helpers
    bool MapFunctionCode(uintptr_t func_addr, size_t size = 0x2000);
    bool SetupStack();
    bool WriteParameters(const std::vector<uint64_t>& params);
    EmulationResult ReadRegisters();
    void printf(const char* fmt, ...);
};

inline EmulatorEngine* uworld_emu;
inline EmulatorEngine* player_camera_manager_emulator;