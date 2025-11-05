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
	scenarioDeployStatus = false;
	simulationStatus = false;

	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	
	nomMsgProc = std::bind(&SimulationControlManagerHandler::processSetSimulationMode, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SetSimulationMode"), nomMsgProc));
	
	nomMsgProc = std::bind(&SimulationControlManagerHandler::processSendGMSCommand, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SendGMSCommand"), nomMsgProc));

	//내부 메시지
	nomMsgProc = std::bind(&SimulationControlManagerHandler::processSetScenarioDeployStatus, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("ScenarioDeployStatus"), nomMsgProc));

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
* 시뮬레이션 모드 변경 명령을 수신하고, 해당 시뮬레이션 모드를 저장 및 CSU로 송신하는 함수
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void SimulationControlManagerHandler::processSetSimulationMode(std::shared_ptr<nframework::NOM> _simulationMode)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _simulationMode->getName() << std::endl;
	ntcout << "Receive SetSimulationMode Info in SimulationControlManager!" << std::endl;

	if (scenarioDeployStatus != true) { // 현재 시나리오 배포 여부 판단
		ntcerr << _T("[") << _T(__FUNCTION__) << _T("] ") << "scenario is not deployed." << std::endl;
		return;
	}

	std::shared_ptr<NOM> simulationModeNOM = meb->getNOMInstance(userMgr->getUserName(), _T("SimulationMode"));
	
	// Header
	simulationModeNOM->setValue(_T("msgId"), _simulationMode->getValue(_T("msgId")));
	simulationModeNOM->setValue(_T("length"), _simulationMode->getValue(_T("length")));

	// Body
	simulationModeNOM->setValue(_T("mode"), _simulationMode->getValue(_T("mode")));

	if (simulationModeNOM->getValue(_T("mode"))->toChar() == '1') { // 값 체크 필요
		ntcout << "Set SimulationStatus to true in SimulationControlManager!" << std::endl;
		simulationStatus = true;
	}
	else {
		ntcout << "Set SimulationStatus to false in SimulationControlManager!" << std::endl;
		simulationStatus = false;
	}
	
	userMgr->sendMsg(simulationModeNOM);
}

/*
* 발사 명령을 수신하고, MissileStatusManager CSU로 송신하는 함수
* 매개변수: 발사 명령 NOM 메세지
* 반환값:void
*/
void SimulationControlManagerHandler::processSendGMSCommand(std::shared_ptr<nframework::NOM> _sendGMSCommand)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _sendGMSCommand->getName() << std::endl;
	ntcout << "Receive SendGMSCommand Info in SimulationControlManager!" << std::endl;

	if (simulationStatus != true) { // 모의 진행 여부 판단 
		ntcerr << _T("[") << _T(__FUNCTION__) << _T("] ") << "simulation is not started." << std::endl;
		return;
	}

	std::shared_ptr<NOM> launchMissileNOM = meb->getNOMInstance(userMgr->getUserName(), _T("LaunchMissile"));

	// Header
	launchMissileNOM->setValue(_T("msgId"), _sendGMSCommand->getValue(_T("msgId")));
	launchMissileNOM->setValue(_T("length"), _sendGMSCommand->getValue(_T("length")));

	// Body
	launchMissileNOM->setValue(_T("fire"), _sendGMSCommand->getValue(_T("fire")));

	userMgr->sendMsg(launchMissileNOM);
}

/*
* 시나리오 로드 여부를 수신하고, 해당 정보를 저장하는 함수
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void SimulationControlManagerHandler::processSetScenarioDeployStatus(std::shared_ptr<nframework::NOM> _scenarioDeployStatus)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _scenarioDeployStatus->getName() << std::endl;
	ntcout << "Receive ScenarioDeployStatus in SimulationControlManager!" << std::endl;
	
	if (_scenarioDeployStatus->getValue(_T("status"))-> toChar() != '1') { // 모의 진행 여부 판단 
		ntcerr << _T("[") << _T(__FUNCTION__) << _T("] ") << "scenario is not deployed in SimulationControlManagerHandler!" << std::endl;
		return;
	}
	//시나리오 배포 여부 변경
	scenarioDeployStatus = true;
	ntcout << "Update ScenarioDeployStatus to true in SimulationControlManager!" << std::endl;
}
