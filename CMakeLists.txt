cmake_minimum_required(VERSION 3.22)
project(RaftKee LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(kv_core
    src/kv_store.cpp
    src/kv_state_machine.cpp
    src/local_log.cpp
    src/kv_server.cpp
)

target_include_directories(kv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(kv_cli app/main.cpp)
target_link_libraries(kv_cli PRIVATE kv_core)

enable_testing()

add_executable(state_machine_test
    tests/state_machine_test.cpp
)
target_link_libraries(state_machine_test PRIVATE kv_core)
add_test(NAME state_machine_test COMMAND state_machine_test)

add_executable(local_log_test tests/local_log_test.cpp)
target_link_libraries(local_log_test PRIVATE kv_core)
add_test(NAME local_log_test COMMAND local_log_test)

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(
    KV_PROTO_SRCS
    KV_PROTO_HDRS
    proto/kv.proto
    proto/rpc.proto
)

add_library(kv_wire
    ${KV_PROTO_SRCS}
    src/frame.cpp
)

target_include_directories(kv_wire PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(kv_wire PUBLIC protobuf::libprotobuf)

# 客户端通道与服务端分发器。
add_library(rpc_lib
    src/rpc_dispatcher.cpp
    src/rpc_channel.cpp
)
target_link_libraries(rpc_lib PUBLIC kv_wire)

# KV 业务与 RPC 框架之间的适配层。
add_library(kv_service src/kv_rpc_service.cpp)
target_link_libraries(kv_service PUBLIC kv_core rpc_lib)

add_executable(kv_server app/kv_server_main.cpp)
target_link_libraries(kv_server PRIVATE kv_service)

add_executable(kv_remote app/kv_remote.cpp)
target_link_libraries(kv_remote PRIVATE rpc_lib)

add_executable(rpc_dispatcher_test
    tests/rpc_dispatcher_test.cpp
)
target_link_libraries(rpc_dispatcher_test PRIVATE kv_service)
add_test(NAME rpc_dispatcher_test COMMAND rpc_dispatcher_test)