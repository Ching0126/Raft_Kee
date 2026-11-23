#include "frame.h"
#include "kv.pb.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <string>

int main(int argc,char** argv){
    std::uint16_t port =0;

    if(argc<4||!wire::parsePort(argv[1],port)){
        std::cerr<<"Useage: kv_remote <port> get<key> |"
                    "<port> put|append <key> <value\n";
        return 1;
    }

    const std::string operation = argv[2];
    kvwire::Request request;

    if(operation == "get" && argc == 4){
        request.set_operation(kvwire::GET);
    } else if (operation == "put" && argc == 5) {
        request.set_operation(kvwire::PUT);
    } else if (operation == "append" && argc == 5) {
        request.set_operation(kvwire::APPEND);
    } else {
        std::cerr << "Invalid operation or argument count\n";
        return 1;
    }

    request.set_key(argv[3]);

    if (argc == 5) {
        request.set_value(argv[4]);
    }

    std::string body;
    if (!request.SerializeToString(&body)) {
        std::cerr << "Encode failed\n";
        return 1;
    }

    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    if (!wire::setTimeout(fd, 3)) {
        perror("setsockopt");
        close(fd);
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd,reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0) {
        perror("connect");
        close(fd);
        return 1;
    }

    std::string reply;

    const bool ok =wire::sendFrame(fd, body) &&wire::receiveFrame(fd, reply);

    close(fd);

     if (!ok) {
        std::cerr << "Network or frame error\n";
        return 1;
    }

    kvwire::Response response;
    if (!response.ParseFromString(reply)) {
        std::cerr << "Invalid response\n";
        return 1;
    }

    if (response.status() == kvwire::OK) {
        if (operation == "get") {
            std::cout << "VALUE: " << response.value() << '\n';
        } else {
            std::cout << "OK\n";
        }
    } else if (response.status() == kvwire::NOT_FOUND) {
        std::cout << "NOT FOUND\n";
    } else {
        std::cerr << "Server rejected request\n";
        return 1;
    }

    return 0;
}