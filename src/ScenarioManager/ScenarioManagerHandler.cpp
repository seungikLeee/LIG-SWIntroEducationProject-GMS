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
* 시나리오를 저장하고 CSU로 송신하는 함수
* 매개변수: 시나리오 NOM 메세지
* 반환값:void
*/
void ScenarioManagerHandler::processSetScenario(std::shared_ptr<nframework::NOM> _scenario)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _scenario->getName() << std::endl;
	ntcout << "recv Scenario Info in ScenarioManager!" << std::endl;

	auto msgId = _scenario->getValue(_T("msgId"))->toUShort();
	auto length = _scenario->getValue(_T("length"))->toUShort();

	// Radar
	auto radarX = _scenario->getValue(_T("radarX"))->toDouble();
	auto radarY = _scenario->getValue(_T("radarY"))->toDouble();
	auto radarZ = _scenario->getValue(_T("radarZ"))->toDouble();
	auto radarMode = _scenario->getValue(_T("radarMode"))->toChar();

	// Launcher
	auto launcherX = _scenario->getValue(_T("launcherX"))->toDouble();
	auto launcherY = _scenario->getValue(_T("launcherY"))->toDouble();
	auto launcherZ = _scenario->getValue(_T("launcherZ"))->toDouble();

	// AirThreat
	auto airThreatId = _scenario->getValue(_T("airThreatId"))->toUShort();
	auto airThreatInitX = _scenario->getValue(_T("airThreatInitX"))->toDouble();
	auto airThreatInitY = _scenario->getValue(_T("airThreatInitY"))->toDouble();
	auto airThreatInitZ = _scenario->getValue(_T("airThreatInitZ"))->toDouble();
	auto airThreatSpeed = _scenario->getValue(_T("airThreatSpeed"))->toDouble();
	auto airThreatDirectionX = _scenario->getValue(_T("airThreatDirectionX"))->toDouble();
	auto airThreatDirectionY = _scenario->getValue(_T("airThreatDirectionY"))->toDouble();
	auto airThreatDirectionZ = _scenario->getValue(_T("airThreatDirectionZ"))->toDouble();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;

	ntcout << "Radar X: " << radarX << std::endl;
	ntcout << "Radar Y: " << radarY << std::endl;
	ntcout << "Radar Z: " << radarZ << std::endl;
	ntcout << "Radar Mode: " << radarMode << std::endl;

	ntcout << "Launcher X: " << launcherX << std::endl;
	ntcout << "Launcher Y: " << launcherY << std::endl;
	ntcout << "Launcher Z: " << launcherZ << std::endl;

	ntcout << "AirThreat ID: " << airThreatId << std::endl;
	ntcout << "AirThreat Init X: " << airThreatInitX << std::endl;
	ntcout << "AirThreat Init Y: " << airThreatInitY << std::endl;
	ntcout << "AirThreat Init Z: " << airThreatInitZ << std::endl;
	ntcout << "AirThreat Speed: " << airThreatSpeed << std::endl;
	ntcout << "AirThreat Dir X: " << airThreatDirectionX << std::endl;
	ntcout << "AirThreat Dir Y: " << airThreatDirectionY << std::endl;
	ntcout << "AirThreat Dir Z: " << airThreatDirectionZ << std::endl;
	//STEP1: 시나리오 저장
	scenario = _scenario;

	//STEP2: 시나리오 배포 상태 송신
	std::shared_ptr<NOM> scenarioDeployStatusNOM = meb->getNOMInstance(userMgr->getUserName(), _T("ScenarioDeployStatus"));

	// Header
	scenarioDeployStatusNOM->setValue(_T("msgId"), &NUShort(3303));
	scenarioDeployStatusNOM->setValue(_T("length"), &NUShort(3));  // 예: 총 메시지 길이 (필요 시 조정)

	// Body
	scenarioDeployStatusNOM->setValue(_T("status"), &NCharacter('1')); // 1 = 시작, 0 = 종료 등의 의미로 사용 가능

	ntcout << _T("Send Scenario Deploy Status in ScenarioManager!") << std::endl;
	userMgr->sendMsg(scenarioDeployStatusNOM);

	//STEP3: 발사대 초기 위치 정보만 추출
	std::shared_ptr<NOM> launcherPositionNOM = meb->getNOMInstance(userMgr->getUserName(), _T("LauncherPosition"));

	//Header
	launcherPositionNOM->setValue(_T("msgId"), &NUShort(3304));
	launcherPositionNOM->setValue(_T("length"), &NUShort(3));  // 예: 총 메시지 길이 (필요 시 조정)

	//Body
	launcherPositionNOM->setValue(_T("launcherX"), _scenario->getValue(_T("launcherX")));
	launcherPositionNOM->setValue(_T("launcherY"), _scenario->getValue(_T("launcherY")));
	launcherPositionNOM->setValue(_T("launcherZ"), _scenario->getValue(_T("launcherZ")));
	userMgr->sendMsg(launcherPositionNOM);
}
