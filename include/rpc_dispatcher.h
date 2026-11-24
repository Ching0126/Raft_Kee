#ifndef RAFT_KEE_RPC_DISPATCHER_H
#define RAFT_KEE_RPC_DISPATCHER_H

#include "rpc.pb.h"

#include <functional>
#include <map>
#include <string>
#include <utility>

class RpcDispatcher{
public:
    using Handler = std::function<bool(const std::string&,std::string&)>;

    bool registerMethod(const std::string& service,const std::string& method,Handler handler);

    rpcwire::Response dispatch(const rpcwire::Request& request)const;

private:
    std::map<std::pair<std::string,std::string>,Handler> methods_;
};


#endif