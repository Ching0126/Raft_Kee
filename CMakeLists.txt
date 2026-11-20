cmake_minimum_required(VERSION 3.22)
project(RaftKee LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(kv_core
    src/kv_store.cpp
    src/kv_server.cpp)

target_include_directories(kv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(kv_cli app/main.cpp)

target_link_libraries(kv_cli PRIVATE kv_core)