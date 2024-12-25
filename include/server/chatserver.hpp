#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace std;
using namespace muduo;
using namespace muduo::net;

// 聊天服务器的主类
class ChatServer
{
public:
    // 初始化聊天服务器对象，构造函数用来构造TcpServer成员,因为TcpServer成员没有默认构造
    ChatServer(EventLoop *loop,               // 事件循环
               const InetAddress &listenAddr, // IP+Port
               const string &nameArg);        // 服务器的名字
    // 启动服务
    void start();

private:
    // 上报链接相关信息的回调函数
    void onConnection(const muduo::net::TcpConnectionPtr &);

    // 上报读写事件相关的回调函数
    void onMessage(const muduo::net::TcpConnectionPtr &,
                   Buffer *,
                   Timestamp );

    // 给TCP 连接 以及 连接断开
    TcpServer _server; // 组合的muduo库，实现服务器功能的类对象
    EventLoop *_loop;  // 指向事件循环对象的指针
};

#endif