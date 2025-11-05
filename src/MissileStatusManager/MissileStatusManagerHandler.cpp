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

	nomMsgProc = std::bind(&MissileStatusManagerHandler::processAirThreatInfo, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("AirThreatInfo"), nomMsgProc));

	//내부 메시지
	nomMsgProc = std::bind(&MissileStatusManagerHandler::processSimulationMode, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SimulationMode"), nomMsgProc));

	nomMsgProc = std::bind(&MissileStatusManagerHandler::processLauncherPosition, this, std::placeholders::_1);
	nomProcessorMap.insert(make_pair(_T("LauncherPosition"), nomMsgProc));

	nomMsgProc = std::bind(&MissileStatusManagerHandler::processLaunchMissile, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("LaunchMissile"), nomMsgProc));

	airThreatInfo = meb->getNOMInstance(userMgr->getUserName(), _T("AirThreatInfo"));
	launcherPosition = meb->getNOMInstance(userMgr->getUserName(), _T("LauncherPosition"));

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
* 시뮬레이션 모드를 갱신하는 함수, 모의 종료 명령일 때, 유도탄 비행 모의를 진행 중이라면 비행 모의 종료
* 매개변수: 시뮬레이션 모드 NOM 메세지
* 반환값:void
*/
void MissileStatusManagerHandler::processSimulationMode(std::shared_ptr<nframework::NOM> _simulationMode)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _simulationMode->getName() << std::endl;
	ntcout << "Receive SimulationMode Info in MissileStatusManager!" << std::endl;

	auto msgId = _simulationMode->getValue(_T("msgId"))->toUShort();
	auto length = _simulationMode->getValue(_T("length"))->toUShort();
	auto mode = _simulationMode->getValue(_T("mode"))->toChar();
	
	simulationMode = _simulationMode;

	if (_simulationMode->getValue(_T("mode"))->toChar() == '0') { // 값 체크 필요
		ntcout << "Remove LaunchedMissile Update in MissileStatusManagerHandler!" << std::endl;
		processInterceptFail();
	}
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

	// Header
	missileStatusNOM->setValue(_T("msgId"), &NUShort(3305));
	missileStatusNOM->setValue(_T("length"), &NUShort(17));  // 메시지 전체 길이 예시: 2+2+2+8+8+8+1=31byte 등 실제 명세에 맞춰 수정

	// Body
	missileStatusNOM->setValue(_T("missileId"), &NUShort(1)); // 유도탄 ID, 수정 필요
	missileStatusNOM->setValue(_T("missileX"), launcherPosition->getValue(_T("launcherX"))); // 발사대 X 좌표
	missileStatusNOM->setValue(_T("missileY"), launcherPosition->getValue(_T("launcherY"))); // 발사대 Y 좌표
	missileStatusNOM->setValue(_T("missileZ"), launcherPosition->getValue(_T("launcherZ"))); // 발사대 Z 좌표
	missileStatusNOM->setValue(_T("missileStatus"), &NCharacter('1'));  // 상태: 0=준비, 1=비행, 2=요격 성공, 3=요격 실패
	
	//STEP2: 1Hz 타이머 가동
	STDFUNCTION periodicFunc = std::bind(&MissileStatusManagerHandler::sendMissileCallback, this);
	timerHandle = nTimer->addPeriodicTask(40, periodicFunc);
}

/*
* 요격 성공 시, 후 처리 함수
* 매개변수: void
* 반환값: void
*/
void MissileStatusManagerHandler::processInterceptSuccess()
{
	// STEP1: 유도탄 비행 모의 중지
	if (timerHandle != 0) {
		try {
			nTimer->removeTask(timerHandle);
		}
		catch (const std::exception& e) {
			std::cerr << "[Warning] Failed to remove timer task in MissileStatusManagerHandler: " << e.what() << std::endl;
		}
		timerHandle = 0; // 안전하게 초기화

		// 유도탄 객체 상태 변경
		missileStatusNOM->setValue(_T("missileStatus"), &NCharacter('2')); // 요격 성공

		// 변경 상태 update
		ntcout << _T("Send MissileStatus(InterceptSuccess) in MissileStatusManager!") << std::endl;
		userMgr->updateMsg(missileStatusNOM);
	}
}

