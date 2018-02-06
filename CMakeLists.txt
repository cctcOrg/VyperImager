cmake_minimum_required(VERSION 3.10)

project(vyper-imager)

find_package(PkgConfig REQUIRED)

#set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/pkgconfig;/usr/share/pkgconfig")

pkg_check_modules(GTKMM gtkmm-3.0)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(vyper ${SOURCES} ${HEADERS})
target_link_libraries(vyper ${GTKMM_LIBRARIES})

#add_subdirectory(src)
