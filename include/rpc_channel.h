#ifndef RAFT_KEE_RPC_CHANNEL_H
#define RAFT_KEE_RPC_CHANNEL_H

#include <cstdint>
#include <string>

// 一次同步 RPC 使用一个 TCP 连接。
// 当前目标固定为本机回环地址。
class RpcChannel {
public:
    explicit RpcChannel(std::uint16_t port);

    // 发送服务名、方法名和业务数据。
    // 成功时写入 result；失败时写入 error。
    bool call(const std::string& service,const std::string& method,const std::string& payload,std::string& result,std::string& error) const;

private:
    std::uint16_t port_;
};

#endif