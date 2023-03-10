#include "RemoteAdapter.h"
#include "sbus.h"
#include <memory>

// 类模式的Adapter
RemoteClassAdapter::RemoteClassAdapter() : RemoteProduct("", false)
{
}

RemoteClassAdapter::~RemoteClassAdapter()
{
}

void RemoteClassAdapter::RemoteRequest(RemoteState &rc_data)
{
    this->Request(rc_data);
}

// 对象模式的Adapter
SbusRemoteObjAdapter::SbusRemoteObjAdapter()
{
    // 创建具体的y遥控工厂
    std::shared_ptr<RemoteFactory> factory(new SbusRemote());
    // 通过工厂方法创建遥控产品
    std::shared_ptr<RemoteProduct> myremote(factory->CreateRemoteProduct());
    remote_ = myremote;
}

// SbusRemoteObjAdapter::SbusRemoteObjAdapter(SbusRemote* remote)
// {
//     this->remote_ptr_ = remote;
// }

SbusRemoteObjAdapter::~SbusRemoteObjAdapter()
{
}

void SbusRemoteObjAdapter::RemoteRequest(RemoteState &rc_data)
{
    // RemoteState rc_data;
    this->remote_->Request(rc_data);
}
