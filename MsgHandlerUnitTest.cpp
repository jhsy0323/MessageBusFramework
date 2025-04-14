#include "MsgHandlerUnitTest.h"
#include "MsgHandler.h"
#include <QTimer>

class RunnableTask : public QRunnable {
public:
	RunnableTask(std::function<void()> func) : func_(func) {
		setAutoDelete(true);
	}
	void run() { func_(); }
private:
	std::function<void()> func_;
};

class AsyncThreadPool {
public:
	void AddTask(std::function<void()> func) {
		QThreadPool::globalInstance()->start(new RunnableTask(func));
	}
public:
	static AsyncThreadPool* GetInstance() {
		static AsyncThreadPool i;
		return &i;
	}
private:
	AsyncThreadPool() = default;
};
#define GAsyncThreadPool AsyncThreadPool::GetInstance()

int GetCurMemoryKB()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	auto curMemoryKB = pmc.WorkingSetSize / 1024;
	return curMemoryKB;
}

enum Msg {
	A,
	B,
	C,
};

void MsgHandlerUnitTest::RegisterTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {});
			RegisterMsgHandler(Msg::B, [this](int data) {});
		}
		~ClassA() {
			Deregister();
		}
	};
	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::A, [this] {});
			RegisterMsgHandler(Msg::B, [this](int data) {});
		}
		~ClassB() {
			Deregister();
		}
	};

	ClassA a1;
	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 1);

	ClassA a2;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 2);

	ClassB b1;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 3);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 3);

	QVERIFY(VarifyResult());
}


void MsgHandlerUnitTest::DeregisterTest() {
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {});
			RegisterMsgHandler(Msg::B, [this](int data) {});
		}
		~ClassA() {
			Deregister();
		}
	};
	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	{
		ClassA a1;
		m_results.push_back(globalMsgHandler.size() == 2);
		m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 1);
	}
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 0);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 0);
	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::PriorityPreTest() {
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {});
			RegisterMsgHandler(Msg::B, [this](int data) {});
		}
		~ClassA() {
			Deregister();
		}
	};
	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::A, [this] {}, 1);
			RegisterMsgHandler(Msg::B, [this](int data) {}, 1);
		}
		~ClassB() {
			Deregister();
		}
	};

	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	ClassA a1;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 1);

	ClassA a2;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 2);

	ClassB b;
	m_results.push_back(globalMsgHandler.size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 2);
	m_results.push_back(globalMsgHandler.at("Msg::A").at(1).size() == 1);
	m_results.push_back(globalMsgHandler.at("Msg::B").at(1).size() == 1);

	QVERIFY(VarifyResult());
}
std::vector<int> priorityTestvec;
void MsgHandlerUnitTest::PriorityTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {
				priorityTestvec.push_back(a);
			});
			RegisterMsgHandler(Msg::B, [this] {
				priorityTestvec.push_back(a);
			}, 1);
		}
		~ClassA() {
			Deregister();
		}
		int a = 5;
	};
	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::A, [this] {
				priorityTestvec.push_back(b);
			}, 1);
			RegisterMsgHandler(Msg::B, [this] {
				priorityTestvec.push_back(b);
			});
		}
		~ClassB() {
			Deregister();
		}
		int b = 10;
	};

	ClassA a;
	ClassB b;

	priorityTestvec.clear();
	TriggerMsg(Msg::A);
	m_results.push_back(priorityTestvec.size() == 2);
	m_results.push_back(priorityTestvec[0] == 10 && priorityTestvec[1] == 5);

	priorityTestvec.clear();
	TriggerMsg(Msg::B);
	m_results.push_back(priorityTestvec.size() == 2);
	m_results.push_back(priorityTestvec[0] == 5 && priorityTestvec[1] == 10);

	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::TriggerLambdaNoParamTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {
				a = A;
			});
			RegisterMsgHandler(Msg::B, [this] {
				a = B;
			});
		}
		~ClassA() {
			Deregister();
		}
		int a = 0;
	};

	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::A, [this] {
				b = A;
			});
			RegisterMsgHandler(Msg::B, [this] {
				b = B;
			});
		}
		~ClassB() {
			Deregister();
		}
		int b = 0;
	};
	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	ClassA a1;
	ClassB b1;
	m_results.push_back(a1.a == 0);
	m_results.push_back(b1.b == 0);

	TriggerMsg(Msg::A);
	m_results.push_back(a1.a == A);
	m_results.push_back(b1.b == A);

	TriggerMsg(Msg::B);
	m_results.push_back(a1.a == B);
	m_results.push_back(b1.b == B);

	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::TriggerLambdaHasParamTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::C, [this](int data) {
				a = data;
			});
		}
		~ClassA() {
			Deregister();
		}
		int a = 0;
	};

	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::C, [this](int data) {
				b = data;
			});
		}
		~ClassB() {
			Deregister();
		}
		int b = 0;
	};

	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;

	ClassA a1;
	ClassB b1;
	m_results.push_back(a1.a == 0);
	m_results.push_back(b1.b == 0);

	TriggerMsg(Msg::C, 5);
	m_results.push_back(a1.a == 5);
	m_results.push_back(b1.b == 5);

	TriggerMsg(Msg::C, 999);
	m_results.push_back(a1.a == 999);
	m_results.push_back(b1.b == 999);

	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::TriggerMemberFuncNoParamTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, &ClassA::NoParamFunc1);
			RegisterMsgHandler(Msg::B, &ClassA::NoParamFunc2);
		}
		void NoParamFunc1() {
			a = A;
		}
		void NoParamFunc2() {
			a = B;
		}
		~ClassA() {
			Deregister();
		}
		int a = 0;
	};

	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::A, &ClassB::NoParamFunc1);
			RegisterMsgHandler(Msg::B, &ClassB::NoParamFunc2);
		}
		void NoParamFunc1() {
			b = A;
		}
		void NoParamFunc2() {
			b = B;
		}
		~ClassB() {
			Deregister();
		}
		int b = 0;
	};
	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	ClassA a1;
	ClassB b1;
	m_results.push_back(a1.a == 0);
	m_results.push_back(b1.b == 0);

	TriggerMsg(Msg::A);
	m_results.push_back(a1.a == A);
	m_results.push_back(b1.b == A);

	TriggerMsg(Msg::B);
	m_results.push_back(a1.a == B);
	m_results.push_back(b1.b == B);

	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::TriggerMemberFuncHasParamTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::C, &ClassA::HasParamFunc);
		}
		void HasParamFunc(int data) {
			a = data;
		}
		~ClassA() {
			Deregister();
		}
		int a = 0;
	};

	struct ClassB {
		ClassB() {
			RegisterMsgHandler(Msg::C, &ClassB::HasParamFunc);
		}
		void HasParamFunc(int data) {
			b = data;
		}
		~ClassB() {
			Deregister();
		}
		int b = 0;
	};

	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;

	ClassA a1;
	ClassB b1;
	m_results.push_back(a1.a == 0);
	m_results.push_back(b1.b == 0);

	TriggerMsg(Msg::C, 5);
	m_results.push_back(a1.a == 5);
	m_results.push_back(b1.b == 5);

	TriggerMsg(Msg::C, 999);
	m_results.push_back(a1.a == 999);
	m_results.push_back(b1.b == 999);

	QVERIFY(VarifyResult());
}

