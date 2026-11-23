#include "frame.h"
#include "kv.pb.h"
#include "kv_server.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <string>

kvwire::Response handleRequest(KvServer& server, const std::string& body){
    kvwire::Request request;
    kvwire::Response response;

    if(!request.ParseFromString(body)||request.key().empty()){
        response.set_status(kvwire::BAD_REQUEST);
        return response;
    }

    switch (request.operation()){
        case kvwire::PUT:
            server.put(request.key(),request.value());
            response.set_status(kvwire::OK);
            break;
        case kvwire::APPEND:
            server.append(request.key(),request.value());
            response.set_status(kvwire::OK);
            break;
        case kvwire::GET :{
            const auto result = server.get(request.key());

            if(result){
                response.set_status(kvwire::OK);
                response.set_value(*result);
            }else{
                response.set_status(kvwire::NOT_FOUND);
            }
        }
            break;
        default:
            response.set_status(kvwire::BAD_REQUEST);
    }
    return response;
}

int main(int argc,char** argv){
    std::uint16_t port = 0;

    if(argc!=2 || !wire::parsePort(argv[1],port)){
        std::cerr<<"Useage: kv_server <port 1-65535>\n";
        return 1;
    }

    const int listener = socket(AF_INET,SOCK_STREAM,0);
    if(listener<0){
        perror("socket");
        return 1;
    }

    int reuse = 1;
    if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
        perror("setsocketopt");
        close(listener);
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    if(bind(listener,reinterpret_cast<sockaddr*>(&address),sizeof(address))<0 || listen(listener,16)<0){
        perror("bind/listen");
        close(listener);
        return 1;
    }

    KvServer server;

    std::cout<<"Lisrening on 127.0.0.1:"<<port<<std::endl;

    while(true){
        const int client = accept(listener,nullptr,nullptr);

        if(client<0){
            if(errno==EINTR){
                continue;
            }

            perror("accept");
            close(listener);
            return 1;
        }

         std::string body;

        if (wire::setTimeout(client, 3) &&
            wire::receiveFrame(client, body)) {
            const auto response = handleRequest(server, body);

            std::string encoded;
            if (response.SerializeToString(&encoded)) {
                wire::sendFrame(client, encoded);
            }
        }
        close(client);
    }
}