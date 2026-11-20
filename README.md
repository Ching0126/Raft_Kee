# Raft_Kee

一个使用 C++20 编写的简单内存键值存储项目。目前可以通过命令行写入、追加和读取字符串。

## 本批与上批的区别

上批代码由 `KvServer` 直接调用 `KvStore` 完成键值操作。本批在两者之间加入状态机，并增加了测试：

- 新增 `Command` 和 `CommandResult`，分别统一表示操作请求和执行结果；结果状态包括 `Ok`、`NotFound` 和 `InvalidOperation`。
- 新增 `KvStateMachine`，通过 `apply` 分发 `Put`、`Append` 和 `Get` 命令，再调用 `KvStore` 执行操作。
- `KvServer` 改为构造命令并调用状态机，`get` 接口不再声明为 `const`。
- 新增状态机测试，并在 CMake 中注册为 CTest 测试。

命令行的 `put`、`append`、`get` 和 `exit` 用法以及内存存储行为保持不变；本批还没有实现 Raft 共识。

## 当前功能

- `put <key> <value>`：写入键值；键已存在时覆盖原值。
- `append <key> <value>`：在已有值末尾追加内容；键不存在时创建该键。
- `get <key>`：读取值；键不存在时输出 `NOT FOUND`。
- `exit`：退出程序。输入结束（EOF）也会退出。

键和值按空白字符分隔，不能包含空格；命令不接受多余参数。空行会被忽略，命令区分大小写。

数据保存在进程内存中，程序退出后丢失。目前仅支持单线程访问，尚未实现持久化、网络服务或 Raft 共识。

## 当前代码结构

命令行解析输入后调用 `KvServer`；服务将操作封装成 `Command`，交给 `KvStateMachine::apply` 执行；状态机再操作底层的 `KvStore`。

`Command` 包含操作类型、键和值，操作类型包括 `Put`、`Append` 和 `Get`。状态机通过 `CommandResult` 返回状态和结果值：成功时为 `Ok`，键不存在时为 `NotFound`，无法识别的操作为 `InvalidOperation`。写入和追加成功时返回的结果值为空。

## 环境要求

- CMake 3.22 或更高版本
- 支持 C++20 的编译器

## 构建和运行

在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build -j 2
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

## 运行测试

构建完成后，使用 CTest 运行状态机测试：

```bash
ctest --test-dir build --output-on-failure
```

当前测试检查键不存在、写入后追加并读取、非法操作返回状态，以及非法操作不会修改已有数据。

## 项目结构

```text
.
├── CMakeLists.txt        # 构建库、命令行程序和测试
├── app/main.cpp          # 命令行输入、解析和输出
├── include/
│   ├── command.h         # 命令、操作类型和执行结果
│   ├── kv_server.h       # 键值服务接口
│   ├── kv_state_machine.h # 状态机接口
│   └── kv_store.h        # 内存存储接口
├── src/
│   ├── kv_server.cpp     # 将服务操作转为状态机命令
│   ├── kv_state_machine.cpp # 执行命令并返回结果
│   └── kv_store.cpp      # 基于 unordered_map 的存储实现
└── tests/
    └── state_machine_test.cpp # 状态机测试
```
