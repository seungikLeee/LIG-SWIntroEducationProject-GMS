#include "InterceptManagerHandler.h"

/************************************************************************
	constructor / destructor
************************************************************************/
InterceptManagerHandler::InterceptManagerHandler(nframework::BaseManager* mgr, nframework::IMEBComponent* MEB) : userMgr(mgr), meb(MEB)
{
	initialize();
}

InterceptManagerHandler::~InterceptManagerHandler()
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void InterceptManagerHandler::initialize()
{
	simulationStatus = false;

	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	
	nomMsgProc = std::bind(&InterceptManagerHandler::processSendGMSCommand, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SendGMSCommand"), nomMsgProc));
	
	nomMsgProc = std::bind(&InterceptManagerHandler::processSetSimulationMode, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SimulationMode"), nomMsgProc));
	
	nomMsgProc = std::bind(&InterceptManagerHandler::processAirThreatInfo, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("AirThreatInfo"), nomMsgProc));

	nomMsgProc = std::bind(&InterceptManagerHandler::processMissileStatus, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("MissileStatus"), nomMsgProc));
	//nTimer = &(nframework::NTimer::getInstance());
	//timerHandle = 0;

	airThreatInfo = meb->getNOMInstance(userMgr->getUserName(), _T("AirThreatInfo"));
	missileInfo = meb->getNOMInstance(userMgr->getUserName(), _T("MissileStatus"));
}

void InterceptManagerHandler::release()
{
	nomProcessorMap.clear();
}

void InterceptManagerHandler::processMessage(std::shared_ptr<nframework::NOM> nomMsg)
{
	if (auto itr = nomProcessorMap.find(nomMsg->getName()); itr != nomProcessorMap.end())
		itr->second(nomMsg);
}

/************************************************************************
	Busniess Logic
************************************************************************/
/*
* 발사 명령을 수신하고 미사일 객체를 생성하는 함수
* 매개변수: 발사 명령 NOM 메세지
* 반환값:void
*/
void InterceptManagerHandler::processSendGMSCommand(std::shared_ptr<nframework::NOM> _sendGMSCommand)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _sendGMSCommand->getName() << std::endl;
	ntcout << "Receive SendGMSCommand Info in InterceptManager!" << std::endl;

	auto msgId = _sendGMSCommand->getValue(_T("msgId"))->toUShort();
	auto length = _sendGMSCommand->getValue(_T("length"))->toUShort();
	auto fire = _sendGMSCommand->getValue(_T("fire"))->toChar();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "fire: " << fire << std::endl;
}

