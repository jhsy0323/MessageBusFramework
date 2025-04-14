A message bus framework based on C++

It can greatly accelerate C++ software development

See unit test cpp for how to use it

Here is a brief usage description

```c
#include "MsgHandler.h"
#include <iostream>
using namespace std;

enum class Msg {
	A,
	B,
	C,
	D,
	E,
	F,
};

struct ClassA {
	ClassA() {
		RegisterMsgHandler(Msg::A, &ClassA::NoparaFunc);
		RegisterMsgHandler(Msg::B, &ClassA::HasparaFunc);
		RegisterMsgHandler(Msg::C, [] {
			cout << "lambda NoparaFunc" << endl;
		});
		RegisterMsgHandler(Msg::D, [](int a) {
			cout << "lambda HasparaFunc" << a << endl;
		});
		RegisterMsgHandler(Msg::E, &ClassA::NoparaFuncConst);
		RegisterMsgHandler(Msg::F, &ClassA::HasparaFuncConst);
	}
	void NoparaFunc() {
		cout << "NoparaFunc" << endl;
	}
	void HasparaFunc(int a) {
		cout << "HasparaFunc" << a << endl;
	}
	void NoparaFuncConst()const  {
		cout << "NoparaFuncConst" << endl;
	}
	void HasparaFuncConst(int a) const {
		cout << "HasparaFuncConst" << a << endl;
	}
	~ClassA() {
		Deregister();
	}
};

int main()
{
	ClassA a;
	TriggerMsg(Msg::A);
	TriggerMsg(Msg::B, 999);
	TriggerMsg(Msg::C);
	TriggerMsg(Msg::D, 666);
	TriggerMsg(Msg::E);
	TriggerMsg(Msg::F, 333);
	getchar();
}
```

MsgHandler特性
```
1.提供全局通信，消息触发时，注册该消息的消息处理函数均会被调用，只需包含一个头文件
2.接口简单，只有五个接口
	注册：RegisterMsgHandler
	去注册：Deregister，仅在对象需要释放时调用
	直接触发：TriggerMsg
	异步触发：AsyncExecMsg
	主线程触发：MainExecMsg
3.支持任意枚举类型消息，可通过枚举类型区分模块，枚举值区分消息
	新增任意枚举无需重新编译代码
4.支持三种触发方式：直接触发、异步触发、主线程触发
	直接触发：消息处理函数在触发点线程执行
	异步触发：消息处理函数在线程池中作为一个任务执行
	主线程触发：消息处理函数在主线程中执行
	异步触发和主线程触发需要根据程序定制
5.注册消息处理函数简单，仅需提供消息和消息处理函数
	消息处理函数支持成员函数和lambda表达式
	函数参数支持任意类型
	lambda消息处理函数不需要使用到参数时，可以不写参数列表
6.触发消息简单，仅需提供消息和参数值
	参数支持任意类型，需和注册时保持一致
	参数仅支持值传递，引用和const类型不支持
	需要传递引用参数时，可以注册std::reference_wrapper作为参数，触发时使用std::ref
7.直接触发方式支持优先级，可控制消息处理函数执行顺序
```

InstMsgHandler特性对比
```
1.提供可建立连接的通信，发送者触发消息时，只有被连接者会触发消息处理函数，只需包含一个头文件
2.接口简单，只有五个接口
	注册：XRegisterMsgHandler
	去注册：XDeregister
	直接触发：XTriggerMsg
	连接：XConnect
	取消连接：XDisconnect
3.同上
4.仅支持直接触发，消息处理函数在触发点的线程中执行
5.同上
6.同上
7.不支持触发优先级
```