/*
* 요격 실패 시, 후 처리 함수
* 매개변수: void
* 반환값: void
*/
void MissileStatusManagerHandler::processInterceptFail()
{
	// 유도탄 비행 모의 중지
	if (timerHandle != 0) {
		try {
			nTimer->removeTask(timerHandle);
		}
		catch (const std::exception& e) {
			std::cerr << "[Warning] Failed to remove timer task: " << e.what() << std::endl;
		}
		timerHandle = 0; // 안전하게 초기화

		// 유도탄 객체 상태 변경
		missileStatusNOM->setValue(_T("missileStatus"), &NCharacter('3')); // 요격 실패

		// 변경 상태 update
		ntcout << _T("Send MissileStatus(InterceptFail) in MissileStatusManager!") << std::endl;
		userMgr->updateMsg(missileStatusNOM);
	}
}

/*
* 시나리오 배포 후, 발사대 위치 설정하는 함수
* 매개변수: 발사대 위치 정보
* 반환값: void
*/
void MissileStatusManagerHandler::processLauncherPosition(std::shared_ptr<nframework::NOM> _launcherPosition)
{
	launcherPosition->setValue(_T("msgId"), _launcherPosition->getValue(_T("msgId")));
	launcherPosition->setValue(_T("length"), _launcherPosition->getValue(_T("length")));
	launcherPosition->setValue(_T("launcherX"), _launcherPosition->getValue(_T("launcherX")));
	launcherPosition->setValue(_T("launcherY"), _launcherPosition->getValue(_T("launcherY")));
	launcherPosition->setValue(_T("launcherZ"), _launcherPosition->getValue(_T("launcherZ")));

	ntcout << _T("Receive LauncherPosition in MissileStatusManager!") << std::endl;
}

/*
* 공중 위협 위치를 갱신하는 함수
* 매개변수: 공중 위협 정보 NOM
* 반환값: void
*/
void MissileStatusManagerHandler::processAirThreatInfo(std::shared_ptr<nframework::NOM> _airThreatInfoNOM)
{
	//Header
	airThreatInfo->setValue(_T("msgId"), _airThreatInfoNOM->getValue(_T("msgId")));
	airThreatInfo->setValue(_T("length"), _airThreatInfoNOM->getValue(_T("length")));
	//Body
	airThreatInfo->setValue(_T("airThreatId"), _airThreatInfoNOM->getValue(_T("airThreatId")));
	airThreatInfo->setValue(_T("airThreatStatus"), _airThreatInfoNOM->getValue(_T("airThreatStatus")));  // 0: 준비, 1: 비행, 2: 폭파
	airThreatInfo->setValue(_T("airThreatX"), _airThreatInfoNOM->getValue(_T("airThreatX")));
	airThreatInfo->setValue(_T("airThreatY"), _airThreatInfoNOM->getValue(_T("airThreatY")));
	airThreatInfo->setValue(_T("airThreatZ"), _airThreatInfoNOM->getValue(_T("airThreatZ")));

	ntcout << _T("Receive AirthreatInfo in MissileStatusManager!") << std::endl;
}

/*
* 유도탄 리스트의 실시간 정보를 계산하고 송신하는 함수(단일)
* 매개변수: void
* 반환값: void
*/
void MissileStatusManagerHandler::sendMissileCallback()
{
	moveMissileTowardTarget(1);
}

