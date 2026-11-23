# Raft_Kee

一个使用 C++20 编写的简单内存键值存储项目，支持本地交互式命令行，以及通过本机 TCP 服务写入、追加和读取字符串。

## 本批与上批的区别

上批代码提供本地交互式命令行、内存日志和状态机。本批新增单节点网络访问：

- 新增 `kv_server` 和 `kv_remote`，分别作为 TCP 服务端和命令行客户端。
- 新增 `proto/kv.proto`，使用 Protocol Buffers 定义请求、响应、操作和状态；CMake 构建时自动生成 C++ 代码。
- 新增 TCP 分帧代码，以 4 字节网络字节序长度加消息体传输数据，并设置消息大小上限和收发超时。
- 新增 Protobuf 构建依赖。已有的本地命令行、状态机和内存日志继续保留。

服务端目前只监听本机回环地址，尚未实现 Raft 共识。

## 当前功能

- `put <key> <value>`：写入键值；键已存在时覆盖原值。
- `append <key> <value>`：在已有值末尾追加内容；键不存在时创建该键。
- `get <key>`：读取值；键不存在时输出 `NOT FOUND`。
- `exit`：退出程序。输入结束（EOF）也会退出。

本地交互式命令行的键和值按空白字符分隔，不能包含空格；命令不接受多余参数。空行会被忽略，命令区分大小写。网络客户端通过命令行参数传入键和值。

数据和日志保存在各自进程的内存中，进程退出后丢失；本地命令行和网络服务端不共享数据。服务端依次处理连接，每个连接处理一条请求；尚未实现持久化或 Raft 共识，日志中的“提交”是本地立即提交，不涉及节点之间的复制或共识。

## 当前代码结构

命令行解析输入后调用 `KvServer`；服务将操作封装成 `Command` 并提交给 `LocalLog`；日志记录命令、更新提交索引，然后调用 `KvStateMachine::apply`，更新应用索引并返回结果；状态机操作底层的 `KvStore`。

网络客户端将请求序列化为 Protobuf 消息，通过 TCP 帧发送到服务端；服务端解析请求并调用同一个 `KvServer` 接口，再将响应序列化后发回。每帧以 4 字节大端整数表示消息体长度，消息体最大为 1 MiB；客户端和服务端为连接设置了 3 秒收发超时。

`Command` 包含操作类型、键和值，操作类型包括 `Put`、`Append` 和 `Get`。状态机通过 `CommandResult` 返回状态和结果值：成功时为 `Ok`，键不存在时为 `NotFound`，无法识别的操作为 `InvalidOperation`。写入和追加成功时返回的结果值为空。

## 环境要求

- CMake 3.22 或更高版本
- 支持 C++20 的编译器
- Protocol Buffers 编译器 `protoc` 和 C++ 开发库（Ubuntu 可安装 `protobuf-compiler`、`libprotobuf-dev`）
- 支持代码所用 POSIX socket 接口的环境，例如 Linux

## 构建和运行

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build -j 2
```

构建会根据 `proto/kv.proto` 在构建目录中生成 `kv.pb.h` 和 `kv.pb.cc`。运行本地交互式程序：

```bash
./build/kv_cli
```

交互示例（启动提示略）：

```text
put name Raft
OK
append name _Kee
OK
get name
VALUE: Raft_Kee
get missing
NOT FOUND
exit
```

### 网络服务

在一个终端启动服务端，端口范围为 1～65535：

```bash
./build/kv_server 8080
```

在另一个终端执行客户端命令：

```bash
./build/kv_remote 8080 put name Raft
./build/kv_remote 8080 append name _Kee
./build/kv_remote 8080 get name
./build/kv_remote 8080 get missing
```

依次输出 `OK`、`OK`、`VALUE: Raft_Kee` 和 `NOT FOUND`。客户端连接 `127.0.0.1`；使用完毕后可以在服务端终端按 `Ctrl+C` 停止服务。

## 运行测试

构建完成后，使用 CTest 运行状态机和本地日志测试：

```bash
ctest --test-dir build --output-on-failure
```

状态机测试检查键不存在、写入后追加并读取、非法操作返回状态，以及非法操作不会修改已有数据。本地日志测试检查初始索引、命令顺序与结果、提交和应用索引，以及读取不存在的键时的日志行为。目前 CTest 尚未注册网络通信测试。

## 项目结构

```text
.
├── CMakeLists.txt        # 构建库、命令行程序和测试
├── app/
│   ├── main.cpp          # 本地交互式命令行
│   ├── kv_server_main.cpp # TCP 服务端
│   └── kv_remote.cpp     # TCP 客户端
├── include/
│   ├── command.h         # 命令、操作类型和执行结果
│   ├── frame.h           # TCP 分帧接口
│   ├── kv_server.h       # 键值服务接口
│   ├── kv_state_machine.h # 状态机接口
│   ├── kv_store.h        # 内存存储接口
│   └── local_log.h       # 内存日志和应用结果接口
├── src/
│   ├── frame.cpp         # TCP 分帧、端口解析和超时设置
│   ├── kv_server.cpp     # 将服务操作转为命令并提交日志
│   ├── kv_state_machine.cpp # 执行命令并返回结果
│   ├── kv_store.cpp      # 基于 unordered_map 的存储实现
│   └── local_log.cpp     # 记录、提交和应用本地命令
├── proto/
│   └── kv.proto          # Protobuf 请求与响应定义
└── tests/
    ├── local_log_test.cpp # 本地日志测试
    └── state_machine_test.cpp # 状态机测试
```
