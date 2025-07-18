cmake_minimum_required(VERSION 3.16)

project(BNES VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor/SDL)

add_subdirectory(src)