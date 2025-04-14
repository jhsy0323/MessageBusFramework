#include <QTest>
#include <vector>

class MsgHandlerUnitTest :public QObject {
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();
	void RegisterTest();
	void DeregisterTest();
	void PriorityPreTest();
	void PriorityTest();
	void TriggerLambdaNoParamTest();
	void TriggerLambdaHasParamTest();
	void TriggerMemberFuncNoParamTest();
	void TriggerMemberFuncHasParamTest();
	void AsyncTriggerTest();
	void MainTriggerTest();
	void MemoryLeakTest();
private:
	bool VarifyResult();
private:
	std::vector<bool> m_results;
};

