#include "emulator.hpp"

#include <cstdarg>
#include <cstring>
#include <windows.h>
#include "../../driver/driver.hpp"

//#include "ECTest/Exploit/internal-hook.hpp"

// Global VEH handler
static PVOID g_veh_handler = nullptr;
static bool g_ignore_violations = false;

// Vectored Exception Handler to catch and suppress access violations during emulation
LONG WINAPI UnicornExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
    if (g_ignore_violations) {
        DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;
        if (code == EXCEPTION_ACCESS_VIOLATION ||
            code == EXCEPTION_DATATYPE_MISALIGNMENT ||
            code == EXCEPTION_ARRAY_BOUNDS_EXCEEDED ||
            code == EXCEPTION_IN_PAGE_ERROR) {
            // Suppress the exception - terminate the current operation silently
            // This will cause Unicorn to stop but won't crash the program
            return EXCEPTION_EXECUTE_HANDLER;
        }
    }
    // Pass other exceptions along
    return EXCEPTION_CONTINUE_SEARCH;
}

EmulatorEngine::EmulatorEngine(uint64_t proc, bool enable_cache, uintptr_t STACK_BASE) : uc(nullptr), process(proc), initialized(false), use_cache(enable_cache), STACK_BASE(STACK_BASE),
    emulation_counter(0), stack_pointer(STACK_BASE + EMU_STACK_SIZE - 0x1000),
    debug_mode(false) {
    ctx = std::make_unique<EmulationContext>();
    ctx->process = proc;
}

bool EmulatorEngine::setup( uint64_t base ) {
    uc = 0;
    initialized = false;
    use_cache = true;
    STACK_BASE = 0x7FFF0000;
    emulation_counter = 0;
    stack_pointer = STACK_BASE + EMU_STACK_SIZE - 0x1000;
    ctx = std::make_unique<EmulationContext>( );
    ctx->process = base;

    return true;
}

EmulatorEngine::~EmulatorEngine() {
    Reset();
}

