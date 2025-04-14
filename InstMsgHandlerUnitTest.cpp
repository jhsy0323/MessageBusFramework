#include "InstMsgHandlerUnitTest.h"
#include "InstMsgHandler.h"

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

void InstMsgHandlerUnitTest::RegisterTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   };

   ClassA a;
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);

   ClassB b;
   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   QVERIFY(VarifyResult());
}

void InstMsgHandlerUnitTest::DeregisterTest() 
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   };
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;
   {
   	ClassA a;
   	m_results.push_back(globalMsgHandler.size() == 2);
   	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
   	m_results.push_back(communicators.size() == 0);
   	{
   		ClassB b;
   		m_results.push_back(globalMsgHandler.size() == 2);
   		m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   		m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);
   		XConnect(&a, &b);
   		m_results.push_back(communicators.size() == 2);
   		m_results.push_back(communicators.at(&a).size() == 1);
   		m_results.push_back(communicators.at(&b).size() == 1);
   	}
   	m_results.push_back(globalMsgHandler.size() == 2);
   	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);
   	m_results.push_back(communicators.size() == 1);
   	m_results.push_back(communicators.at(&a).size() == 0);
   }

   m_results.push_back(communicators.size() == 0);
   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 0);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 0);
   QVERIFY(VarifyResult());
}

void InstMsgHandlerUnitTest::XConnectABTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   };

   ClassA a;
   ClassB b;
   XConnect(&a, &b);

   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);
   QVERIFY(VarifyResult());
}

void InstMsgHandlerUnitTest::XDisconnectABTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   };

   ClassA a;
   ClassB b;
   XConnect(&a, &b);

   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   XDisconnect(&a, &b);
   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 0);
   m_results.push_back(communicators.at(&b).size() == 0);
   QVERIFY(VarifyResult());
}

int ABTriggerLambdaNoParamTestTempVar = 0;
void InstMsgHandlerUnitTest::ABTriggerLambdaNoParamTest()
{
   ABTriggerLambdaNoParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {
   			ABTriggerLambdaNoParamTestTempVar += 1;
   		});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::A);
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {
   			ABTriggerLambdaNoParamTestTempVar += 1;
   		});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::A);
   	}
   };
   ClassA a;
   ClassB b;
   XConnect(&a, &b);
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   a.Call();
   m_results.push_back(ABTriggerLambdaNoParamTestTempVar == 1);
   b.Call();
   m_results.push_back(ABTriggerLambdaNoParamTestTempVar == 2);

   XDisconnect(&a, &b);
   a.Call();
   m_results.push_back(ABTriggerLambdaNoParamTestTempVar == 2);
   b.Call();
   m_results.push_back(ABTriggerLambdaNoParamTestTempVar == 2);
   QVERIFY(VarifyResult());
}

int ABTriggerLambdaHasParamTestTempVar = 0;
void InstMsgHandlerUnitTest::ABTriggerLambdaHasParamTest()
{
   ABTriggerLambdaHasParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {
   			ABTriggerLambdaHasParamTestTempVar = data;
   		});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::B, 100);
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {
   			ABTriggerLambdaHasParamTestTempVar = data;
   		});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::B, -100);
   	}
   };
   ClassA a;
   ClassB b;
   XConnect(&a, &b);
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   a.Call();
   m_results.push_back(ABTriggerLambdaHasParamTestTempVar == 100);
   b.Call();
   m_results.push_back(ABTriggerLambdaHasParamTestTempVar == -100);
   QVERIFY(VarifyResult());
}

int ABTriggerMemberFuncNoParamTestTempVar = 0;
void InstMsgHandlerUnitTest::ABTriggerMemberFuncNoParamTest()
{
   ABTriggerMemberFuncNoParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, &ClassA::NoParamFunc);
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	void NoParamFunc() {
   		ABTriggerMemberFuncNoParamTestTempVar += 1;
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::A);
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, &ClassB::NoParamFunc);
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	void NoParamFunc() {
   		ABTriggerMemberFuncNoParamTestTempVar += 1;
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::A);
   	}
   };
   ClassA a;
   ClassB b;
   XConnect(&a, &b);
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   a.Call();
   m_results.push_back(ABTriggerMemberFuncNoParamTestTempVar == 1);
   b.Call();
   m_results.push_back(ABTriggerMemberFuncNoParamTestTempVar == 2);

   XDisconnect(&a, &b);
   a.Call();
   m_results.push_back(ABTriggerMemberFuncNoParamTestTempVar == 2);
   b.Call();
   m_results.push_back(ABTriggerMemberFuncNoParamTestTempVar == 2);
   QVERIFY(VarifyResult());
}

int ABTriggerMemberFuncHasParamTestTempVar = 0;
void InstMsgHandlerUnitTest::ABTriggerMemberFuncHasParamTest()
{
   ABTriggerMemberFuncHasParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, &ClassA::HasParamFunc);
   	}
   	void HasParamFunc(int data) {
   		ABTriggerMemberFuncHasParamTestTempVar = data;
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::B, 100);
   	}
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, &ClassB::HasParamFunc);
   	}
   	void HasParamFunc(int data) {
   		ABTriggerMemberFuncHasParamTestTempVar = data;
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void Call() {
   		XTriggerMsg(Msg::B, -100);
   	}
   };
   ClassA a;
   ClassB b;
   XConnect(&a, &b);
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 2);

   m_results.push_back(communicators.size() == 2);
   m_results.push_back(communicators.at(&a).size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   a.Call();
   m_results.push_back(ABTriggerMemberFuncHasParamTestTempVar == 100);
   b.Call();
   m_results.push_back(ABTriggerMemberFuncHasParamTestTempVar == -100);
   QVERIFY(VarifyResult());
}

