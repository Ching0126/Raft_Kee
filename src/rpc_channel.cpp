#include "rpc_channel.h"

#include "frame.h"
#include "rpc.pb.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

RpcChannel::RpcChannel(std::uint16_t port)
    : port_(port) {}

bool RpcChannel::call(
    const std::string& service,
    const std::string& method,
    const std::string& payload,
    std::string& result,
    std::string& error
) const {
    result.clear();
    error.clear();

    rpcwire::Request request;
    request.set_service(service);
    request.set_method(method);
    request.set_payload(payload);

    std::string encoded;
    if (!request.SerializeToString(&encoded)) {
        error = "encode RPC request failed";
        return false;
    }

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error = "socket failed";
        return false;
    }

    // 使用你第四批项目中的函数名 setTimeout。
    if (!wire::setTimeout(fd, 3)) {
        close(fd);
        error = "set socket timeout failed";
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd,reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0) {
        close(fd);
        error = "connect failed";
        return false;
    }

    std::string reply;

    const bool ok =wire::sendFrame(fd, encoded) && wire::receiveFrame(fd, reply);

    close(fd);

    if (!ok) {
        error = "network or frame error";
        return false;
    }

    rpcwire::Response response;
    if (!response.ParseFromString(reply)) {
        error = "invalid RPC response";
        return false;
    }

    if (!response.success()) {
        error = response.error();
        return false;
    }

    result = response.payload();
    return true;
}