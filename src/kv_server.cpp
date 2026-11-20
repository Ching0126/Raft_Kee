#include "kv_server.h"

void KvServer::put(const std::string& key, const std::string& value) {
    store_.put(key, value);
}

void KvServer::append(const std::string& key, const std::string& value) {
    store_.append(key, value);
}

std::optional<std::string> KvServer::get(const std::string& key) const {
    return store_.get(key);
}