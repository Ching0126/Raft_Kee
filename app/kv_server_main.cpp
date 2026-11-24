#include "frame.h"
#include "kv_rpc_service.h"
#include "rpc.pb.h"
#include "rpc_dispatcher.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::uint16_t port = 0;

    if (argc != 2 || !wire::parsePort(argv[1], port)) {
        std::cerr << "Usage: kv_server <port 1-65535>\n";
        return 1;
    }

    // 先创建业务对象和适配器，再注册 RPC 方法。
    KvServer server;
    KvRpcService kv_service(server);
    RpcDispatcher dispatcher;

    if (!kv_service.registerWith(dispatcher)) {
        std::cerr << "Failed to register KV service\n";
        return 1;
    }

    const int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        return 1;
    }

    int reuse = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        close(listener);
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(listener,reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0 ||listen(listener, 16) < 0) {
        perror("bind/listen");
        close(listener);
        return 1;
    }

    std::cout << "Listening on 127.0.0.1:"
              << port << std::endl;

    while (true) {
        const int client = accept(listener, nullptr, nullptr);

        if (client < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("accept");
            close(listener);
            return 1;
        }

        std::string body;

        if (wire::setTimeout(client, 3) && wire::receiveFrame(client, body)) {
            rpcwire::Request request;
            rpcwire::Response response;

            if (request.ParseFromString(body)) {
                response = dispatcher.dispatch(request);
            } else {
                response.set_error("invalid RPC request");
            }

            std::string encoded;
            if (response.SerializeToString(&encoded)) {
                wire::sendFrame(client, encoded);
            }
        }

        close(client);
    }
}