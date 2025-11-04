#pragma once
#include <nFramework/BaseManager.h>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/mec/MECComponent.h>
#include <nFramework/nLineStream/NLineTstream.h>
#include <nFramework/nTimer/NTimer.h>

using namespace nframework;
using namespace nom;

class InterceptManagerHandler {
public:
	InterceptManagerHandler(nframework::BaseManager*, nframework::IMEBComponent*);
	~InterceptManagerHandler();

public:
	void processMessage(std::shared_ptr<nframework::NOM>);
	void processSendGMSCommand(std::shared_ptr<nframework::NOM>);
	void processSetSimulationMode(std::shared_ptr<nframework::NOM>);
	void processAirThreatInfo(std::shared_ptr<nframework::NOM>);
	void processMissileStatus(std::shared_ptr<nframework::NOM>);

private:
	void initialize();
	void release();

private:
	nframework::BaseManager* userMgr;
	nframework::IMEBComponent* meb;

	nframework::nlinestream::NLineTstream ntcout{ nframework::nlinestream::Level::COUT };
	nframework::nlinestream::NLineTstream ntcerr{ nframework::nlinestream::Level::CERR };

	std::map<tstring, std::function<void(std::shared_ptr<nframework::NOM>)>> nomProcessorMap;
	std::shared_ptr<nframework::NOM> simulationMode;
	std::shared_ptr<nframework::NOM> airThreatInfo;
	std::shared_ptr<nframework::NOM> missileInfo;

	bool simulationStatus; // 모의 상태 (true: 모의 진행 중, false: 모의 진행 X)

	nframework::NTimer* nTimer;
	int timerHandle;
};