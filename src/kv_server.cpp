#include "kv_server.h"

void KvServer::put(const std::string& key, const std::string& value) {
    const Command command{Operation::Put, key, value};
    log_.submit(command);
}

void KvServer::append(const std::string& key, const std::string& value) {
    const Command command{Operation::Append, key, value};
    log_.submit(command);
}

std::optional<std::string> KvServer::get(const std::string& key) {
    const Command command{Operation::Get, key, ""};
    const ApplyMsg applied = log_.submit(command);

    if (applied.result.status == ResultStatus::NotFound) {
        return std::nullopt;
    }

    return applied.result.value;
}