cmake_minimum_required(VERSION 3.17)
project(webserver_boost)

set(CMAKE_CXX_STANDARD 17)
set(Boost_INCLUDE_DIR "C:/Program Files/boost/boost_1_75_0")
set(Boost_LIBRARY_DIR "C:/Program Files/boost/boost_1_75_0/libs")

find_package(Boost 1.75 REQUIRED)

add_executable(webserver_boost src/main.cpp include/ctpl_stl.h)
target_link_libraries(webserver_boost PUBLIC Boost::boost)
target_link_libraries(webserver_boost PUBLIC Ws2_32)