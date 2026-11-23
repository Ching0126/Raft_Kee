#ifndef RAFT_KEE_FRAME_H
#define RAFT_KEE_FRAME_H

#include <cstdint>
#include <string>

namespace wire{
    constexpr std::uint32_t kMaxFrameSize = 1024 * 1024 ;
    //解析端口
    bool parsePort(const char* text,std::uint16_t& port);
    //发送或者读取一条消息
    bool sendFrame(int fd,const std::string& body);
    bool receiveFrame(int fd,std::string& body);
    //设置连接超时时间，单位为秒
    bool setTimeout(int fd,int seconds);
}//namespace

#endif