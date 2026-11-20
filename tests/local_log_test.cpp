#include "local_log.h"

#include <iostream>

int main() {
    LocalLog log;

    // 还没有提交命令时，索引和日志数量应该均为 0。
    if (log.size() != 0 ||log.committedIndex() != 0 ||log.lastApplied() != 0) {
        std::cerr << "FAIL: initial indexes\n";
        return 1;
    }

    const auto write = log.submit({Operation::Put, "x", "hello"});

    const auto append = log.submit({Operation::Append, "x", "_raft"});

    const auto read = log.submit({Operation::Get, "x", ""});

    // 验证索引顺序和三条命令的执行结果。
    if (write.index != 1 ||
        append.index != 2 ||
        read.index != 3 ||
        write.result.status != ResultStatus::Ok ||
        append.result.status != ResultStatus::Ok ||
        read.result.status != ResultStatus::Ok ||
        read.result.value != "hello_raft") {
        std::cerr << "FAIL: order or result\n";
        return 1;
    }

    if (log.size() != 3 ||
        log.committedIndex() != 3 ||
        log.lastApplied() != 3) {
        std::cerr << "FAIL: final indexes\n";
        return 1;
    }

    // 键不存在时，Get 本身仍是一条已提交、已应用的命令。
    const auto missing = log.submit({Operation::Get, "missing", ""});

    if (missing.index != 4 ||
        missing.result.status != ResultStatus::NotFound ||
        log.size() != 4 ||
        log.committedIndex() != 4 ||
        log.lastApplied() != 4) {
        std::cerr << "FAIL: missing key or index\n";
        return 1;
    }

    std::cout << "Applied indexes: "
              << write.index << ", "
              << append.index << ", "
              << read.index << ", "
              << missing.index << "\n";

    std::cout << "Read value: " << read.result.value << "\n";
    std::cout << "Local log checks passed\n";

    return 0;
}