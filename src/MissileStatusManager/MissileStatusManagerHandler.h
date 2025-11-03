#pragma once
#include <nFramework/BaseManager.h>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/mec/MECComponent.h>
#include <nFramework/nLineStream/NLineTstream.h>

using namespace nframework;
using namespace nom;

class MissileStatusManagerHandler {
public:
	MissileStatusManagerHandler(nframework::BaseManager*, nframework::IMEBComponent*);
	~MissileStatusManagerHandler();

public:
	void processMessage(std::shared_ptr<nframework::NOM>);
	void processSetSimulationMode(std::shared_ptr<nframework::NOM>);

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
};