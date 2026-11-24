#include "kv_rpc_service.h"

#include "kv.pb.h"

KvRpcService::KvRpcService(KvServer& server)
    :server_(server){}

bool KvRpcService::registerWith(RpcDispatcher& dispatcher){
     return dispatcher.registerMethod(
        "KvService",
        "Execute",
        [this](
            const std::string& request,
            std::string& response
        ) {
            return execute(request, response);
        }
    );
}

bool KvRpcService::execute(
    const std::string& payload,
    std::string& result
) {
    kvwire::Request request;
    kvwire::Response response;

    if (!request.ParseFromString(payload) ||
        request.key().empty()) {
        response.set_status(kvwire::BAD_REQUEST);
        return response.SerializeToString(&result);
    }

    switch (request.operation()) {
    case kvwire::PUT:
        server_.put(request.key(), request.value());
        response.set_status(kvwire::OK);
        break;

    case kvwire::APPEND:
        server_.append(request.key(), request.value());
        response.set_status(kvwire::OK);
        break;

    case kvwire::GET: {
        const auto value = server_.get(request.key());

        if (value) {
            response.set_status(kvwire::OK);
            response.set_value(*value);
        } else {
            response.set_status(kvwire::NOT_FOUND);
        }

        break;
    }

    default:
        response.set_status(kvwire::BAD_REQUEST);
    }

    return response.SerializeToString(&result);
}