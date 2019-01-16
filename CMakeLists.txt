cmake_minimum_required(VERSION 3.12)
project(EXTERMINATE CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC 1)

find_package(Qt5 REQUIRED COMPONENTS Core Multimedia)

add_executable(EXTERMINATE EXTERMINATE.cxx)
target_link_libraries(EXTERMINATE Qt5::Multimedia)
