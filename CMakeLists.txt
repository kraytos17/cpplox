cmake_minimum_required(VERSION 3.28)

enable_language(CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_DEBUG_POSTFIX "-d")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpplox)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/chunk.cpp
    src/chunk.hpp
    src/debug.hpp
    src/debug.cpp
    src/value.hpp
    src/value.cpp
    src/vm.hpp
    src/vm.cpp
    src/compiler.hpp
    src/compiler.cpp
    src/scanner.hpp
    src/scanner.cpp
    src/object.hpp
    src/object.cpp
    src/forward_decl.hpp
    src/inline_decl.hpp
)

# Compiler and linker flags for safety
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    
    # General compile warnings and sanitizers for both Clang and GCC
    add_compile_options(
        # Warning flags
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
        -Wsign-conversion
        -Wcast-align
        -Wformat=2
        -Wunused
        -Wnull-dereference
        -Wdouble-promotion
        -Wold-style-cast
        -Wcast-qual
        -Woverloaded-virtual
        -Wshadow
        -Wstrict-aliasing=2
        -Wnon-virtual-dtor
        -Wsign-promo
        -Wlogical-op
        -Wunsafe-loop-optimizations
        -Wctad-maybe-unsupported
        -Wpessimizing-move
        -Wrange-loop-construct
        -Wlifetime                     # Lifetime analysis
        -Wdeprecated                   # Deprecation warnings
        -Wstrict-overflow               # Catch potential overflow optimizations
        -Wstack-protector               # Check stack protection
        -fstack-protector-strong        # Stack protection
        -fno-omit-frame-pointer         # Debugging aid, keep frame pointer
        -fstack-clash-protection        # Stack clash protection
        -fvisibility=hidden             # Hide symbols by default
        -Wfloat-equal                   # Warn on floating-point comparisons
    )

    # Sanitizers for runtime checks
    add_compile_options(
        -fsanitize=address              # Address sanitizer
        -fsanitize=undefined            # Undefined behavior sanitizer
        -fsanitize=leak                 # Leak sanitizer
        # -fsanitize=thread              # Uncomment to enable thread sanitizer
    )

    add_link_options(
        -fsanitize=address              # Address sanitizer
        -fsanitize=undefined            # Undefined behavior sanitizer
        -fsanitize=leak                 # Leak sanitizer
        # -fsanitize=thread              # Uncomment to enable thread sanitizer
        -fPIE                           # Position Independent Executable
        -pie                            # Enable position independent code execution
        -fstack-protector-strong        # Stack protection
    )

    # Debug-specific options
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(
            -fno-omit-frame-pointer     # Don't omit frame pointers in debug builds
            -fstack-clash-protection    # Enable stack clash protection
            -fPIE                       # Position-independent executable
        )
        add_link_options(
            -fPIE                       # Position-independent executable linking
            -pie                        # Enable PIE support
        )
    endif()

    # Clang-specific warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(
            -Wno-c++98-compat           # Disable warnings about C++98 compatibility
            -Wno-c++98-compat-pedantic  # Disable pedantic C++98 compatibility warnings
        )
    endif()

    # GCC-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            -fanalyzer                  # Enable static analysis
            -Walloc-zero                # Warn on zero-sized allocations
        )
    endif()

else()
    message(WARNING "Using a compiler other than Clang or GCC. Safety options may not be applied.")
endif()
