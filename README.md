# Raft_Kee

一个使用 C++20 编写的简单内存键值存储项目。目前可以通过命令行写入、追加和读取字符串。

## 当前功能

- `put <key> <value>`：写入键值；键已存在时覆盖原值。
- `append <key> <value>`：在已有值末尾追加内容；键不存在时创建该键。
- `get <key>`：读取值；键不存在时输出 `NOT FOUND`。
- `exit`：退出程序。输入结束（EOF）也会退出。

键和值按空白字符分隔，不能包含空格；命令不接受多余参数。空行会被忽略，命令区分大小写。

数据保存在进程内存中，程序退出后丢失。目前仅支持单线程访问，尚未实现持久化、网络服务或 Raft 共识。

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

## 项目结构

```text
.
├── CMakeLists.txt        # 构建 kv_core 库和 kv_cli 程序
├── app/main.cpp          # 命令行输入、解析和输出
├── include/
│   ├── kv_server.h       # 键值服务接口
│   └── kv_store.h        # 内存存储接口
└── src/
    ├── kv_server.cpp     # 将服务操作转交给存储层
    └── kv_store.cpp      # 基于 unordered_map 的存储实现
```
