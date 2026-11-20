#ifndef RAFT_KEE_KV_SERVER_H
#define RAFT_KEE_KV_SERVER_H

#include "local_log.h"

#include <optional>
#include <string>

class KvServer{
public:
    void put(const std::string& key , const std::string& value);

    void append(const std::string& key,const std::string& value);

    std::optional<std::string> get(const std::string& key);
private:
    LocalLog log_;
};

#endif