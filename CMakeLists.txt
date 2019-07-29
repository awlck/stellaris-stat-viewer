cmake_minimum_required(VERSION 3.0.2)

project(stellaris_stat_viewer VERSION 0.3.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
set(MACOSX_BUNDLE_BUNDLE_NAME "Stellaris Stat Viewer")

option(SSV_BUILD_WIDGETS "Build the QtWidgets frontend" ON)
option(SSV_BUILD_TESTS "Build test cases" ON)
option(SSV_BUILD_JSON "Build the JSON frontend" ON)
option(ENABLE_EXTRA_WARNINGS "Enable extra warnings that may not be supported by all compilers" ON)
set(SSV_BUILD_VERSION "${PROJECT_VERSION}-git-custom" CACHE STRING "For release and CI builds of SSV, the version this is.")
set(SOME_FRONTEND_FOUND OFF)

if(SSV_BUILD_WIDGETS)
	find_package(Qt5 COMPONENTS Core Widgets CONFIG REQUIRED)
else()
	find_package(Qt5 COMPONENTS Core CONFIG REQUIRED)
endif()
set(CMAKE_AUTOMOC ON)

if(ENABLE_EXTRA_WARNINGS)
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
endif()

if(SSV_BUILD_WIDGETS)
add_library(ssv_frontend_widgets STATIC
        src/frontends/widgets/widgets_main.cpp
        src/frontends/widgets/gametranslator.cpp src/frontends/widgets/gametranslator.h
        src/frontends/widgets/mainwindow.cpp src/frontends/widgets/mainwindow.h
        src/frontends/widgets/numerictableitem.cpp src/frontends/widgets/numerictableitem.h
        src/frontends/widgets/settingsdialog.cpp src/frontends/widgets/settingsdialog.h
        src/frontends/widgets/techtreedialog.cpp src/frontends/widgets/techtreedialog.h
        src/frontends/widgets/views/economy_view.cpp src/frontends/widgets/views/economy_view.h
        src/frontends/widgets/views/fleets_view.cpp src/frontends/widgets/views/fleets_view.h
        src/frontends/widgets/views/overview_view.cpp src/frontends/widgets/views/overview_view.h
        src/frontends/widgets/views/techs_view.cpp src/frontends/widgets/views/techs_view.h
    )
target_link_libraries(ssv_frontend_widgets Qt5::Widgets)
set(SOME_FRONTEND_FOUND ON)
endif()

if(SSV_BUILD_JSON)
add_library(ssv_frontend_json STATIC
        src/frontends/json/json_main.cpp
        src/frontends/json/dataextraction.cpp src/frontends/json/dataextraction.h)
target_link_libraries(ssv_frontend_json Qt5::Core)
set(SOME_FRONTEND_FOUND ON)
endif()

configure_file(src/frontends.h.in frontends.h)

# for testing
add_library(ssv_parser STATIC
        src/core/parser.cpp src/core/parser.h)
target_link_libraries(ssv_parser Qt5::Core)

add_executable(stellaris_stat_viewer WIN32 MACOSX_BUNDLE
        src/main.cpp src/frontends.h.in
        src/core/galaxy_model.cpp src/core/galaxy_model.h
        src/core/galaxy_state.cpp src/core/galaxy_state.h
        src/core/empire.cpp src/core/empire.h
        src/core/fleet.cpp src/core/fleet.h
        src/core/ship.cpp src/core/ship.h
        src/core/ship_design.cpp src/core/ship_design.h
        src/core/technology.cpp src/core/technology.h
        src/core/puff/puff.c src/core/puff/puff.h
        src/core/extract_gamestate.cpp src/core/extract_gamestate.h)
target_compile_definitions(stellaris_stat_viewer PRIVATE SSV_VERSION="${SSV_BUILD_VERSION}")
target_link_libraries(stellaris_stat_viewer ssv_parser Qt5::Core)
target_include_directories(stellaris_stat_viewer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(SSV_BUILD_WIDGETS)
    target_link_libraries(stellaris_stat_viewer ssv_frontend_widgets)
endif()
if(SSV_BUILD_JSON)
    target_link_libraries(stellaris_stat_viewer ssv_frontend_json)
endif()

if(SSV_BUILD_TESTS)
    find_package(Qt5Test CONFIG REQUIRED)
    enable_testing()
    add_executable(test_parser tests/test_parser.cpp)
    target_link_libraries(test_parser ssv_parser Qt5::Test)
    add_test(NAME parser COMMAND test_parser)
endif()

if(NOT SOME_FRONTEND_FOUND AND NOT SSV_BUILD_TESTS)
	message(WARNING "All frontends and tests disabled, the built application will not run on its own.")
endif()

