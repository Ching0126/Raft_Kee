#ifndef RAFT_KEE_COMMAND_H
#define RAFT_KEE_COMMAND_H

#include <string>

enum class Operation{
    Put,
    Append,
    Get
};

struct Command{
    Operation operation;
    std::string key;
    std::string value;
};

enum class ResultStatus{
    Ok,
    NotFound,
    InvalidOperation
};

struct CommandResult{
    ResultStatus status;
    std::string value;
};

#endif