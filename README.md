# Raft_Kee

一个使用 C++20 编写的简单内存键值存储项目。目前可以通过命令行写入、追加和读取字符串。

## 本批与上批的区别

上批代码由 `KvServer` 将命令直接交给状态机。本批在服务和状态机之间加入内存日志，并增加了测试：

- 新增 `LocalLog`、日志条目 `LogEntry` 和应用结果 `ApplyMsg`。每次提交都会为命令分配从 1 开始的递增索引，记录命令，并立即提交和应用到状态机。
- `KvServer` 改为向 `LocalLog` 提交命令，从返回的应用结果中取得读取结果。`Get` 也会产生日志，即使键不存在也是如此。
- 新增日志数量、最近提交索引和最近应用索引的查询接口，初始值均为 0。
- 新增本地日志测试，并在 CMake 中注册为 CTest 测试。

命令行的 `put`、`append`、`get` 和 `exit` 用法以及内存存储行为保持不变；本批还没有实现 Raft 共识。

## 当前功能

- `put <key> <value>`：写入键值；键已存在时覆盖原值。
- `append <key> <value>`：在已有值末尾追加内容；键不存在时创建该键。
- `get <key>`：读取值；键不存在时输出 `NOT FOUND`。
- `exit`：退出程序。输入结束（EOF）也会退出。

键和值按空白字符分隔，不能包含空格；命令不接受多余参数。空行会被忽略，命令区分大小写。

数据和日志保存在进程内存中，程序退出后丢失。目前仅支持单线程访问，尚未实现持久化、网络服务或 Raft 共识；日志中的“提交”是本地立即提交，不涉及节点之间的复制或共识。

## 当前代码结构

命令行解析输入后调用 `KvServer`；服务将操作封装成 `Command` 并提交给 `LocalLog`；日志记录命令、更新提交索引，然后调用 `KvStateMachine::apply`，更新应用索引并返回结果；状态机操作底层的 `KvStore`。

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

构建完成后，使用 CTest 运行状态机和本地日志测试：

```bash
ctest --test-dir build --output-on-failure
```

状态机测试检查键不存在、写入后追加并读取、非法操作返回状态，以及非法操作不会修改已有数据。本地日志测试检查初始索引、命令顺序与结果、提交和应用索引，以及读取不存在的键时的日志行为。

## 项目结构

```text
.
├── CMakeLists.txt        # 构建库、命令行程序和测试
├── app/main.cpp          # 命令行输入、解析和输出
├── include/
│   ├── command.h         # 命令、操作类型和执行结果
│   ├── kv_server.h       # 键值服务接口
│   ├── kv_state_machine.h # 状态机接口
│   ├── kv_store.h        # 内存存储接口
│   └── local_log.h       # 内存日志和应用结果接口
├── src/
│   ├── kv_server.cpp     # 将服务操作转为命令并提交日志
│   ├── kv_state_machine.cpp # 执行命令并返回结果
│   ├── kv_store.cpp      # 基于 unordered_map 的存储实现
│   └── local_log.cpp     # 记录、提交和应用本地命令
└── tests/
    ├── local_log_test.cpp # 本地日志测试
    └── state_machine_test.cpp # 状态机测试
```
