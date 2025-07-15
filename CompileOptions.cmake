##*******************************************##
##              [[ fatpound* ]]              ##
##                                           ##
##   CMake Utility Module for Standard C++   ##
##        My Default Compiler Options        ##
##*******************************************##

if (NOT TARGET CompileOptions)

add_library (CompileOptions INTERFACE)
add_library (FatCxx::CompileOptions ALIAS CompileOptions)

if (CMAKE_CXX_STANDARD EQUAL 23)
    target_compile_features(CompileOptions INTERFACE cxx_std_23)
elseif (CMAKE_CXX_STANDARD EQUAL 20)
    target_compile_features(CompileOptions INTERFACE cxx_std_20)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(CompileOptions INTERFACE
        #################################
        -pedantic # Conform to ISO/IEC Standard


        ## Active warnings
        -Wall
        -Wextra
        -Wpedantic


        ## Inactive warnings
        -Wno-unused-function


        ## Static analyzer
        $<$<STREQUAL:${FATCXX_GCC_ANALYZER},Enabled>: -fanalyzer>


        ## Preprocessor definitions
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DFATLIB_BUILDING_ON_WINDOWS -D_UNICODE -DUNICODE -DSTRICT -DNOMINMAX>


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror

            -DIN_DEBUG
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -D_DEBUG>
        >
        $<$<CONFIG:Release>:
            -O3

            -march=native

            -DIN_RELEASE
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DNDEBUG>
        >
    )

    target_link_libraries(CompileOptions INTERFACE
        ##################################

        ## Standard library
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -lstdc++exp>
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options (CompileOptions INTERFACE
        ##################################
        -pedantic # Conform to ISO/IEC Standard


        ## Active warnings
        -Weverything
        -Wpedantic


        ## Inactive warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic

        -Wno-c++20-compat
        -Wno-c++20-extensions

        -Wno-missing-prototypes
        -Wno-newline-eof
        -Wno-padded
        -Wno-unused-function
        -Wno-unused-template
        -Wno-documentation


        ## Preprocessor definitions
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DFATLIB_BUILDING_ON_WINDOWS -D_UNICODE -DUNICODE -DSTRICT -DNOMINMAX>


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror

            -DIN_DEBUG
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -D_DEBUG>
        >
        $<$<CONFIG:Release>:
            -O3

            -march=native

            -DIN_RELEASE
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DNDEBUG>
        >


        ## Standard library
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>: -stdlib=libc++>
    )

    target_link_options(CompileOptions INTERFACE
        ##################################

        ## Standard library
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>: -lc++>
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
    target_compile_options (CompileOptions INTERFACE
        ##################################
        ## Active warnings
        -Wall


        ## Inactive warnings
        -Wno-unused-function


        ## Preprocessor definitions
        $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DFATLIB_BUILDING_ON_WINDOWS -D_UNICODE -DUNICODE -DSTRICT -DNOMINMAX>


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>:   -O0>
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -Od>

            -Werror

            -DIN_DEBUG
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -D_DEBUG>
        >
        $<$<CONFIG:Release>:
            -O2

            -DIN_RELEASE
            $<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Windows>: -DNDEBUG>
        >
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options (CompileOptions INTERFACE
        ##################################
        /permissive-        # Conform to ISO/IEC Standard
        # /nologo
        # /MP               # Multi-processor compilation but it's not needed because CMake already launches multiple threads
        /Zc:wchar_t         # treat 'wchar_t' as a built-in type
        /Zc:preprocessor    # Use standards conforming preprocessor
        /Zc:__cplusplus     # Enable updated __cplusplus value for the corresponding standard
        /errorReport:none   # Don't report internal compiler errors


        ## Code generation
        /EHsc               # Enable C++ exceptions


        ## Diagnostics
        /diagnostics:column # Use old-style diagnostic messages because they are more readable when using MSVC


        ## Active warnings
        /Wall


        ## Inactive warnings
        /external:anglebrackets # Treat includes with angular brackets as external headers
        /external:W0 # Do NOT emit warnings for external headers

        /wd4061 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is a default case)
        /wd4062 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is NOT a default case)
        /wd4324 # structure was padded due to alignment specifier
        /wd4464 # relative include path contains '..'
        /wd4514 # Unreferenced inline function has been removed
        /wd4820 # n bytes padding added after construct MyClass
        /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5246 # outdated warning, the initialization of a subobject should be wrapped in braces


        ## Preprocessor definitions
        /DFATLIB_BUILDING_WITH_MSVC
        /DFATLIB_BUILDING_ON_WINDOWS
        /D_UNICODE
        /DUNICODE
        /DSTRICT
        /DNOMINMAX


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            /Od

            /WX # Treat warnings as errors

            /wd4710 # Function not inlined
            /wd4711 # Function selected for inline expansion

            /DIN_DEBUG
            /D_DEBUG
        >
        $<$<CONFIG:Release>:
            /O2

            /WX- # Do NOT Treat warnings as errors

            /GF  # String pooling
            /GL  # Whole-program optimization

            /DIN_RELEASE
            /DNDEBUG


            ## Advanced Options
            /Qpar # Enable auto-parallelization of loops with #pragma loop directive

            $<$<STREQUAL:${FATCXX_MSVC_FULLREPORT},Enabled>:
                /Qpar-report:2 # Auto-parallelizer reporting (Max)
                /Qvec-report:2 # Auto-vectorizer   reporting (Max)
            >
        >
    )

    target_link_options(CompileOptions INTERFACE
        ##################################
        /ERRORREPORT:NONE # Don't report internal linker errors
    )

endif ()

endif()
