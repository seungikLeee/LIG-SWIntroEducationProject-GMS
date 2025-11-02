#pragma once
#include <nFramework/util/IniHandler.h>
#include "ScenarioManager.h"
#include <map>

/**
* @ class: ScenarioManager
* @ author : 하재희
* @ version: 1.0
* @ see also: ScenarioHandler
* @ description: MEC 객체를 사용하기 위해 BaseManager 인터페이스를 사용받아 구현한 클래스
*
* @ date: 2023.11.4
*
**/



/************************************************************************
	constructor / destructor
************************************************************************/
ScenarioManager::ScenarioManager(void)
{
	initialize();
}

ScenarioManager::~ScenarioManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
ScenarioManager::initialize(void)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	
	setUserName(_T("ScenarioManager"));

	// design by contract
	mec = std::make_unique<MECComponent>();
	mec->setUser(this);
}

void
ScenarioManager::release()
{
	/*delete mec;
	mec = nullptr;*/
	meb = nullptr;
}

/************************************************************************
	inherited functions
************************************************************************/
std::shared_ptr<NOM>
ScenarioManager::registerMsg(tstring msgName)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << msgName << std::endl;
	
	
	std::shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

	return nomMsg;
}

void
ScenarioManager::discoverMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

}

void
ScenarioManager::updateMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->updateMsg(nomMsg);
}

void
ScenarioManager::reflectMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
}

void
ScenarioManager::deleteMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->deleteMsg(nomMsg);
	registeredMsgMap.erase(nomMsg->getInstanceID());
}

void
ScenarioManager::removeMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.erase(nomMsg->getInstanceID());

}

void
ScenarioManager::sendMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->sendMsg(nomMsg);
}

void
ScenarioManager::recvMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	scenarioManagerHandler->processMessage(nomMsg);
	
}


void
ScenarioManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
ScenarioManager::getUserName()
{
	return name;
}

void
ScenarioManager::setData(void* data)
{

}

bool
ScenarioManager::start()
{
	//IniHandler iniHandler;
	//iniHandler.readIni(_T("ScenarioManager/ScenarioManager.ini")); // ※주의 작업디렉터리: Main.exe가 있는 경로

	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	scenarioManagerHandler = std::make_shared<ScenarioManagerHandler>(this, meb);

	return true;
}

bool
ScenarioManager::stop()
{
	bool result = true;

	return result;
}

void
ScenarioManager::setMEBComponent(IMEBComponent* realMEB)
{
	meb = realMEB;
	mec->setMEB(meb);
}

/************************************************************************
	Export Function
************************************************************************/
extern "C" BASEMGRDLL_API
BaseManager * createObject()
{
	return new ScenarioManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}
