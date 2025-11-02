#pragma once
#include <nFramework/util/IniHandler.h>
#include "SimulationControlManager.h"
#include <map>

/**
* @ class: SimulationControlManager
* @ author : 하재희
* @ version: 1.0
* @ see also: SimulationControlHandler
* @ description: MEC 객체를 사용하기 위해 BaseManager 인터페이스를 사용받아 구현한 클래스
*
* @ date: 2023.11.4
*
**/



/************************************************************************
	constructor / destructor
************************************************************************/
SimulationControlManager::SimulationControlManager(void)
{
	initialize();
}

SimulationControlManager::~SimulationControlManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
SimulationControlManager::initialize(void)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	
	setUserName(_T("SimulationControlManager"));

	// design by contract
	mec = new MECComponent;
	mec->setUser(this);
}

void
SimulationControlManager::release()
{
	delete mec;
	mec = nullptr;
	meb = nullptr;
}

/************************************************************************
	inherited functions
************************************************************************/
std::shared_ptr<NOM>
SimulationControlManager::registerMsg(tstring msgName)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << msgName << std::endl;
	
	
	std::shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

	return nomMsg;
}

void
SimulationControlManager::discoverMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.emplace(nomMsg->getInstanceID(), nomMsg);

}

void
SimulationControlManager::updateMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->updateMsg(nomMsg);
}

void
SimulationControlManager::reflectMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
}

void
SimulationControlManager::deleteMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->deleteMsg(nomMsg);
	registeredMsgMap.erase(nomMsg->getInstanceID());
}

void
SimulationControlManager::removeMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	discoveredMsgMap.erase(nomMsg->getInstanceID());

}

void
SimulationControlManager::sendMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
	mec->sendMsg(nomMsg);
}

void
SimulationControlManager::recvMsg(std::shared_ptr<NOM> nomMsg)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << nomMsg->getName() << std::endl;
	
	
}


void
SimulationControlManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
SimulationControlManager::getUserName()
{
	return name;
}

void
SimulationControlManager::setData(void* data)
{

}

bool
SimulationControlManager::start()
{
	IniHandler iniHandler;
	iniHandler.readIni(_T("SimulationControlManager/SimulationControlManager.ini")); // ※주의 작업디렉터리: Main.exe가 있는 경로

	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << std::endl;
	
	


	return true;
}

bool
SimulationControlManager::stop()
{
	bool result = true;

	return result;
}

void
SimulationControlManager::setMEBComponent(IMEBComponent* realMEB)
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
	return new SimulationControlManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}
