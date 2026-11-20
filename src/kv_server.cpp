#include "kv_server.h"

void KvServer::put(const std::string& key, const std::string& value) {
    const Command command{Operation::Put, key, value};
    state_machine_.apply(command);
}

void KvServer::append(const std::string& key, const std::string& value) {
    const Command command{Operation::Append, key, value};
    state_machine_.apply(command);
}

std::optional<std::string> KvServer::get(const std::string& key) {
    const Command command{Operation::Get, key, ""};
    const CommandResult result = state_machine_.apply(command);

    if (result.status == ResultStatus::NotFound) {
        return std::nullopt;
    }

    return result.value;
}