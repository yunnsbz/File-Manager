cmake_minimum_required(VERSION 3.16)

project(File-Manager VERSION 0.3 LANGUAGES CXX)

set(CMAKE_AUTOUIC off)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include(CompileOptions.cmake)

# Kaynak dosyaları
set(SOURCE_FILES
    src/main.cpp
    src/mainwindow.cpp
    src/TabManager.cpp
    src/ToolBarManager.cpp
    src/TableManager.cpp
    src/TreeManager.cpp
    src/FileModelOperations.cpp
    src/ThemeManager.cpp
    src/FileOperationManager.cpp
)

# Header dosyaları
set(HEADER_FILES
    include/mainwindow.hpp
    include/TabManager.h
    include/ToolBarManager.h
    include/TableManager.h
    include/TreeManager.h
    include/FileModelOperations.h
    include/ThemeManager.h
    include/FileOperationManager.h
)

# UI dosyaları (manuel işlenecek)
set(UI_FILES
    ui/mainwindow.ui
)

# Resource dosyaları (.qrc)
set(RESOURCE_FILES
    resources.qrc
)

# CMake'e her şeyi bildir
qt_add_executable(File-Manager
    MANUAL_FINALIZATION
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${RESOURCE_FILES}
    config/.clang-format
    config/.clang-tidy
)

qt_wrap_ui(UIC_FILES ui/mainwindow.ui)

# Header klasörleri dosya yoluna ekleme işlemi
target_include_directories(File-Manager PRIVATE include)

target_link_libraries(File-Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets FatCxx::CompileOptions)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.File-Manager)
endif()

set_target_properties(File-Manager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS File-Manager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

file(GLOB_RECURSE QT_GENERATED_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/mocs_compilation.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/include/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/*_autogen/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/.rcc/*.cpp"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    foreach(file IN LISTS QT_GENERATED_FILES)
        set_source_files_properties(${file} PROPERTIES
            COMPILE_FLAGS "-Wno-ctad-maybe-unsupported -Wno-global-constructors -Wno-unsafe-buffer-usage"
        )
    endforeach()
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(File-Manager)
endif()
