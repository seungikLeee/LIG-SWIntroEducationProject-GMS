#pragma once
#include <nFramework/BaseManager.h>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/mec/MECComponent.h>
#include <nFramework/nLineStream/NLineTstream.h>

using namespace nframework;
using namespace nom;

class SimulationControlManagerHandler {
public:
	SimulationControlManagerHandler(nframework::BaseManager*, nframework::IMEBComponent*);
	~SimulationControlManagerHandler();

public:
	void processMessage(std::shared_ptr<nframework::NOM>);
	void processSetSimulationMode(std::shared_ptr<nframework::NOM>);
	void processSendGMSCommand(std::shared_ptr<nframework::NOM>);
	void processSetScenarioDeployStatus(std::shared_ptr<nframework::NOM>);

private:
	void initialize();
	void release();

private:
	nframework::BaseManager* userMgr;
	nframework::IMEBComponent* meb;

	nframework::nlinestream::NLineTstream ntcout{ nframework::nlinestream::Level::COUT };
	nframework::nlinestream::NLineTstream ntcerr{ nframework::nlinestream::Level::CERR };

	std::map<tstring, std::function<void(std::shared_ptr<nframework::NOM>)>> nomProcessorMap;
	std::shared_ptr<nframework::NOM> scenario;

	bool simulationStatus; // 모의 상태 (true: 모의 진행 중, false: 모의 진행 X)
	bool scenarioDeployStatus; // 시나리오 배포 상태 (true: 배포 완료, false: 배포 X)
};