void InstMsgHandlerUnitTest::XConnectBaseTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   };
   struct ClassB : ClassA {
   	ClassB() {
   		XConnect(this);
   	}
   	~ClassB() {
   		XDeregister();
   	}
   };

   ClassB b;

   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);

   m_results.push_back(communicators.size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);
}

int BaseTriggerNoParamTestTempVar = 0;
void InstMsgHandlerUnitTest::BaseTriggerNoParamTest()
{
   BaseTriggerNoParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {
   			BaseTriggerNoParamTestTempVar += 1;
   		});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void CallBase() {
   		XTriggerMsg(Msg::A);
   	}
   };
   struct ClassB : ClassA {
   	ClassB() {
   		XConnect(this);
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void CallDriver() {
   		XTriggerMsg(Msg::A);
   	}
   };

   ClassB b;
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);

   m_results.push_back(communicators.size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   b.CallBase();
   m_results.push_back(BaseTriggerNoParamTestTempVar == 1);
   b.CallDriver();
   m_results.push_back(BaseTriggerNoParamTestTempVar == 2);
   QVERIFY(VarifyResult());
}

int BaseTriggerHasParamTestTempVar = 0;
void InstMsgHandlerUnitTest::BaseTriggerHasParamTest()
{
   BaseTriggerHasParamTestTempVar = 0;
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {
   			BaseTriggerHasParamTestTempVar = data;
   		});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	void CallBase() {
   		XTriggerMsg(Msg::B, 100);
   	}
   };
   struct ClassB : ClassA {
   	ClassB() {
   		XConnect(this);
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	void CallDriver() {
   		XTriggerMsg(Msg::B, -100);
   	}
   };

   ClassB b;
   const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   m_results.push_back(globalMsgHandler.size() == 2);
   m_results.push_back(globalMsgHandler.at("Msg::A").size() == 1);
   m_results.push_back(globalMsgHandler.at("Msg::B").size() == 1);

   m_results.push_back(communicators.size() == 1);
   m_results.push_back(communicators.at(&b).size() == 1);

   b.CallBase();
   m_results.push_back(BaseTriggerHasParamTestTempVar == 100);
   b.CallDriver();
   m_results.push_back(BaseTriggerHasParamTestTempVar == -100);
   QVERIFY(VarifyResult());
}

void InstMsgHandlerUnitTest::MemoryLeakTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	int a = 0;
   };
   struct ClassB {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	int b = 0;
   };
   auto beginMemory = GetCurMemoryKB();
   {
   	std::vector<ClassA*> veca;
   	std::vector<ClassB*> vecb;
   	for (int i = 0; i < 10000; ++i) {
   		ClassA* a = new ClassA;
   		ClassB* b = new ClassB;
   		XConnect(a, b);
   		veca.push_back(a);
   		vecb.push_back(b);
   	}

   	const auto& globalMsgHandler = InstMsgHandler::GetInstance()->globalMsgHandler_;
   	const auto& communicators = InstMsgHandler::GetInstance()->communicators_;

   	m_results.push_back(globalMsgHandler.size() == 2);
   	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 20000);
   	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 20000);
   	m_results.push_back(communicators.size() == 20000);
   	for (auto& communicator : communicators) {
   		QVERIFY(communicator.second.size() == 1);
   	}
   	for (auto& v : veca) {
   		delete v;
   	}
   	for (auto& v : vecb) {
   		delete v;
   	}
   	veca.clear();
   	vecb.clear();
   	m_results.push_back(globalMsgHandler.size() == 2);
   	m_results.push_back(globalMsgHandler.at("Msg::A").size() == 0);
   	m_results.push_back(globalMsgHandler.at("Msg::B").size() == 0);
   	m_results.push_back(communicators.size() == 0);
   }
   auto endMemory = GetCurMemoryKB();
   QVERIFY(endMemory - beginMemory < 30);
}

void InstMsgHandlerUnitTest::BaseMemoryLeakTest()
{
   struct ClassA {
   	ClassA() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   	}
   	~ClassA() {
   		XDeregister();
   	}
   	int a = 0;
   };
   struct ClassB : ClassA {
   	ClassB() {
   		XRegisterMsgHandler(Msg::A, [this] {});
   		XRegisterMsgHandler(Msg::B, [this](int data) {});
   		XConnect(this);
   	}
   	~ClassB() {
   		XDeregister();
   	}
   	int b = 0;
   };
   auto beginMemory = GetCurMemoryKB();
   {
   	std::vector<ClassB*> vecb;
   	for (int i = 0; i < 10000; ++i) {
   		vecb.push_back(new ClassB);
   	}
   	for (auto& v : vecb) {
   		delete v;
   	}
   	vecb.clear();
   }
   auto endMemory = GetCurMemoryKB();
   QVERIFY(endMemory - beginMemory < 30);
}

bool InstMsgHandlerUnitTest::VarifyResult()
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

void InstMsgHandlerUnitTest::initTestCase()
{
}

void InstMsgHandlerUnitTest::cleanupTestCase()
{
}

void InstMsgHandlerUnitTest::init()
{
   m_results.clear();
   InstMsgHandler::GetInstance()->globalMsgHandler_.clear();
   InstMsgHandler::GetInstance()->communicators_.clear();
}

void InstMsgHandlerUnitTest::cleanup()
{
}

