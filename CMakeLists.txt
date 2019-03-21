cmake_minimum_required(VERSION 3.0.2)

project(stellaris_stat_viewer VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")

option(SSV_BUILD_TESTS BOOL ON)

find_package(Qt5Widgets CONFIG REQUIRED)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    add_compile_options(
            "/we4716"   # treat "Function does not return a value" as an error
            "/we4715"   # treat "Not all control paths return a value" as an error
            # MSVC unfortunately does not provide a fallthrough warning.
    )
else()
    add_compile_options(
            "-Wall"     # enable "Wall" warnings
            "-Wextra"   # enable "Wextra" warnings
            "-Wimplicit-fallthrough"    # explicitly enable warnings about switch fallthrough
            "-Werror=return-type"       # treat return value-related warnings as errors
    )
endif()

add_library(ssv_parser src/parser/parser.cpp src/parser/parser.h)
target_link_libraries(ssv_parser Qt5::Core)

add_library(ssv_model
        src/model/galaxy_state.cpp src/model/galaxy_state.h
        src/model/empire.cpp src/model/empire.h
		src/model/fleet.cpp src/model/fleet.h
		src/model/ship.cpp src/model/ship.h
        src/model/ship_design.cpp src/model/ship_design.h
    )
target_link_libraries(ssv_model Qt5::Core)

add_executable(stellaris_stat_viewer WIN32 MACOSX_BUNDLE
        src/main.cpp
		src/economy_view.cpp src/economy_view.h
        src/fleets_view.cpp src/fleets_view.h
        src/mainwindow.cpp src/mainwindow.h
        src/numerictableitem.cpp src/numerictableitem.h
        src/overview_view.cpp src/overview_view.h
    )
target_link_libraries(stellaris_stat_viewer Qt5::Widgets ssv_parser ssv_model)

if(SSV_BUILD_TESTS)
    find_package(Qt5Test CONFIG REQUIRED)
    enable_testing()
    add_executable(test_parser tests/test_parser.cpp)
    target_link_libraries(test_parser ssv_parser Qt5::Test)
    add_test(NAME parser COMMAND test_parser)
endif()