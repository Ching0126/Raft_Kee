#ifndef RAFT_KEE_STORE_H
#define RAFT_KEE_STORE_H

#include <optional>
#include <string>
#include <unordered_map>
//内存中的键值存储，当前只提供单线程访问
class KvStore{
public:
    //写入键值，KEY若存在则覆盖旧值
    void put(const std::string& key,const std::string& value);
    //在旧值后追加
    void append(const std::string& key,const std::string& value);
    //返回key对应的值
    std::optional<std::string> get(const std::string& key)const;
private:
    std::unordered_map<std::string,std::string>data_;
};

#endif