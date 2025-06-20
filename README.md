A message bus framework based on C++, It can greatly accelerate C++ software development
Here is a example
```cpp
#include "MsgHandler.h"
#include <iostream>
using namespace std;

enum class Msg {
	test_1,
	test_2,
	test_3,
	test_4,
	test_5,
	test_6,
	test_7,
	test_8,
	test_9,
	test_10,
};

struct ClassA {
	ClassA() {
		RegisterMsgHandler(Msg::test_1, &ClassA::NoParaFunc);
		RegisterMsgHandler(Msg::test_2, &ClassA::HasparaFunc);
		RegisterMsgHandler(Msg::test_3, &ClassA::NoParaFuncConst);
		RegisterMsgHandler(Msg::test_4, &ClassA::HasParaFuncConst);
		RegisterMsgHandler(Msg::test_5, [] {cout << "test 5 success" << endl; });
		RegisterMsgHandler(Msg::test_6, [](int a) {cout << "test 6 success, a = " << a << endl; });
		RegisterMsgHandler(Msg::test_7, &ClassA::intRefFunc);
		RegisterMsgHandler(Msg::test_8, &ClassA::constIntRefFunc);
		RegisterMsgHandler(Msg::test_9, &ClassA::MultiParamFunc);
	}
	void NoParaFunc() { cout << "test 1 success" << endl; }
	void HasparaFunc(int a) { cout << "test 2 success, a = " << a << endl; }
	void NoParaFuncConst()const { cout << "test 3 success" << endl; }
	void HasParaFuncConst(int a) const { cout << "test 4 success, a = " << a << endl; }
	void intRefFunc(int& a) { cout << "test 7 success, a = " << a << endl; }
	void constIntRefFunc(const int& a) { cout << "test 8 success, a = " << a << endl; }
	void MultiParamFunc(int a, int b, int c, int d) { cout << "test 9 success, value = " << a << b << c << d << endl; }
	~ClassA() {
		Deregister();
	}
};

int main()
{
	ClassA a;
	SyncExecMsg(Msg::test_1);				//输出test 1 success
	SyncExecMsg(Msg::test_2, 2);				//输出test 2 success, a = 2
	SyncExecMsg(Msg::test_3);				//输出test 3 success
	SyncExecMsg(Msg::test_4, 4);				//输出test 4 success, a = 4
	SyncExecMsg(Msg::test_5);				//输出test 5 success
	SyncExecMsg(Msg::test_6, 6);				//输出test 6 success, a = 6
	int i = 7;
	SyncExecMsg(Msg::test_7, i);				//输出test 7 success, a = 7
	SyncExecMsg(Msg::test_8, 8);				//输出test 8 success, a = 8
	SyncExecMsg(Msg::test_9, 1, 2, 3, 4);			//输出test 9 success, value = 1234
	cout << "test end" << endl;
	getchar();
}
```

MsgHandler特性
```
1.提供全局通信
2.接口简单，只有五个接口
	注册：RegisterMsgHandler
	去注册：Deregister，仅在对象需要释放时调用
	直接触发：SyncExecMsg
	异步触发：AsyncExecMsg
	主线程触发：MainExecMsg
3.消息支持任意枚举类型，可通过枚举类型区分模块，枚举值区分消息
	新增任意枚举无需重新编译代码
4.触发消息支持三种方式：直接触发、异步触发、主线程触发
	直接触发：消息处理函数在触发点线程执行
	异步触发：消息处理函数在线程池中作为一个任务执行
	主线程触发：消息处理函数在主线程中执行
	异步触发和主线程触发需要自行设置执行异步任务的线程池和投递任务给主线程的函数
5.注册消息处理函数简单，仅需提供消息和消息处理函数
	消息处理函数支持成员函数和lambda表达式
6.注册消息处理函数支持任意类型参数
	lambda消息处理函数不需要使用到参数时，可以不写参数列表
7.触发消息简单，仅需提供消息和参数值
8.参数支持基本常用类型，如T、T&、const T&、T*、const T*、const T*const、T*const、const T*&、const T*const&、T*const&，由于右值引用T&&容易导致异步编码时的编译问题，且一般除了移动构造移动赋值很少用到该参数类型，故不支持。
	异步触发方式，需要注意线程函数参数为引用时的情况，变量生命周期可能提前被释放，线程可能操作被释放的资源
9.直接触发方式支持优先级，可控制消息处理函数执行顺序
10.发送消息触发消息执行函数的效率基本等价于直接的函数调用，不会存在太大开销

对比QT信号槽通信
1.无需继承，声明Q_OBJECT宏
	但是在需要释放的对象析构函数需要添加去注册化
2.QT需要建立连接，且需要在在中介者中建立连接用于降低耦合性
	无需连接，全局触发
3.QT新增消息需要定义新的信号，包含头文件的源文件需要重新编译
	新增消息使用枚举，源文件无需重新编译
4.QT异步处理需要建立新的线程对象，在完成时返回信号到主线程
	使用QtConcurrent和QFutureWatcher可以实现类似框架一致功能
5.QT在复杂场景需要建立多个连接才能完成消息传递
```

InstMsgHandler特性对比
```
基本同上，如下有所差别
1.提供可建立连接的通信，发送者触发消息时，只有被连接者会触发消息处理函数
2.接口添加X前缀，仅支持直接触发，和新增建立连接和断开接口
	注册：XRegisterMsgHandler
	去注册：XDeregister
	直接触发：XTriggerMsg
	连接：XConnect
	取消连接：XDisconnect
3.不支持触发优先级
```
