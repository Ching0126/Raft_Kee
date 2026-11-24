#include "kv.pb.h"
#include "kv_rpc_service.h"
#include "rpc_dispatcher.h"

#include <iostream>
#include <string>

int main() {
    KvServer server;
    KvRpcService service(server);
    RpcDispatcher dispatcher;

    // 首次注册成功，同名方法不能重复注册。
    if (!service.registerWith(dispatcher) ||
        service.registerWith(dispatcher)) {
        std::cerr << "FAIL: registration\n";
        return 1;
    }

    rpcwire::Request request;
    request.set_service("Unknown");
    request.set_method("Execute");

    if (dispatcher.dispatch(request).success()) {
        std::cerr << "FAIL: unknown service\n";
        return 1;
    }

    request.set_service("KvService");
    request.set_method("Unknown");

    if (dispatcher.dispatch(request).success()) {
        std::cerr << "FAIL: unknown method\n";
        return 1;
    }

    request.set_method("Execute");

    kvwire::Request put;
    put.set_operation(kvwire::PUT);
    put.set_key("x");
    put.set_value("hello");

    std::string payload;
    if (!put.SerializeToString(&payload)) {
        return 1;
    }

    request.set_payload(payload);
    auto reply = dispatcher.dispatch(request);

    kvwire::Response result;
    if (!reply.success() ||
        !result.ParseFromString(reply.payload()) ||
        result.status() != kvwire::OK) {
        std::cerr << "FAIL: put\n";
        return 1;
    }

    kvwire::Request get;
    get.set_operation(kvwire::GET);
    get.set_key("x");

    if (!get.SerializeToString(&payload)) {
        return 1;
    }

    request.set_payload(payload);
    reply = dispatcher.dispatch(request);

    if (!reply.success() ||
        !result.ParseFromString(reply.payload()) ||
        result.status() != kvwire::OK ||
        result.value() != "hello") {
        std::cerr << "FAIL: get\n";
        return 1;
    }

    // RPC 找到了方法，但业务内容不是有效的 Protobuf。
    request.set_payload(
        std::string(1, static_cast<char>(0xff))
    );

    reply = dispatcher.dispatch(request);

    if (!reply.success() ||
        !result.ParseFromString(reply.payload()) ||
        result.status() != kvwire::BAD_REQUEST) {
        std::cerr << "FAIL: invalid business payload\n";
        return 1;
    }

    std::cout << "RPC dispatcher checks passed\n";
    return 0;
}