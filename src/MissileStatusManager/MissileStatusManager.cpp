#pragma once
#include <nFramework/util/IniHandler.h>
#include "MissileStatusManager.h"
#include <map>

/**
* @ class: MissileStatusManager
* @ author : 하재희
* @ version: 1.0
* @ see also: MissileStatusHandler
* @ description: MEC 객체를 사용하기 위해 BaseManager 인터페이스를 사용받아 구현한 클래스
* @ date: 2023.11.4
**/



/************************************************************************
	constructor / destructor
************************************************************************/
MissileStatusManager::MissileStatusManager(void)
{
	initialize();
}

MissileStatusManager::~MissileStatusManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
MissileStatusManager::initialize(void)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	setUserName(_T("MissileStatusManager"));

	// design by contract
	mec = std::make_unique<MECComponent>();
	mec->setUser(this);
}

void
MissileStatusManager::release()
{
	/*delete mec;
	mec = nullptr;*/
	meb = nullptr;
}

/************************************************************************
	inherited functions
************************************************************************/
std::shared_ptr<NOM>
MissileStatusManager::registerMsg(tstring msgName)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << msgName << std::endl;
	
	
	std::shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

	return nomMsg;
}

void
MissileStatusManager::discoverMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);
	missileStatusManagerHandler->processMessage(nomMsg);
}

void
MissileStatusManager::updateMsg(std::shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
MissileStatusManager::reflectMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	missileStatusManagerHandler->processMessage(nomMsg);
	
}

void
MissileStatusManager::deleteMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->deleteMsg(nomMsg);
	registeredMsgMap.erase(nomMsg->getInstanceID());
}

void
MissileStatusManager::removeMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.erase(nomMsg->getInstanceID());

}

void
MissileStatusManager::sendMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->sendMsg(nomMsg);
}

void
MissileStatusManager::recvMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	missileStatusManagerHandler->processMessage(nomMsg);
	
}


void
MissileStatusManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
MissileStatusManager::getUserName()
{
	return name;
}

void
MissileStatusManager::setData(void* data)
{

}

bool
MissileStatusManager::start()
{
	//IniHandler iniHandler;
	//iniHandler.readIni(_T("MissileStatusManager/MissileStatusManager.ini")); // ※주의 작업디렉터리: Main.exe가 있는 경로

	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	missileStatusManagerHandler = std::make_shared<MissileStatusManagerHandler>(this, meb);

	return true;
}

bool
MissileStatusManager::stop()
{
	bool result = true;

	return result;
}

void
MissileStatusManager::setMEBComponent(IMEBComponent* realMEB)
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
	return new MissileStatusManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}
