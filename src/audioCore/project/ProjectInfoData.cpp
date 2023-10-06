#include "ProjectInfoData.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

void ProjectInfoData::init() {
	/** Time */
	this->createTime = utils::getCurrentTime();
	this->lastSavedTime = 0;
	this->spentTime = 0;
	this->lastUpdateTime = this->createTime;

	/** Platform */
	this->createPlatform = utils::createPlatformInfoString();
	this->lastSavedPlatform = "";

	/** Author */
	this->authors = { utils::getUserName() };
}

void ProjectInfoData::update() {
	/** Time */
	this->lastSavedTime = utils::getCurrentTime();
	this->spentTime = (this->lastSavedTime - this->lastUpdateTime);
	this->lastUpdateTime = lastSavedTime;

	/** Platform */
	this->lastSavedPlatform = utils::createPlatformInfoString();
}

bool ProjectInfoData::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::ProjectInfo*>(data);
	if (!mes) { return false; }

	/** Time */
	this->createTime = mes->createdtime();
	this->lastSavedTime = mes->lastsavedtime();
	this->spentTime = mes->spenttime();
	this->lastUpdateTime = utils::getCurrentTime();

	/** Platform */
	this->createPlatform = mes->createdplatform();
	this->lastSavedPlatform = mes->lastsavedplatform();

	/** Authors */
	this->authors.clear();
	for (auto& author : mes->authors()) {
		this->authors.add(author);
	}
	if ((this->authors.size() < 1) || (*this->authors.end()) != utils::getUserName()) {
		this->authors.add(utils::getUserName());
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> ProjectInfoData::serialize() const {
	auto mes = std::make_unique<vsp4::ProjectInfo>();

	mes->set_createdtime(this->createTime);
	mes->set_lastsavedtime(this->lastSavedTime);
	mes->set_spenttime(this->spentTime);

	mes->set_createdplatform(this->createPlatform.toStdString());
	mes->set_lastsavedplatform(this->lastSavedPlatform.toStdString());

	auto authors = mes->mutable_authors();
	for (auto& s : this->authors) {
		authors->Add(s.toStdString());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

ProjectInfoData* ProjectInfoData::getInstance() {
	return ProjectInfoData::instance ? ProjectInfoData::instance : (ProjectInfoData::instance = new ProjectInfoData());
}

void ProjectInfoData::releaseInstance() {
	if (ProjectInfoData::instance) {
		delete ProjectInfoData::instance;
		ProjectInfoData::instance = nullptr;
	}
}

ProjectInfoData* ProjectInfoData::instance = nullptr;
