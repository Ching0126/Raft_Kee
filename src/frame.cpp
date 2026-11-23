#include "frame.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <cerrno>
#include <charconv>
#include <cstring>
#include <system_error>

namespace{
//循环发送，直到全部数据发送完成或发生错误。
bool sendAll(int fd,const char* data,std::size_t size){
    while(size>0){
        const ssize_t n = send(fd,data,size,MSG_NOSIGNAL);

        if(n<0&&errno==EINTR){
            continue;
        }

        if(n<=0){
            return false;
        }

        data+=n;//让data指针向后移动n个字符
        size-=static_cast<std::size_t>(n);
    }
    return true;
}

bool receiveAll(int fd,char* data,std::size_t size){
    while(size>0){
        const ssize_t n = recv(fd,data,size,0);

        if(n<0&&errno==EINTR){
            continue;
        }

        if(n<=0){
            return false;
        }

        data+=n;
        size-=static_cast<std::size_t>(n);
    }
    return true;
}
}//namespace

namespace wire{
//端口号转化为整数
bool parsePort(const char* text,std::uint16_t& port){
    unsigned int value = 0;
    const char* end = text + std::strlen(text);

    const auto result = std::from_chars(text,end,value);//逐字符转int 

    if(result.ec != std::errc{} || result.ptr!=end || value==0 || value>65535){
        return false;
    }

    port = static_cast<std::uint16_t>(value);//输出参数port
    return true;
}

bool sendFrame(int fd,const std::string& body){
    if(body.size()>kMaxFrameSize){
        return false;
    }

    const std::uint32_t length = htonl(static_cast<std::uint32_t>(body.size()));//32位整数转化为网络字节序 大小端转换其实 转化成大端

    return sendAll(fd,reinterpret_cast<const char*>(&length),sizeof(length))&&sendAll(fd,body.data(),body.size());
}

bool receiveFrame(int fd,std::string& body){
    std::uint32_t length = 0;
   
    if(!receiveAll(fd,reinterpret_cast<char*>(&length),sizeof(length))){
        return false;
    }

    length = ntohl(length);

    if(length>kMaxFrameSize){
        return false;
    }

    body.resize(length);
    return receiveAll(fd,body.data(),body.size());
}

bool setTimeout(int fd,int seconds){
    const timeval timeout{seconds,0};

    return setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==0 &&
        setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout))==0;
}

}