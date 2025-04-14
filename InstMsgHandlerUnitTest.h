#include <QTest>
#include <vector>

class InstMsgHandlerUnitTest :public QObject {
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();

	void RegisterTest();
	void DeregisterTest();

	void XConnectABTest();
	void XDisconnectABTest();
	void ABTriggerLambdaNoParamTest();
	void ABTriggerLambdaHasParamTest();
	void ABTriggerMemberFuncNoParamTest();
	void ABTriggerMemberFuncHasParamTest();

	void XConnectBaseTest();
	void BaseTriggerNoParamTest();
	void BaseTriggerHasParamTest();

	void MemoryLeakTest();
	void BaseMemoryLeakTest();
private:
	bool VarifyResult();
private:
	std::vector<bool> m_results;
};
