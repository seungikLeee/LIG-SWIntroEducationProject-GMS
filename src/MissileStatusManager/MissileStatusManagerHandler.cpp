#include "MissileStatusManagerHandler.h"

/************************************************************************
	constructor / destructor
************************************************************************/
MissileStatusManagerHandler::MissileStatusManagerHandler(nframework::BaseManager* mgr, nframework::IMEBComponent* MEB) : userMgr(mgr), meb(MEB)
{
	initialize();
}

MissileStatusManagerHandler::~MissileStatusManagerHandler()
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void MissileStatusManagerHandler::initialize()
{
	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	nomMsgProc = std::bind(&MissileStatusManagerHandler::processSetSimulationMode, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SimulationMode"), nomMsgProc));
}

void MissileStatusManagerHandler::release()
{
	nomProcessorMap.clear();
}

void MissileStatusManagerHandler::processMessage(std::shared_ptr<nframework::NOM> nomMsg)
{
	if (auto itr = nomProcessorMap.find(nomMsg->getName()); itr != nomProcessorMap.end())
		itr->second(nomMsg);
}

/************************************************************************
	Busniess Logic
************************************************************************/
/*
* 시뮬레이션 모드를 갱신하는 함수
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void MissileStatusManagerHandler::processSetSimulationMode(std::shared_ptr<nframework::NOM> _simulationMode)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _simulationMode->getName() << std::endl;
	ntcout << "Receive SimulationMode Info in MissileStatusManager!" << std::endl;

	auto msgId = _simulationMode->getValue(_T("msgId"))->toUShort();
	auto length = _simulationMode->getValue(_T("length"))->toUShort();
	auto mode = _simulationMode->getValue(_T("mode"))->toChar();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "mode: " << mode << std::endl;
	
	simulationMode = _simulationMode;
}

/*
* 발사 명령을 수신하고 미사일 객체를 생성하는 함수
* 매개변수: 발사 명령 NOM 메세지
* 반환값:void
*/
//To do