cmake_minimum_required(VERSION 3.0.2)

project(stellaris_stat_viewer VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5Widgets CONFIG REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(stellaris_stat_viewer WIN32 #MACOSX_BUNDLE
        src/main.cpp
        src/mainwindow.cpp src/mainwindow.h
        src/parser.cpp src/parser.h)
target_link_libraries(stellaris_stat_viewer Qt5::Widgets)

if(MSVC)
    target_compile_options(stellaris_stat_viewer PRIVATE
            "/we4716"   # treat "Function does not return a value" as an error
            "/we4715"   # treat "Not all control paths return a value" as an error
    )
else()
    target_compile_options(stellaris_stat_viewer PRIVATE
            "-Wall"     # enable "Wall" warnings
            "-Wextra"   # enable "Wextra" warnings
            "-Werror=return-type"   # treat return value-related warnings as errors
    )
endif()