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
	
	nomMsgProc = std::bind(&MissileStatusManagerHandler::processLaunchMissile, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("LaunchMissile"), nomMsgProc));

	nomMsgProc = bind(&MissileStatusManagerHandler::processLaunchedMissileStop, this, std::placeholders::_1);
	nomProcessorMap.insert(make_pair(_T("LaunchedMissileStop"), nomMsgProc));

	nTimer = &(nframework::NTimer::getInstance());
	timerHandle = 0;
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
	
	if (_simulationMode->getValue(_T("mode"))->toChar() == '0') { // 값 체크 필요
		ntcout << "Remove LaunchedMissile Update in MissileStatusManagerHandler!" << std::endl;
		processLaunchedMissileStop(_simulationMode); // Todo: 인자 값 필요 없음, 수정 필요
	}
	simulationMode = _simulationMode;
}

/*
* 발사 명령을 수신하고 미사일 객체를 생성하는 함수
* 매개변수: 발사 명령 NOM 메세지
* 반환값:void
*/
void MissileStatusManagerHandler::processLaunchMissile(std::shared_ptr<nframework::NOM> _launchMissile)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _launchMissile->getName() << std::endl;
	ntcout << "Receive LaunchMissile Info in MissileStatusManager!" << std::endl;

	auto msgId = _launchMissile->getValue(_T("msgId"))->toUShort();
	auto length = _launchMissile->getValue(_T("length"))->toUShort();
	auto fire = _launchMissile->getValue(_T("fire"))->toChar();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "fire: " << fire << std::endl;

	//STEP1: Object 메세지 등록
	missileStatusNOM = userMgr->registerMsg(_T("MissileStatus"));
	
	//STEP2: 1Hz 타이머 가동
	STDFUNCTION periodicFunc = std::bind(&MissileStatusManagerHandler::sendMissileCallback, this);
	timerHandle = nTimer->addPeriodicTask(1000, periodicFunc);
}

/*
* 시뮬레이션을 종료하고, 유도탄 객체 모의 중지하는 함수
* 매개변수: task NOM 메세지 (필요 없음)
* 반환값:void
*/
void MissileStatusManagerHandler::processLaunchedMissileStop(std::shared_ptr<nframework::NOM> task)
{
	//STEP1: 유도탄 객체 모의 중지
	nTimer->removeTask(timerHandle);
}


/*
* 유도탄 리스트의 실시간 정보를 계산하고 송신하는 함수(단일)
* 매개변수: void
* 반환값: void
*/
void MissileStatusManagerHandler::sendMissileCallback()
{
	//STEP1: 유도탄 객체 위치 및 속도 갱신

	//STEP2: 유도탄 정보 송신
	
	// Header
	missileStatusNOM->setValue(_T("msgId"), &NUShort(3401));
	missileStatusNOM->setValue(_T("length"), &NUShort(17));  // 메시지 전체 길이 예시: 2+2+2+8+8+8+1=31byte 등 실제 명세에 맞춰 수정

	// Body
	missileStatusNOM->setValue(_T("missileId"), &NUShort(1));          // 유도탄 ID
	missileStatusNOM->setValue(_T("missileX"), &NDouble(1000.0));     // X 좌표
	missileStatusNOM->setValue(_T("missileY"), &NDouble(500.0));      // Y 좌표
	missileStatusNOM->setValue(_T("missileZ"), &NDouble(50.0));       // Z 좌표
	missileStatusNOM->setValue(_T("missileStatus"), &NCharacter(1));  // 상태: 0=준비, 1=비행, 2=폭파

	ntcout << _T("Send MissileStatus Update in MissileStatusManager!") << std::endl;
	userMgr->updateMsg(missileStatusNOM);
}