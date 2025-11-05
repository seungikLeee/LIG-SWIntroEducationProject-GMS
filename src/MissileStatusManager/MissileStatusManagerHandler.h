#pragma once
#include <nFramework/BaseManager.h>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/mec/MECComponent.h>
#include <nFramework/nLineStream/NLineTstream.h>
#include <nFramework/nTimer/NTimer.h>

using namespace nframework;
using namespace nom;

class MissileStatusManagerHandler {
public:
	MissileStatusManagerHandler(nframework::BaseManager*, nframework::IMEBComponent*);
	~MissileStatusManagerHandler();

public:
	void processMessage(std::shared_ptr<nframework::NOM>);
	void processSimulationMode(std::shared_ptr<nframework::NOM>);
	void processAirThreatInfo(std::shared_ptr<nframework::NOM>);
	void processLauncherPosition(std::shared_ptr<nframework::NOM>);
	void processLaunchMissile(std::shared_ptr<nframework::NOM>);
	void processInterceptSuccess();
	void processInterceptFail();
	
private:
	void initialize();
	void release();

	void sendMissileCallback();
	void moveMissileTowardTarget(double);
	void sendInterceptResult(std::shared_ptr<nframework::NOM>);

private:
	nframework::BaseManager* userMgr;
	nframework::IMEBComponent* meb;

	nframework::nlinestream::NLineTstream ntcout{ nframework::nlinestream::Level::COUT };
	nframework::nlinestream::NLineTstream ntcerr{ nframework::nlinestream::Level::CERR };

	std::map<tstring, std::function<void(std::shared_ptr<nframework::NOM>)>> nomProcessorMap;
	
	std::shared_ptr<nframework::NOM> simulationMode;
	std::shared_ptr<nframework::NOM> airThreatInfo;
	std::shared_ptr<nframework::NOM> launcherPosition;
	std::shared_ptr<nframework::NOM> missileStatusNOM;

	nframework::NTimer* nTimer;
	int timerHandle;
};