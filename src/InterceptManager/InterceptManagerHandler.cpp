#include "InterceptManagerHandler.h"

/************************************************************************
	constructor / destructor
************************************************************************/
InterceptManagerHandler::InterceptManagerHandler(nframework::BaseManager* mgr, nframework::IMEBComponent* MEB) : userMgr(mgr), meb(MEB)
{
	initialize();
}

InterceptManagerHandler::~InterceptManagerHandler()
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void InterceptManagerHandler::initialize()
{
	std::function<void(std::shared_ptr<nframework::NOM>)> nomMsgProc;
	nomMsgProc = std::bind(&InterceptManagerHandler::processSendGMSCommand, this, std::placeholders::_1);
	nomProcessorMap.insert(std::make_pair(_T("SendGMSCommand"), nomMsgProc));
}

void InterceptManagerHandler::release()
{
	nomProcessorMap.clear();
}

void InterceptManagerHandler::processMessage(std::shared_ptr<nframework::NOM> nomMsg)
{
	if (auto itr = nomProcessorMap.find(nomMsg->getName()); itr != nomProcessorMap.end())
		itr->second(nomMsg);
}

/************************************************************************
	Busniess Logic
************************************************************************/
/*
* 발사 명령을 수신하고 미사일 객체를 생성하는 함수
* 매개변수: 발사 명령 NOM 메세지
* 반환값:void
*/
void InterceptManagerHandler::processSendGMSCommand(std::shared_ptr<nframework::NOM> _sendGMSCommand)
{
	ntcout << _T("[") << _T(__FUNCTION__) << _T("] ") << _sendGMSCommand->getName() << std::endl;
	ntcout << "Receive SendGMSCommand Info in InterceptManager!" << std::endl;

	auto msgId = _sendGMSCommand->getValue(_T("msgId"))->toUShort();
	auto length = _sendGMSCommand->getValue(_T("length"))->toUShort();
	auto fire = _sendGMSCommand->getValue(_T("fire"))->toChar();

	ntcout << "msgId: " << msgId << std::endl;
	ntcout << "length: " << length << std::endl;
	ntcout << "fire: " << fire << std::endl;
}