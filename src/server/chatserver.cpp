#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <functional> // 函数对象绑定器
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json; // 起别名

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调, 有三个参数预留三个参数占位符
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动ChatServer服务
void ChatServer::start()
{
    _server.start();
}

// TcpServer绑定的回调函数，当有新连接或连接中断时调用
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    // 客户端断开链接
    if (!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown(); // close(fd)
    }
}
// TcpServer绑定的回调函数，当有新数据时调用,专门处理读写事件
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buf = buffer->retrieveAllAsString(); // 从buffer缓冲区拿到数据放到字符串里
    // 数据的反序列化
    json js = json::parse(buf);
    // 达到的目的：完全解耦网络模块的代码和业务模块的代码
    // 通过js["msgid"] 获取=》业务handler
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，；来执行相应的业务处理
    msgHandler(conn, js, time);
}