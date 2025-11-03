#include "SimulationControlManagerHandler.h"

/************************************************************************
	constructor / destructor
************************************************************************/
SimulationControlManagerHandler::SimulationControlManagerHandler(nframework::BaseManager* mgr, nframework::IMEBComponent* MEB) : userMgr(mgr), meb(MEB)
{
	initialize();
}

SimulationControlManagerHandler::~SimulationControlManagerHandler()
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void SimulationControlManagerHandler::initialize()
{
	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	nomMsgProc = std::bind(&SimulationControlManagerHandler::processSetSimulationMode, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SetSimulationMode"), nomMsgProc));
}

void SimulationControlManagerHandler::release()
{
	nomProcessorMap.clear();
}

void SimulationControlManagerHandler::processMessage(std::shared_ptr<nframework::NOM> nomMsg)
{
	if (auto itr = nomProcessorMap.find(nomMsg->getName()); itr != nomProcessorMap.end())
		itr->second(nomMsg);
}

/************************************************************************
	Busniess Logic
************************************************************************/
/*
* 시뮬레이션 모드 변경 명령을 저장하고 CSU로 송신하는 함수
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void SimulationControlManagerHandler::processSetSimulationMode(std::shared_ptr<nframework::NOM> _simulationMode)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _simulationMode->getName() << std::endl;
	ntcout << "Receive SetSimulationMode Info in SimulationControlManager!" << std::endl;

	//auto msgId = _simulationMode->getValue(_T("msgId"))->toUShort();
	//auto length = _simulationMode->getValue(_T("length"))->toUShort();
	//auto mode = _simulationMode->getValue(_T("mode"))->toChar();

	//ntcout << "msgId: " << msgId << std::endl;
	//ntcout << "length: " << length << std::endl;
	//ntcout << "mode: " << mode << std::endl;

	std::shared_ptr<NOM> simulationModeNOM = meb->getNOMInstance(userMgr->getUserName(), _T("SimulationMode"));
	
	// Header
	simulationModeNOM->setValue(_T("msgId"), _simulationMode->getValue(_T("msgId")));
	simulationModeNOM->setValue(_T("length"), _simulationMode->getValue(_T("length")));

	// Body
	simulationModeNOM->setValue(_T("mode"), _simulationMode->getValue(_T("mode")));

	userMgr->sendMsg(simulationModeNOM);
}