std::vector<std::thread::id> AsyncTriggerTestVec;
void MsgHandlerUnitTest::AsyncTriggerTest()
{
	AsyncTriggerTestVec.push_back(std::this_thread::get_id());
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {
				AsyncTriggerTestVec.push_back(std::this_thread::get_id());
			});
		}
		~ClassA() {
			Deregister();
		}
	};
	ClassA a;
	AsyncExecMsg(Msg::A);

	while (AsyncTriggerTestVec.size() != 2) {
		QCoreApplication::processEvents();
	}
	m_results.push_back(AsyncTriggerTestVec[0] != AsyncTriggerTestVec[1]);
	QVERIFY(VarifyResult());
}

std::vector<std::thread::id> MainTriggerTestVec;
void MsgHandlerUnitTest::MainTriggerTest()
{
	MainTriggerTestVec.clear();
	MainTriggerTestVec.push_back(std::this_thread::get_id());
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {
				MainTriggerTestVec.push_back(std::this_thread::get_id());
				MainExecMsg(Msg::B);
			});
			RegisterMsgHandler(Msg::B, [this] {
				MainTriggerTestVec.push_back(std::this_thread::get_id());
			});
		}
		~ClassA() {
			Deregister();
		}
	};
	ClassA a;
	AsyncExecMsg(Msg::A);

	while (MainTriggerTestVec.size() != 3) {
		QCoreApplication::processEvents();
	}
	m_results.push_back(MainTriggerTestVec[0] != MainTriggerTestVec[1] 
		&& MainTriggerTestVec[1] != MainTriggerTestVec[2] 
		&& MainTriggerTestVec[0] == MainTriggerTestVec[2]);
	QVERIFY(VarifyResult());
}

void MsgHandlerUnitTest::MemoryLeakTest()
{
	struct ClassA {
		ClassA() {
			RegisterMsgHandler(Msg::A, [this] {
				a = A;
			});
			RegisterMsgHandler(Msg::B, [this] {
				a = B;
			});
			RegisterMsgHandler(Msg::C, [this](int data) {
				a = std::any_cast<int>(data);
			});
		}
		~ClassA() {
			Deregister();
		}
		int a = 0;
	};
	const auto& globalMsgHandler = MsgHandler::GetInstance()->globalMsgHandler_;
	auto beginMemory = GetCurMemoryKB();
	{
		std::vector<ClassA*> vec;
		for (int i = 0; i < 100000; ++i) {
			vec.push_back(new ClassA);
		}
		m_results.push_back(globalMsgHandler.size() == 3);
		m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::C").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 100000);
		m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 100000);
		m_results.push_back(globalMsgHandler.at("Msg::C").at(0).size() == 100000);
		for (auto& v : vec) {
			delete v;
		}
		vec.clear();
		m_results.push_back(globalMsgHandler.size() == 3);
		m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::C").size() == 1);
		m_results.push_back(globalMsgHandler.at("Msg::A").at(0).size() == 0);
		m_results.push_back(globalMsgHandler.at("Msg::B").at(0).size() == 0);
		m_results.push_back(globalMsgHandler.at("Msg::C").at(0).size() == 0);
	}
	auto endMemory = GetCurMemoryKB();
	QVERIFY(endMemory - beginMemory < 30);
}

bool MsgHandlerUnitTest::VarifyResult()
{
	bool result = true;
	for (auto& rlt : m_results) {
		if (!rlt) {
			result = false;
			break;
		}
	}
	return result;
}

void MsgHandlerUnitTest::initTestCase()
{
	MsgHandler::GetInstance()->SetAsyncExecFunc([](std::function<void()> func) { GAsyncThreadPool->AddTask(func); });
	MsgHandler::GetInstance()->SetMainExecFunc([](std::function<void()> func) { QTimer::singleShot(0, qApp, func); });
}

void MsgHandlerUnitTest::cleanupTestCase()
{
}

void MsgHandlerUnitTest::init()
{
	m_results.clear();
	MsgHandler::GetInstance()->globalMsgHandler_.clear();
}

void MsgHandlerUnitTest::cleanup()
{
}
