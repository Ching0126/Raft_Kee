#include "rpc_dispatcher.h"

bool RpcDispatcher::registerMethod(const std::string& service,const std::string& method,Handler handler){
    if (service.empty() || method.empty() || !handler) {
        return false;
    }

    // emplace 的 second 表示是否成功插入。
    return methods_.emplace(std::make_pair(service, method),std::move(handler)).second;
}

rpcwire::Response RpcDispatcher::dispatch(const rpcwire::Request& request)const{
    rpcwire::Response response;

    const auto it = methods_.find({request.service(),request.method()});

    if(it==methods_.end()){
        response.set_error("unknown service or method");
        return response;
    }

    std::string payload;

    if(!it->second(request.payload(),payload)){
        response.set_error("handler failed");
        return response;
    }

    response.set_success(true);
    response.set_payload(payload);
    return response;
}