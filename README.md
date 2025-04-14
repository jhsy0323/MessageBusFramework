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
