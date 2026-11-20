#include "kv_state_machine.h"

#include <iostream>

int main() {
    KvStateMachine machine;

    // 初始状态没有 x。
    auto result = machine.apply({Operation::Get, "x", ""});
    if (result.status != ResultStatus::NotFound) {
        std::cerr << "FAIL: missing key\n";
        return 1;
    }

    // 先写入，再追加。
    result = machine.apply({Operation::Put, "x", "hello"});
    if (result.status != ResultStatus::Ok) {
        std::cerr << "FAIL: put\n";
        return 1;
    }

    result = machine.apply({Operation::Append, "x", "_raft"});
    if (result.status != ResultStatus::Ok) {
        std::cerr << "FAIL: append\n";
        return 1;
    }

    result = machine.apply({Operation::Get, "x", ""});
    if (result.status != ResultStatus::Ok ||result.value != "hello_raft") {
        std::cerr << "FAIL: ordered operations\n";
        return 1;
    }

    // 模拟收到无法识别的操作。
    result = machine.apply({static_cast<Operation>(999),"x","bad"});

    if (result.status != ResultStatus::InvalidOperation) {
        std::cerr << "FAIL: invalid operation\n";
        return 1;
    }

    // 非法操作不应更改原有数据。
    result = machine.apply({Operation::Get, "x", ""});
    if (result.status != ResultStatus::Ok ||
        result.value != "hello_raft") {
        std::cerr << "FAIL: data changed after invalid operation\n";
        return 1;
    }

    std::cout << "State machine checks passed\n";
    return 0;
}