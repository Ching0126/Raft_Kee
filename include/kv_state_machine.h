#ifndef RAFT_KEE_KV_STATE_MACHINE_H
#define RAFT_KEE_KV_STATE_MACHINE_H

#include "command.h"
#include "kv_store.h"

class KvStateMachine{
public:
    //执行一条命令，返回状态
    CommandResult apply(const Command& command);
private:
    KvStore store_;
};

#endif