cmake_minimum_required(VERSION 3.17)
project(webserver_boost)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.72 REQUIRED)

add_executable(webserver_boost src/main.cpp include/ctpl_stl.h)
target_link_libraries(webserver_boost PUBLIC Boost::boost)
target_link_libraries(webserver_boost PUBLIC Ws2_32)