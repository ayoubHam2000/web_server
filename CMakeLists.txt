cmake_minimum_required(VERSION 3.24)
project(web_server)

set(CMAKE_CXX_STANDARD 98)

include_directories(web_server Includes/)

add_executable(web_server main.cpp Includes/WebServer.h ChunckContentHandler.hpp FileSystem.hpp)