void EmulatorEngine::printf(const char* fmt, ...) {
    if (!debug_mode) return;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

bool EmulatorEngine::Initialize() {
    if (initialized) {
        printf("Already initialized\n");
        return true;
    }

    // Install VEH handler if not already installed
    if (!g_veh_handler) {
        g_veh_handler = AddVectoredExceptionHandler(1, UnicornExceptionHandler);
        printf("Installed VEH handler\n");
    }

    uc_err err = uc_open(UC_ARCH_X86, UC_MODE_64, &uc);
    if (err != UC_ERR_OK) {
        printf("Failed to initialize Unicorn: %s\n", uc_strerror(err));
        return false;
    }

    // Map stack
    err = uc_mem_map(uc, STACK_BASE, EMU_STACK_SIZE, UC_PROT_READ | UC_PROT_WRITE);
    if (err != UC_ERR_OK) {
        printf("Failed to map stack: %s\n", uc_strerror(err));
        uc_close(uc);
        uc = nullptr;
        return false;
    }

    // Initialize stack with zeros
    std::vector<uint8_t> stack_init(EMU_STACK_SIZE, 0);
    uc_mem_write(uc, STACK_BASE, stack_init.data(), EMU_STACK_SIZE);

    // Add memory hooks
    uc_hook_add(uc, &hook_mem,
        UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED |
        UC_HOOK_MEM_FETCH_UNMAPPED | UC_HOOK_MEM_FETCH_PROT |
        UC_HOOK_MEM_READ_PROT | UC_HOOK_MEM_WRITE_PROT |
        UC_HOOK_MEM_INVALID,
        (void*)dynamic_memory_hook, ctx.get(), 1, 0);

    initialized = true;
    printf("Initialization complete\n");
    return true;
}

void EmulatorEngine::Reset() {
    if (uc) {
        uc_close(uc);
        uc = nullptr;
    }
    initialized = false;
    ctx->mapped_pages.clear();
    ClearCache();
}

void EmulatorEngine::ClearCache() {
    std::lock_guard<std::mutex> lock(cache_mutex);
    page_cache.clear();
    function_cache.clear();
}

bool EmulatorEngine::dynamic_memory_hook(uc_engine* uc, uc_mem_type type, uint64_t address,
    int size, int64_t value, void* user_data) {
    EmulationContext* ctx = static_cast<EmulationContext*>(user_data);

    // Calculate page base (4KB aligned)
    uint64_t page_base = address & ~0xFFF;

    // Check for intercept callback
    if (ctx->intercept_callback && (type == UC_MEM_FETCH_UNMAPPED || type == UC_MEM_FETCH_PROT)) {
        if (ctx->intercept_callback(address)) {
            return true;  // Callback handled it
        }
    }

    // Check if we've already mapped this page
    if (ctx->mapped_pages.find(page_base) != ctx->mapped_pages.end()) {
        return false;  // Already mapped, something else is wrong
    }

    // Map the page
    constexpr size_t PAGE_SIZE = 0x1000;
    uc_err err = uc_mem_map(uc, page_base, PAGE_SIZE, UC_PROT_ALL);
    if (err != UC_ERR_OK) {
        return false;
    }

    // Read the page data from target process
    std::vector<uint8_t> page_data(PAGE_SIZE);
    if (!driver.readvm(driver.pid, page_base, ( uint64_t )page_data.data(), PAGE_SIZE)) {
        // If read fails, fill with zeros
        memset(page_data.data(), 0, PAGE_SIZE);
    }

    // Write it to emulator
    err = uc_mem_write(uc, page_base, page_data.data(), PAGE_SIZE);
    if (err != UC_ERR_OK) {
        return false;
    }

    // Mark page as mapped
    ctx->mapped_pages[page_base] = true;

    return true;
}

void EmulatorEngine::code_hook(uc_engine* uc, uint64_t address, uint32_t size, void* user_data) {
    // Can be used for debugging/tracing
}

bool EmulatorEngine::MapFunctionCode( uintptr_t func_addr, size_t size ) {
    printf( "[+] map_function_code: func_address=0x%llX size=0x%zX\n",
        func_addr, size );

    // --- Map a code region around the function in Unicorn ---
    uintptr_t code_page_base = ( func_addr & ~0xFFFULL );
    constexpr size_t CODE_MAP_SIZE = 0x10000; // 64KB

    if ( ctx->mapped_pages.find( code_page_base ) == ctx->mapped_pages.end( ) ) {
        uc_err err = uc_mem_map( uc, code_page_base, CODE_MAP_SIZE, UC_PROT_ALL );
        printf( "[+] uc_mem_map( base=0x%llX, size=0x%zX ) -> %d (%s)\n",
            code_page_base, CODE_MAP_SIZE, err, uc_strerror( err ) );
        if ( err != UC_ERR_OK ) {
            return false;
        }

        for ( size_t i = 0; i < CODE_MAP_SIZE; i += 0x1000 )
            ctx->mapped_pages[ code_page_base + i ] = true;
    }

    // --- Only read inside the current 4K page from the game process ---
    const size_t offset_in_page = func_addr & 0xFFFULL;
    const size_t bytes_left_in_page = 0x1000 - offset_in_page;
    const size_t readSize = std::min<size_t>( size, bytes_left_in_page );

    printf( "[+] func_address=0x%llX offset_in_page=0x%zX bytes_left_in_page=0x%zX read_size=0x%zX\n",
        func_addr, offset_in_page, bytes_left_in_page, readSize );

    std::vector<uint8_t> func_code( readSize );

    driver.readvm( driver.pid, func_addr, ( uint64_t )func_code.data( ), readSize );

    uc_err err = uc_mem_write(uc, func_addr, func_code.data( ), readSize);
    printf("[+] uc_mem_write(func_address=0x%llX, size=0x%zX) -> %d (%s)\n",
        func_addr, readSize, err, uc_strerror(err));
    if (err != UC_ERR_OK) {
        return false;
    }

    return true;
}

bool EmulatorEngine::SetupStack() {
    // Reset stack pointer
    stack_pointer = STACK_BASE + EMU_STACK_SIZE - 0x1000;

    // Set up a fake return address (0 will cause execution to stop)
    uint64_t fake_return = 0x0;
    uc_mem_write(uc, stack_pointer, &fake_return, sizeof(fake_return));

    // Set RSP
    uc_reg_write(uc, UC_X86_REG_RSP, &stack_pointer);

    return true;
}

//bool EmulatorEngine::WriteParameters(const std::vector<uint64_t>& params) {
//    // x64 calling convention: RCX, RDX, R8, R9, then stack
//    if (params.size() > 0) uc_reg_write(uc, UC_X86_REG_RCX, &params[0]);
//    if (params.size() > 1) uc_reg_write(uc, UC_X86_REG_RDX, &params[1]);
//    if (params.size() > 2) uc_reg_write(uc, UC_X86_REG_R8, &params[2]);
//    if (params.size() > 3) uc_reg_write(uc, UC_X86_REG_R9, &params[3]);
//
//    // Additional parameters go on the stack
//    if (params.size() > 4) {
//        for (size_t i = 4; i < params.size(); i++) {
//            uint64_t stack_offset = 0x20 + ((i - 4) * 8);  // Shadow space + param offset
//            uc_mem_write(uc, stack_pointer + stack_offset, &params[i], sizeof(uint64_t));
//        }
//    }
//
//    return true;
//}
bool EmulatorEngine::WriteParameters(const std::vector<uint64_t>& params) {
    if (params.size() > 0) {
        // normal RCX
        uc_reg_write(uc, UC_X86_REG_RCX, &params[0]);

        // HACK/feature: also seed RBX with the first arg,
        // so methods that expect "this" in RBX (like your camera stub) work.
        uc_reg_write(uc, UC_X86_REG_RBX, &params[0]);
    }
    if (params.size() > 1) uc_reg_write(uc, UC_X86_REG_RDX, &params[1]);
    if (params.size() > 2) uc_reg_write(uc, UC_X86_REG_R8, &params[2]);
    if (params.size() > 3) uc_reg_write(uc, UC_X86_REG_R9, &params[3]);

    if (params.size() > 4) {
        for (size_t i = 4; i < params.size(); i++) {
            uint64_t stack_offset = 0x20 + ((i - 4) * 8);
            uc_mem_write(uc, stack_pointer + stack_offset, &params[i], sizeof(uint64_t));
        }
    }

    return true;
}

EmulationResult EmulatorEngine::ReadRegisters() {
    EmulationResult result;
    result.success = true;

    uc_reg_read(uc, UC_X86_REG_RAX, &result.rax);
    uc_reg_read(uc, UC_X86_REG_RCX, &result.rcx);
    uc_reg_read(uc, UC_X86_REG_RDX, &result.rdx);
    uc_reg_read(uc, UC_X86_REG_RBX, &result.rbx);
    uc_reg_read(uc, UC_X86_REG_RSI, &result.rsi);
    uc_reg_read(uc, UC_X86_REG_RDI, &result.rdi);
    uc_reg_read(uc, UC_X86_REG_R8, &result.r8);
    uc_reg_read(uc, UC_X86_REG_R9, &result.r9);
    uc_reg_read(uc, UC_X86_REG_R10, &result.r10);
    uc_reg_read(uc, UC_X86_REG_R11, &result.r11);
    uc_reg_read(uc, UC_X86_REG_R12, &result.r12);
    uc_reg_read(uc, UC_X86_REG_R13, &result.r13);
    uc_reg_read(uc, UC_X86_REG_R14, &result.r14);
    uc_reg_read(uc, UC_X86_REG_R15, &result.r15);
    uc_reg_read(uc, UC_X86_REG_RSP, &result.rsp);
    uc_reg_read(uc, UC_X86_REG_RBP, &result.rbp);
    uc_reg_read(uc, UC_X86_REG_RIP, &result.rip);

    return result;
}

EmulationResult EmulatorEngine::EmulateFunction(uintptr_t func_offset,
    const std::vector<uint64_t>& params,
    uint32_t max_instructions)
{
    EmulationResult result{};
    result.success = false;

    // Enable ignoring access violations during emulation
    g_ignore_violations = true;

    /*__try
    {*/
        if (!initialized)
        {
            if (!Initialize())
            {
                result.error_message = "Failed to initialize emulator";
                g_ignore_violations = false;
                return result;
            }
        }

        uintptr_t func_addr = process + func_offset;
        printf("[Emu] Emulating function at 0x%llX (base: 0x%llX + offset: 0x%llX)\n",
            (unsigned long long)func_addr,
            (unsigned long long)process,
            (unsigned long long)func_offset);

        // Clear mapped pages for this run (but keep cache)
        ctx->mapped_pages.clear();

        // Map function code (0x400 is plenty for your small stubs)
        if (!MapFunctionCode(func_addr, 0x400))
        {
            result.error_message = "Failed to map function code";
            g_ignore_violations = false;
            return result;
        }

        // Setup stack
        if (!SetupStack())
        {
            result.error_message = "Failed to setup stack";
            g_ignore_violations = false;
            return result;
        }

        // Write parameters (RCX/RDX/R8/R9 + stack, and mirrors first param into RBX)
        if (!WriteParameters(params))
        {
            result.error_message = "Failed to write parameters";
            g_ignore_violations = false;
            return result;
        }

        // Set RIP to function start
        uc_reg_write(uc, UC_X86_REG_RIP, &func_addr);

        // Execute
        uc_err err = uc_emu_start(uc, func_addr, 0, 0, max_instructions);

        if (err != UC_ERR_OK && err != UC_ERR_INSN_INVALID)
        {
            printf("[Emu] Emulation error: %s\n", uc_strerror(err));
            result = ReadRegisters();
            result.success = false;
            result.error_message = uc_strerror(err);
            g_ignore_violations = false;
            return result;
        }

        // Read results
        result = ReadRegisters();
        emulation_counter++;

        printf("[Emu] Emulation complete. RAX=0x%llX\n",
            (unsigned long long)result.rax);
    //}
    //__except (EXCEPTION_EXECUTE_HANDLER)
    //{
    //    // Catch any SEH exceptions from Unicorn
    //    result.error_message = "SEH exception caught in EmulateFunction";
    //    result.success = false;

    //    // Try to read registers anyway
    //    __try
    //    {
    //        result = ReadRegisters();
    //    }
    //    __except (EXCEPTION_EXECUTE_HANDLER)
    //    {
    //        // Failed to read registers
    //    }
    //}

    // Disable ignoring violations after emulation
    g_ignore_violations = false;

    return result;
}

uint64_t EmulatorEngine::EmulateCameraManager(uintptr_t func_offset)
{
    uint64_t decrypt_func_addr = process + 0x2A8CD79;
    uint64_t decrypt_func_ret = process + 0x2A8CE89;

    uint64_t rsp_val = STACK_BASE + EMU_STACK_SIZE - 0x1000;
    uc_reg_write(uc, UC_X86_REG_RSP, &rsp_val);
    uc_reg_write(uc, UC_X86_REG_RAX, &func_offset);
	
    {
        // Put a fake TEB just below your emu stack.
    // Adjust if this collides with other ranges you map.
        const uint64_t teb_base = STACK_BASE - 0x2000;
        const size_t   teb_size = 0x2000;

        uc_err err = uc_mem_map(uc, teb_base, teb_size, UC_PROT_READ | UC_PROT_WRITE);
        if (err != UC_ERR_OK)
        {
            printf("set_teb: uc_mem_map failed: %s\n", uc_strerror(err));
            return 0;
        }

        // Zero the region
        std::vector<uint8_t> zero(teb_size, 0);
        uc_mem_write(uc, teb_base, zero.data(), teb_size);

        // NT_TIB.Self = pointer to TEB (at offset 0x00)
        uint64_t self = teb_base;
        uc_mem_write(uc, teb_base + 0x00, &self, sizeof(self));

        // Fake PEB somewhere after TEB (we don't really care about its contents)
        uint64_t peb = teb_base + 0x1000;

        // On Windows:
        // - x86: FS:[0x30] -> PEB
        // - x64: GS:[0x60] (and a lot of tools still treat 0x30 as "interesting")
        // We write the same fake PEB pointer to both offsets so either pattern can work.
        uc_mem_write(uc, teb_base + 0x30, &peb, sizeof(peb)); // in case something expects this
        uc_mem_write(uc, teb_base + 0x60, &peb, sizeof(peb)); // or this
    }

    auto err = uc_emu_start(uc, decrypt_func_addr, decrypt_func_ret, 0, 0);
    if (err != UC_ERR_OK)
    {
        uint64_t rip;
        uc_reg_read(uc, UC_X86_REG_RIP, &rip);
        printf("Emulation failed (RIP: %llx, err: %s)",
            rip, uc_strerror(err));
        return 0;
    }

    uint64_t rax = 0;
    uc_reg_read(uc, UC_X86_REG_RAX, &rax);
    return rax;
}
/*
execute from 0x2396ED to 0x23996C included
pass a buffer to R14 and the index to EBP
it works for 0 for instance (it gives you None or 10 = IntProperty)
but I have weird numbers in the UObjects
so it crashes
I don't know if my UObject iteration is faulty
or if there's another encryption at FName level

public string GetFName(uint index)
{
    _u.SetReg(X86.UC_X86_REG_R14, 0x666000);
    _u.SetReg(X86.UC_X86_REG_EBP, index);
    var bufferPtr = _u.Execute((long)_nameRva, _nameUntilRva, X86.UC_X86_REG_R14);

    return Encoding.UTF8.GetTerminatedString(Buffer);
}
where _nameRva = 0x2396ED and _nameUntilRva = 0x23996C


public string GetFName(uint index)
{
    var gnameSig = _scanner.FindPattern("89 E8 C1 E8 ?? 0F B7 F5");
    if (!gnameSig.Found)
        throw new Exception("gname sig not found.");
    ulong funcRva = (ulong)gnameSig.Offset;
    _d.SetIp(funcRva);
    long untilRva = (long)_d.JumpToNextMnemonic(Mnemonic.Call).IP;

    _u.SetReg(X86.UC_X86_REG_EBP, index);
    var length = _u.Execute((long)funcRva, untilRva, X86.UC_X86_REG_R8D);
    var entry = _u.ReadReg(X86.UC_X86_REG_RDX);
    return Encoding.UTF8.GetString(_driver.Read<byte>((ulong)entry, (int)length));
}
*/
std::string EmulatorEngine::EmulateGetGname(uintptr_t func_offset, uint64_t size, int index)
{
    uint64_t decrypt_func_addr = process + func_offset;
    uint64_t decrypt_func_ret = process + func_offset + size;

    uint64_t rsp_val = STACK_BASE + EMU_STACK_SIZE - 0x1000;
    uc_reg_write(uc, UC_X86_REG_RSP, &rsp_val);
    uc_reg_write(uc, UC_X86_REG_EBP, &index);
	
    {
        // Put a fake TEB just below your emu stack.
        // Adjust if this collides with other ranges you map.
        const uint64_t teb_base = STACK_BASE - 0x2000;
        const size_t   teb_size = 0x2000;

        uc_err err = uc_mem_map(uc, teb_base, teb_size, UC_PROT_READ | UC_PROT_WRITE);
        if (err != UC_ERR_OK)
        {
            printf("set_teb: uc_mem_map failed: %s\n", uc_strerror(err));
            return 0;
        }

        // Zero the region
        std::vector<uint8_t> zero(teb_size, 0);
        uc_mem_write(uc, teb_base, zero.data(), teb_size);

        // NT_TIB.Self = pointer to TEB (at offset 0x00)
        uint64_t self = teb_base;
        uc_mem_write(uc, teb_base + 0x00, &self, sizeof(self));

        // Fake PEB somewhere after TEB (we don't really care about its contents)
        uint64_t peb = teb_base + 0x1000;

        // On Windows:
        // - x86: FS:[0x30] -> PEB
        // - x64: GS:[0x60] (and a lot of tools still treat 0x30 as "interesting")
        // We write the same fake PEB pointer to both offsets so either pattern can work.
        uc_mem_write(uc, teb_base + 0x30, &peb, sizeof(peb)); // in case something expects this
        uc_mem_write(uc, teb_base + 0x60, &peb, sizeof(peb)); // or this
    }

    auto err = uc_emu_start(uc, decrypt_func_addr, decrypt_func_ret, 0, 0);
    if (err != UC_ERR_OK)
    {
        uint64_t rip;
        uc_reg_read(uc, UC_X86_REG_RIP, &rip);
        printf("Emulation failed (RIP: %llx, err: %s)",
            rip, uc_strerror(err));
        return 0;
    }

    uint64_t rax = 0;
    int length = 0;
    uc_reg_read(uc, UC_X86_REG_R8D, &length);
    uc_reg_read(uc, UC_X86_REG_RDX, &rax);
    char buffer[256];
    driver.readvm( driver.pid, rax, ( uint64_t )&buffer, (int)length);
    std::string text = std::string(buffer, length);
    return text;
}


EmulationResult EmulatorEngine::EmulateFunctionWithStruct(uintptr_t func_offset,
    const void* struct_data,
    size_t struct_size,
    uint32_t max_instructions) {
    // Allocate struct in emulator memory
    uint64_t struct_addr = stack_pointer - 0x1000 - struct_size;

    // Write struct data
    uc_mem_write(uc, struct_addr, struct_data, struct_size);

    // Call function with struct pointer as first parameter
    return EmulateFunction(func_offset, { struct_addr }, max_instructions);
}

void EmulatorEngine::SetInterceptCallback(std::function<bool(uint64_t)> callback) {
    ctx->intercept_callback = callback;
}
