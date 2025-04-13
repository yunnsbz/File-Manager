#[[FatSettings]]#

add_library (CompileOptions INTERFACE)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(CompileOptions INTERFACE
        ##################################
        -pedantic # Conform to ISO/IEC C++


        ## Active warnings
        -Wall
        -Wextra
        -Wpedantic


        ## Inactive warnings
        # -Wno-newline-eof


        ## Preprocessor
        -DFAT_BUILDING_WITH_MSVC=0


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror
        >
        $<$<CONFIG:Release>:
            -O2
        >
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM")
    target_compile_options (CompileOptions INTERFACE
        ##################################
        -pedantic # Conform to ISO/IEC C++


        ## Active warnings
        -Weverything


        ## Inactive warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic

        -Wno-c++20-compat
        -Wno-c++20-extensions

        -Wno-padded
        # -Wno-newline-eof


        ## Preprocessor
        -DFAT_BUILDING_WITH_MSVC=0


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            -O0

            -Werror
        >
        $<$<CONFIG:Release>:
            -O2
        >
    )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options (CompileOptions INTERFACE
        #####################################
        /permissive- # Conform to ISO/IEC C++
        /std:c++latest


        ## Active warnings
        /Wall


        ## Inactive warnings
        /wd4061 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is a default case)
        /wd4062 # Not all enum identifiers of an Enum (class) are handled by a switch statement (When there is NOT a default case)
        /wd4324 # structure was padded due to alignment specifier
        /wd4514 # Unreferenced inline function has been removed
        /wd4820 # n bytes padding added after construct MyClass
        /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5246 # outdated warning, the initialization of a subobject should be wrapped in braces


        ## Preprocessor
        -DFAT_BUILDING_WITH_MSVC=1


        ## Configuration-specific
        $<$<CONFIG:Debug>:
            /Od

            /WX # Treat warnings as errors
        >
        $<$<CONFIG:Release>:
            /O2
            /GF # String pooling
            /GL # Whole-program optimization

            /Qvec-report:2 # Auto vectorizer reporting (max)
            /Qpar-report:2 # Parallelizer reporting (max)
        >
    )

endif ()
