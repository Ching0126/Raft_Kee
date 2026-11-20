#include "kv_server.h"

#include <iostream>
#include <sstream>
#include <string>

int main(){
    KvServer server;
    std::string line;

    std::cout<<"Commands: put<key> <value> ,append<key> <value>, "
                "get <key> , exit\n";
    
    while(std::getline(std::cin,line)){
        std::istringstream input(line);
        std::string command;
        std::string key;
        std::string value;
        std::string extra;

        if(!(input>>command)){
            continue;//忽略空行
        }

        if(command == "exit"){
            if(input>>extra){
                std::cout<<"ERROR:usage:exit\n";
                continue;
            }
            break;
        }

        if(command == "put" || command == "append"){
            if(!(input>>key>>value)||(input>>extra)){
                std::cout<<"ERROR:useage:"<<command<<"<key> <value>\n";
                continue;
            }

            if(command == "put"){
                server.put(key,value);
            }else{
                server.append(key,value);
            }

            std::cout<<"OK\n";
        }else if(command == "get"){
            if(!(input>>key) || (input>>extra)){
                std::cout<<"ERROR useage:get key>\n";
                continue;
            }

            const auto result = server.get(key);
            if(result){
                std::cout<<"VALUE: "<<*result<<'\n';
            }else{
                std::cout<<"NOT FOUND\n";
            }
        }else{
            std::cout<<"ERROR: unknown command\n";
        }
    }
    return 0;
}