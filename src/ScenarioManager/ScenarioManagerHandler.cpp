#include "ScenarioManagerHandler.h"

/************************************************************************
	constructor / destructor
************************************************************************/
ScenarioManagerHandler::ScenarioManagerHandler(nframework::BaseManager* mgr, nframework::IMEBComponent* MEB) : userMgr(mgr), meb(MEB)
{
	initialize();
}

ScenarioManagerHandler::~ScenarioManagerHandler()
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void ScenarioManagerHandler::initialize()
{
	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	nomMsgProc = std::bind(&ScenarioManagerHandler::processSetScenario, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SetScenario"), nomMsgProc));
}

void ScenarioManagerHandler::release()
{
	nomProcessorMap.clear();
}

void ScenarioManagerHandler::processMessage(std::shared_ptr<nframework::NOM> nomMsg)
{
	if (auto itr = nomProcessorMap.find(nomMsg->getName()); itr != nomProcessorMap.end())
		itr->second(nomMsg);
}

/************************************************************************
	Busniess Logic
************************************************************************/
/*
* 시나리오를 저장하고 공중위협 통제 CSU로 송신하는 함수
* 매개변수: 시나리오 NOM 메세지
* 반환값:void
*/
void ScenarioManagerHandler::processSetScenario(std::shared_ptr<nframework::NOM> _scenario)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _scenario->getName() << std::endl;
	ntcout << "recv Scenario Info in ScenarioManager!" << std::endl;

	//STEP1: 시나리오 저장
	//scenario = _scenario;

	//STEP2: 공중위협 초기화 요청 송신
	/*std::shared_ptr<nframework::NOM> reqAirThreatInitNOM = meb->getNOMInstance(userMgr->getUserName(), _T("ReqAirThreatInit"));
	reqAirThreatInitNOM->setValue(_T("MessageID"), _scenario->getValue(_T("MessageID")));
	reqAirThreatInitNOM->setValue(_T("ScenarioID"), _scenario->getValue(_T("ScenarioID")));
	reqAirThreatInitNOM->setValue(_T("ScenarioName"), _scenario->getValue(_T("ScenarioName")));
	reqAirThreatInitNOM->setDataTypeObjectByCopying(_T("AirThreatList"), _scenario->getDataTypeObject(_T("AirThreatList")));
	reqAirThreatInitNOM->setDataTypeObjectByCopying(_T("Battery"), _scenario->getDataTypeObject(_T("Battery")));
	reqAirThreatInitNOM->setDataTypeObjectByCopying(_T("RouteList"), _scenario->getDataTypeObject(_T("RouteList")));
	userMgr->sendMsg(reqAirThreatInitNOM);*/
}
