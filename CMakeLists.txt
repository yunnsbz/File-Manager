cmake_minimum_required(VERSION 3.19)

project(File-Manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES FALSE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(File-Manager
    WIN32 MACOSX_BUNDLE

    src/main.cpp
    src/mainwindow.cpp
    src/TabManager.cpp
    src/ToolBarManager.cpp
    src/TableManager.cpp
    src/TreeManager.cpp
    src/FileModelOperations.cpp
    src/ThemeManager.cpp
    src/FileOperationManager.cpp

    include/mainwindow.hpp
    include/TabManager.h
    include/ToolBarManager.h
    include/TableManager.h
    include/TreeManager.h
    include/FileModelOperations.h
    include/ThemeManager.h
    include/FileOperationManager.h
    include/IFileOperation.h
    include/AbstractFileOp.h

    include/PasteFileOperation.h

    include/MoveFileOperation.h
    include/RenameFileOperation.h
    include/NewFolderOperation.h

    ui/MainWindow.ui

    config/.clang-tidy
    config/.clang-format
    CompileOptions.cmake
)

qt_add_resources(RESOURCE_SOURCES
    resources.qrc
)

include(CompileOptions.cmake)

target_sources(File-Manager PRIVATE ${RESOURCE_SOURCES})

set_property(TARGET File-Manager PROPERTY AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

target_include_directories(File-Manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE QT_GENERATED_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/mocs_compilation.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/include/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/.rcc/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/qrc_*.cpp"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    foreach(file IN LISTS QT_GENERATED_FILES)
        set_source_files_properties(${file} PROPERTIES
            COMPILE_FLAGS "-Wno-ctad-maybe-unsupported -Wno-global-constructors -Wno-unsafe-buffer-usage"
        )
    endforeach()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    foreach(file IN LISTS QT_GENERATED_FILES)
        set_source_files_properties(${file} PROPERTIES
            COMPILE_FLAGS "/wd4464 /wd4866"
        )
    endforeach()

    target_compile_options(CompileOptions INTERFACE /wd4464 /wd4625 /wd4626 /wd5026 /wd5027)
endif()

target_link_libraries(File-Manager
    PRIVATE
        Qt::Core
        Qt::Widgets
        FatCxx::CompileOptions
)

include(GNUInstallDirs)

install(TARGETS File-Manager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET File-Manager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
