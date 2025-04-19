#[[ fatpound ]]#

add_library (CompileOptions INTERFACE)
add_library (Fat::CompileOptions ALIAS CompileOptions)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(CompileOptions INTERFACE
        ##################################
        -pedantic # Conform to ISO/IEC Standard C++


        ## Active warnings
        -Wall
        -Wextra
        -Wpedantic


        ## Inactive warnings
        -Wno-newline-eof


        ## Preprocessor
        -DFAT_BUILDING_WITH_MSVC=0
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>:   -DFAT_BUILDING_ON_WINDOWS=0>
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DFAT_BUILDING_ON_WINDOWS=1>


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror
        >
        $<$<CONFIG:Release>:
            -O2
        >

        ## Standard library
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>: -stdlib=libstdc++>
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
    target_compile_options (CompileOptions INTERFACE
        ##################################
        -pedantic # Conform to ISO/IEC Standard C++


        ## Active warnings
        -Weverything


        ## Inactive warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic

        -Wno-c++20-compat
        -Wno-c++20-extensions

        -Wno-missing-prototypes
        -Wno-newline-eof
        -Wno-padded


        ## Preprocessor
        -DFAT_BUILDING_WITH_MSVC=0
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>:   -DFAT_BUILDING_ON_WINDOWS=0>
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DFAT_BUILDING_ON_WINDOWS=1>


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror
        >
        $<$<CONFIG:Release>:
            -O2
        >

        ## Standard library
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>: -stdlib=libc++>
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options (CompileOptions INTERFACE
        #####################################
        /permissive-        # Conform to ISO/IEC Standard C++
        # /std:c++latest
        # /std:clatest
        # /nologo
        # /MP               # Multi-processor compilation but it's not needed because CMake already launches multiple threads
        /Zc:wchar_t         # treat 'wchar_t' as a built-in type
        /Zc:preprocessor    # Use standards conforming preprocessor
        /errorReport:none   # Don't report internal compiler errors


        ## Code generation
        /EHsc               # Enable C++ exceptions
        /arch:AVX2          # Enable AVX2 extensions for code generation


        ## Diagnostics
        /diagnostics:column # Use old-style diagnostic messages because they are more readable when using MSVC


        ## Active warnings
        /Wall


        ## Inactive warnings
        /external:anglebrackets # Treat headers included with <> as external
        /external:W0            # Do NOT emit warnings for external headers

        /wd4061 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is a default case)
        /wd4062 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is NOT a default case)
        /wd4324 # structure was padded due to alignment specifier
        /wd4514 # Unreferenced inline function has been removed
        /wd4820 # n bytes padding added after construct MyClass
        /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5246 # outdated warning, the initialization of a subobject should be wrapped in braces


        ## Preprocessor definitions
        /DFAT_BUILDING_WITH_MSVC=1
        /DFAT_BUILDING_ON_WINDOWS=1
        /DNOMINMAX
        /DSTRICT
        /D_UNICODE
        /DUNICODE


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            /Od

            /WX  # Treat warnings as errors

            /DIN_DEBUG=true
            /DIN_RELEASE=false
        >
        $<$<CONFIG:Release>:
            /O2

            /WX- # Do NOT Treat warnings as errors

            /GF  # String pooling
            /GL  # Whole-program optimization

            /DIN_DEBUG=false
            /DIN_RELEASE=true
        >
    )

endif ()
