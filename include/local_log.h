#ifndef RAFT_KEE_LOCAL_LOG_H
#define RAFT_KEE_LOCAL_LOG_H

#include "command.h"
#include "kv_state_machine.h"

#include <cstddef>
#include <vector>
//一条内存日志，索引从1开始
struct LogEntry{
    std::size_t index;
    Command command;
};
//日志应用完成后的通知，携带索引和执行结果
struct ApplyMsg{
    std::size_t index;
    CommandResult result;
};

class LocalLog{
public:
    ApplyMsg submit(const Command& command);

    std::size_t size() const;//日志数量

    std::size_t committedIndex()const;//最近提交的日志索引

    std::size_t lastApplied()const;//最近提交到状态机的日志索引
private:
    std::vector<LogEntry> entries_;
    std::size_t committed_index_ = 0;
    std::size_t last_applied_ = 0;
    KvStateMachine state_machine_;
};
#endif