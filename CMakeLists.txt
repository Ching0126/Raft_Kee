cmake_minimum_required(VERSION 3.22)
project(RaftKee LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(kv_core
    src/kv_store.cpp
    src/kv_state_machine.cpp
    src/local_log.cpp
    src/kv_server.cpp)

target_include_directories(kv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(kv_cli app/main.cpp)
target_link_libraries(kv_cli PRIVATE kv_core)

enable_testing()
add_executable(state_machine_test
    tests/state_machine_test.cpp)

target_link_libraries(state_machine_test PRIVATE kv_core)

add_test(NAME state_machine_test COMMAND state_machine_test)

add_executable(local_log_test tests/local_log_test.cpp)

target_link_libraries(local_log_test PRIVATE kv_core)
add_test(NAME local_log_test COMMAND local_log_test)