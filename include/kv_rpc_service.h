#ifndef RAFT_KEE_KV_RPC_SERVICE_H
#define RAFT_KEE_KV_RPC_SERVICE_H

#include "kv_server.h"
#include "rpc_dispatcher.h"

#include <string>


class KvRpcService{
public:
    explicit KvRpcService(KvServer& server);
    
    bool registerWith(RpcDispatcher& dispatcher);
private:
    bool execute(const std::string& payload,std::string& result);
    KvServer& server_;
};

#endif