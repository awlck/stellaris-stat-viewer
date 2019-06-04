cmake_minimum_required(VERSION 3.0.2)

project(stellaris_stat_viewer VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
set(MACOSX_BUNDLE_BUNDLE_NAME "Stellaris Stat Viewer")

option(SSV_BUILD_TESTS BOOL ON)
set(SSV_BUILD_VERSION "${PROJECT_VERSION}-git-custom" CACHE STRING "For release and CI builds of SSV, the version this is.")

find_package(Qt5 COMPONENTS Core Widgets CONFIG REQUIRED)
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

add_library(ssv_core SHARED
        src/parser/parser.cpp src/parser/parser.h
        src/model/galaxy_model.cpp src/model/galaxy_model.h
        src/model/galaxy_state.cpp src/model/galaxy_state.h
        src/model/empire.cpp src/model/empire.h
		src/model/fleet.cpp src/model/fleet.h
		src/model/ship.cpp src/model/ship.h
        src/model/ship_design.cpp src/model/ship_design.h
		src/model/technology.cpp src/model/technology.h
    )
target_link_libraries(ssv_core Qt5::Core)

add_library(ssv_frontend_widgets SHARED
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
target_link_libraries(ssv_frontend_widgets Qt5::Widgets ssv_core)

add_executable(stellaris_stat_viewer WIN32 MACOSX_BUNDLE
        src/main.cpp)
target_compile_definitions(stellaris_stat_viewer PRIVATE SSV_VERSION="${SSV_BUILD_VERSION}")
target_link_libraries(stellaris_stat_viewer Qt5::Core)

if(APPLE)
    target_link_options(stellaris_stat_viewer PRIVATE -rpath @executable_path/.)
    target_link_options(ssv_frontend_widgets PRIVATE -rpath @executable_path/.)
endif()

if(SSV_BUILD_TESTS)
    find_package(Qt5Test CONFIG REQUIRED)
    enable_testing()
    add_executable(test_parser tests/test_parser.cpp)
    target_link_libraries(test_parser ssv_core Qt5::Test)
    add_test(NAME parser COMMAND test_parser)
endif()