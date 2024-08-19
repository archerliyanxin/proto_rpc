#pragma once
#include "memory"
#include "functional"
namespace network{
class Buffer;
class TcpConncetion;
using TcpConncetionPtr =std::shared_ptr<TcpConncetion>;
using ConnectionCallBack = std::function<void (const TcpConncetionPtr&)>;
using CloseCallBack = std::function<void (const TcpConncetionPtr&)>;
using WriteCompleteCallBack = std::function<void (const TcpConncetionPtr&)>;
using HightWaterMarkCallBack = std::function<void (const TcpConncetionPtr&, size_t)>;

using MessageCallBack = std::function<void (const TcpConncetionPtr &, Buffer *)>;

void defaultConnectionCallback(const TcpConncetionPtr &conn);
void defaultMessageCallback(const TcpConncetionPtr &conn, Buffer *buffer);
}