/*
* 목표 공중 위협으로 유도탄 위치를 갱신하는 함수
* 매개변수: 유도탄 속도
* 반환값: void
*/
void MissileStatusManagerHandler::moveMissileTowardTarget(double missileSpeed)
{
	if (airThreatInfo->getValue(_T("airThreatStatus"))->toUShort() == 2) { // 공중위협 폭파 시, 유도탄도 폭파 처리
		ntcout << _T("AirThreat is already detonated!") << std::endl;
		processInterceptFail();
		return;
	}
		
	// 현재 좌표
	double AT_x = airThreatInfo->getValue(_T("airThreatX"))->toDouble();
	double AT_y = airThreatInfo->getValue(_T("airThreatY"))->toDouble();
	double AT_z = airThreatInfo->getValue(_T("airThreatZ"))->toDouble();

	double GM_x = missileStatusNOM->getValue(_T("missileX"))->toDouble();
	double GM_y = missileStatusNOM->getValue(_T("missileY"))->toDouble();
	double GM_z = missileStatusNOM->getValue(_T("missileZ"))->toDouble();
	
	ntcout << _T("airThreat position") << std::endl;
	ntcout << airThreatInfo->getValue(_T("airThreatX"))->toDouble() << std::endl;
	ntcout << airThreatInfo->getValue(_T("airThreatY"))->toDouble() << std::endl;
	ntcout << airThreatInfo->getValue(_T("airThreatZ"))->toDouble() << std::endl;

	ntcout << _T("Before update missile position") << std::endl;
	ntcout << missileStatusNOM->getValue(_T("missileX"))->toDouble() << std::endl;
	ntcout << missileStatusNOM->getValue(_T("missileY"))->toDouble() << std::endl;
	ntcout << missileStatusNOM->getValue(_T("missileZ"))->toDouble() << std::endl;

	double dx = AT_x - GM_x;
	double dy = AT_y - GM_y;
	double dz = AT_z - GM_z;

	double dist = std::sqrt(dx * dx + dy * dy + dz * dz);

	if (dist < 0.3) { // 임계 거리 이내일 때, 요격 성공 판단
		ntcout << _T("Intercept decision in MissileStatusManager!") << std::endl;
		processInterceptSuccess();
		sendInterceptResult(missileStatusNOM);
		return;
	}
	// 단위 벡터
	double ux = dx / dist;
	double uy = dy / dist;
	double uz = dz / dist;

	// 이동 거리 = min(속도, 남은 거리)
	double moveDist = missileSpeed;
	moveDist = moveDist * 0.3; // 주기에 따라 변환
	GM_x += ux * moveDist;
	GM_y += uy * moveDist;
	GM_z += uz * moveDist;

	// NOM 갱신
	missileStatusNOM->setValue(_T("missileX"), &NDouble(GM_x));
	missileStatusNOM->setValue(_T("missileY"), &NDouble(GM_y));
	missileStatusNOM->setValue(_T("missileZ"), &NDouble(GM_z));

	ntcout << _T("After update missile position") << std::endl;

	ntcout << missileStatusNOM->getValue(_T("missileX"))->toDouble() << std::endl;
	ntcout << missileStatusNOM->getValue(_T("missileY"))->toDouble() << std::endl;
	ntcout << missileStatusNOM->getValue(_T("missileZ"))->toDouble() << std::endl;

	ntcout << _T("Send Updated MissileStatus in MissileStatusManager!") << std::endl;
	userMgr->updateMsg(missileStatusNOM);
}

/*
* 요격 결과 정보를 송신하는 함수
* 매개변수: 유도탄 정보 NOM
* 반환값: void
*/
void MissileStatusManagerHandler::sendInterceptResult(std::shared_ptr<nframework::NOM> _missileInfoNOM)
{
	std::shared_ptr<NOM> interceptResultNOM = meb->getNOMInstance(userMgr->getUserName(), _T("InterceptResult"));

	// Header
	interceptResultNOM->setValue(_T("msgId"), &NUShort(3002)); // 수정 필요
	interceptResultNOM->setValue(_T("length"), &NUShort(8));

	// Body
	interceptResultNOM->setValue(_T("airThreatId"), airThreatInfo->getValue(_T("airThreatId")));
	interceptResultNOM->setValue(_T("missileId"), _missileInfoNOM->getValue(_T("missileId")));

	userMgr->sendMsg(interceptResultNOM);
}