/*
* 시뮬레이션 모드 변경 명령을 수신하고, 해당 시뮬레이션 모드를 저장하는 함수
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void InterceptManagerHandler::processSetSimulationMode(std::shared_ptr<nframework::NOM> _simulationMode)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _simulationMode->getName() << std::endl;
	ntcout << "Receive SetSimulationMode Info in SimulationControlManager!" << std::endl;

	//auto msgId = _simulationMode->getValue(_T("msgId"))->toUShort();
	//auto length = _simulationMode->getValue(_T("length"))->toUShort();
	//auto mode = _simulationMode->getValue(_T("mode"))->toChar();

	//ntcout << "msgId: " << msgId << std::endl;
	//ntcout << "length: " << length << std::endl;
	//ntcout << "mode: " << mode << std::endl;

	if (_simulationMode->getValue(_T("mode"))->toChar() == '1') { // 값 체크 필요
		ntcout << "Set SimulationStatus to true in InterceptManager!" << std::endl;
		simulationStatus = true;
	}
	else {
		ntcout << "Set SimulationStatus to false in InterceptManager!" << std::endl;
		simulationStatus = false;
	}
}

/*
* 시뮬레이션이 시작하면, 주기적으로 공중위협 정보를 갱신하는 함수
* 매개변수: 공중위협 정보 NOM 메세지
* 반환값:void
*/
void InterceptManagerHandler::processAirThreatInfo(std::shared_ptr<nframework::NOM> _airThreatInfoNOM)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _airThreatInfoNOM->getName() << std::endl;
	ntcout << "Receive AirThreat Info in InterceptManager!" << std::endl;

	//auto msgId = _simulationMode->getValue(_T("msgId"))->toUShort();
	//auto length = _simulationMode->getValue(_T("length"))->toUShort();
	//auto mode = _simulationMode->getValue(_T("mode"))->toChar();

	//ntcout << "msgId: " << msgId << std::endl;
	//ntcout << "length: " << length << std::endl;
	//ntcout << "mode: " << mode << std::endl;

	//Update AirThreatInfo
	//Header
	airThreatInfo->setValue(_T("msgId"), _airThreatInfoNOM->getValue(_T("msgId")));
	airThreatInfo->setValue(_T("length"), _airThreatInfoNOM->getValue(_T("length"))); 
	//Body
	airThreatInfo->setValue(_T("airThreatId"), _airThreatInfoNOM->getValue(_T("airThreatId")));
	airThreatInfo->setValue(_T("airThreatStatus"), _airThreatInfoNOM->getValue(_T("airThreatStatus")));  // 0: 준비, 1: 비행, 2: 폭파
	airThreatInfo->setValue(_T("airThreatX"), _airThreatInfoNOM->getValue(_T("airThreatX")));
	airThreatInfo->setValue(_T("airThreatY"), _airThreatInfoNOM->getValue(_T("airThreatY")));
	airThreatInfo->setValue(_T("airThreatZ"), _airThreatInfoNOM->getValue(_T("airThreatZ")));

	auto msgId = airThreatInfo->getValue(_T("msgId"))->toUShort();
	auto length = airThreatInfo->getValue(_T("length"))->toUShort();
	auto airThreatId = airThreatInfo->getValue(_T("airThreatId"))->toUShort();
	auto airThreatStatus = airThreatInfo->getValue(_T("airThreatStatus"))->toUShort(); // 0: 준비, 1: 비행, 2: 폭파
	auto airThreatX = airThreatInfo->getValue(_T("airThreatX"))->toDouble();
	auto airThreatY = airThreatInfo->getValue(_T("airThreatY"))->toDouble();
	auto airThreatZ = airThreatInfo->getValue(_T("airThreatZ"))->toDouble();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "airThreatId: " << airThreatId << std::endl;
	ntcout << "airThreatStatus: " << airThreatStatus << " (0: 준비, 1: 비행, 2: 폭파)" << std::endl;
	ntcout << "airThreatX: " << airThreatX << std::endl;
	ntcout << "airThreatY: " << airThreatY << std::endl;
	ntcout << "airThreatZ: " << airThreatZ << std::endl;
}

void InterceptManagerHandler::processMissileStatus(std::shared_ptr<nframework::NOM> _missileInfoNOM)
{
	//Header
	missileInfo->setValue(_T("msgId"), _missileInfoNOM->getValue(_T("msgId")));
	missileInfo->setValue(_T("length"), _missileInfoNOM->getValue(_T("length")));
	//Body
	missileInfo->setValue(_T("missileId"), _missileInfoNOM->getValue(_T("missileId")));
	missileInfo->setValue(_T("missileX"), _missileInfoNOM->getValue(_T("missileX")));
	missileInfo->setValue(_T("missileY"), _missileInfoNOM->getValue(_T("missileY")));
	missileInfo->setValue(_T("missileZ"), _missileInfoNOM->getValue(_T("missileZ")));
	missileInfo->setValue(_T("missileStatus"), _missileInfoNOM->getValue(_T("missileStatus")));

	//디버깅용 출력
	auto msgId = missileInfo->getValue(_T("msgId"))->toUShort();
	auto length = missileInfo->getValue(_T("length"))->toUShort();
	auto missileId = missileInfo->getValue(_T("missileId"))->toUShort();
	auto missileX = missileInfo->getValue(_T("missileX"))->toDouble();
	auto missileY = missileInfo->getValue(_T("missileY"))->toDouble();
	auto missileZ = missileInfo->getValue(_T("missileZ"))->toDouble();
	auto missileStatus = missileInfo->getValue(_T("missileStatus"))->toUShort();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "missileId: " << missileId << std::endl;
	ntcout << "missileX: " << missileX << std::endl;
	ntcout << "missileY: " << missileY << std::endl;
	ntcout << "missileZ: " << missileZ << std::endl;
	ntcout << "missileStatus: " << missileStatus << std::endl;
	//거리 계산 함수 따로 구현
}