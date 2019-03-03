cmake_minimum_required(VERSION 3.0.2)

project(stellaris_stat_viewer VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets CONFIG REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(stellaris_stat_viewer WIN32 MACOSX_BUNDLE
        src/main.cpp
        src/mainwindow.cpp src/mainwindow.h
        src/parser.cpp src/parser.h)
target_link_libraries(stellaris_stat_viewer Qt5::Widgets)