cmake_minimum_required(VERSION 3.20.2)
project(projectKA)

set(CMAKE_CXX_STANDARD 11)

add_executable(projectKA main.cpp Minesweeper.cpp Minesweeper.h)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

set(SOURCE_FILES main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} ${GTKMM_LIBRARIES})