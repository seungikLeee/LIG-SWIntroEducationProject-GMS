#pragma once
#include <nFramework/util/IniHandler.h>
#include "InterceptManager.h"
#include <map>

/**
* @ class: InterceptManager
* @ author : 하재희
* @ version: 1.0
* @ see also: InterceptHandler
* @ description: MEC 객체를 사용하기 위해 BaseManager 인터페이스를 사용받아 구현한 클래스
* @ date: 2023.11.4
**/



/************************************************************************
	constructor / destructor
************************************************************************/
InterceptManager::InterceptManager(void)
{
	initialize();
}

InterceptManager::~InterceptManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
InterceptManager::initialize(void)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	setUserName(_T("InterceptManager"));

	// design by contract
	mec = std::make_unique<MECComponent>();
	mec->setUser(this);
}

void
InterceptManager::release()
{
	/*delete mec;
	mec = nullptr;*/
	meb = nullptr;
}

/************************************************************************
	inherited functions
************************************************************************/
std::shared_ptr<NOM>
InterceptManager::registerMsg(tstring msgName)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << msgName << std::endl;
	
	
	std::shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

	return nomMsg;
}

void
InterceptManager::discoverMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);
	
	interceptManagerHandler->processMessage(nomMsg);
}

void
InterceptManager::updateMsg(std::shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
InterceptManager::reflectMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	interceptManagerHandler->processMessage(nomMsg);
}

void
InterceptManager::deleteMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->deleteMsg(nomMsg);
	registeredMsgMap.erase(nomMsg->getInstanceID());
}

void
InterceptManager::removeMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.erase(nomMsg->getInstanceID());

}

void
InterceptManager::sendMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->sendMsg(nomMsg);
}

void
InterceptManager::recvMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	interceptManagerHandler->processMessage(nomMsg);
}


void
InterceptManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
InterceptManager::getUserName()
{
	return name;
}

void
InterceptManager::setData(void* data)
{

}

bool
InterceptManager::start()
{
	//IniHandler iniHandler;
	//iniHandler.readIni(_T("InterceptManager/InterceptManager.ini")); // ※주의 작업디렉터리: Main.exe가 있는 경로

	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	interceptManagerHandler = std::make_shared<InterceptManagerHandler>(this, meb);

	return true;
}

bool
InterceptManager::stop()
{
	bool result = true;

	return result;
}

void
InterceptManager::setMEBComponent(IMEBComponent* realMEB)
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
	return new InterceptManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}
