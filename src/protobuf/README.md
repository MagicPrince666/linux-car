# protobuf用法

protocol buffers 是一种语言无关、平台无关、可扩展的序列化结构数据的方法，它可用于（数据）通信协议、数据存储等。

Protocol Buffers 是一种灵活，高效，自动化机制的结构数据序列化方法－可类比 XML，但是比 XML 更小（3 ~ 10倍）、更快（20 ~ 100倍）、更为简单。

你可以定义数据的结构，然后使用特殊生成的源代码轻松的在各种数据流中使用各种语言进行编写和读取结构数据。你甚至可以更新数据结构，而不破坏由旧数据结构编译的已部署程序。

推荐使用3.11以上版本，支持比较多的新特性

## ubuntu18.04编译安装
以master为例，可选其他已发布的release版本

```
sudo apt-get install autoconf automake libtool curl make g++ unzip

git clone https://github.com/protocolbuffers/protobuf.git

cd protobuf

./autogen.sh

./configure

make

make check # 可不检查，没影响

sudo make install # 安装普通版本

sudo make install-strip # 或安装精简版 strip后体积大幅减小

sudo ldconfig # refresh shared library cache.

```

## 交叉编译
### 以rk3308为例
```
git clone https://github.com/protocolbuffers/protobuf.git
./autogen.sh
./configure --host=arm-linux CC=aarch64-rockchip-linux-gnu-gcc CXX=aarch64-rockchip-linux-gnu-g++ STRIP=aarch64-rockchip-linux-gnu-strip --disable-protoc --prefix=$PWD/install
make install-strip
```
### r328
```
git clone https://github.com/protocolbuffers/protobuf.git
./autogen.sh
./configure --host=arm-linux CC=arm-openwrt-linux-gnueabi-gcc CXX=arm-openwrt-linux-gnueabi-g++ STRIP=arm-openwrt-linux-gnueabi-strip --disable-protoc --prefix=$PWD/install
make install-strip
```

## 用例说明

### 第一步，创建 .proto 文件，定义数据结构，如下例1所示：
```
// 使用proto3
syntax = "proto3";
// 使用protobuf-lite 版本
option optimize_for = LITE_RUNTIME;
// 包名
package leo;

message Example {
    optional string stringVal = 1;
    optional bytes bytesVal = 2;
    message EmbeddedMessage {
        int32 int32Val = 1;
        string stringVal = 2;
    }
    optional EmbeddedMessage embeddedExample1 = 3;
    repeated int32 repeatedInt32Val = 4;
    repeated string repeatedStringVal = 5;
}
```

我们在上例中定义了一个名为 Example 的 消息，语法很简单，message 关键字后跟上消息名称：
```
message xxx {

}
```
之后我们在其中定义了 message 具有的字段，形式为：
```
message xxx {
  // 字段规则：required -> 字段只能也必须出现 1 次
  // 字段规则：optional -> 字段可出现 0 次或1次
  // 字段规则：repeated -> 字段可出现任意多次（包括 0）
  // 类型：int32、int64、sint32、sint64、string、32-bit ....
  // 字段编号：0 ~ 536870911（除去 19000 到 19999 之间的数字）
  字段规则 类型 名称 = 字段编号;
}
```
### 第二步，protoc 编译 .proto 文件生成读写接口
以C++为例,首先以cxx.proto为模板，生成C++代码
```
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/cxx.proto
```
protoc会将cxx.proto里面的数据结构转换成相应的C++代码，开发者无需关心代码怎么实现，因为生成的代码不是给人看的，机器能理解就行

将生成的cxx.pb.cc（一般会生成.pb.cc）加入到工程里面，参与编译


### 第三步，调用接口实现序列化、反序列化以及读写
参考test.cc

具体用法参考官方说明

https://developers.google.com/protocol-buffers/docs/cpptutorial