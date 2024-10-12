#include "ProjectInfoData.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

void ProjectInfoData::init() {
	/** Time */
	this->content.createTime = utils::getCurrentTime();
	this->content.lastSavedTime = 0;
	this->content.spentTime = 0;
	this->content.lastUpdateTime = this->content.createTime;

	/** Platform */
	this->content.createPlatform = utils::createPlatformInfoString();
	this->content.lastSavedPlatform = "";

	/** Author */
	this->content.authors = { utils::getUserName() };

	/** Saved */
	this->content.saved = true;
}

void ProjectInfoData::update() {
	/** Time */
	this->content.lastSavedTime = utils::getCurrentTime();
	this->content.spentTime = (this->content.lastSavedTime - this->content.lastUpdateTime);
	this->content.lastUpdateTime = this->content.lastSavedTime;

	/** Platform */
	this->content.lastSavedPlatform = utils::createPlatformInfoString();

	/** Saved */
	this->content.saved = true;
}

void ProjectInfoData::push() {
	this->traceback.push(this->content);
}

void ProjectInfoData::pop() {
	if (this->traceback.size() > 0) {
		this->content = this->traceback.top();
		this->traceback.pop();
	}
}

void ProjectInfoData::release() {
	while (this->traceback.size() > 0) {
		this->traceback.pop();
	}
}

bool ProjectInfoData::checkSaved() const {
	return this->content.saved;
}

void ProjectInfoData::unsave() {
	this->content.saved = false;
}

bool ProjectInfoData::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::ProjectInfo*>(data);
	if (!mes) { return false; }

	/** Time */
	this->content.createTime = mes->createdtime();
	this->content.lastSavedTime = mes->lastsavedtime();
	this->content.spentTime = mes->spenttime();
	this->content.lastUpdateTime = utils::getCurrentTime();

	/** Platform */
	this->content.createPlatform = mes->createdplatform();
	this->content.lastSavedPlatform = mes->lastsavedplatform();

	/** Authors */
	this->content.authors.clear();
	for (auto& author : mes->authors()) {
		this->content.authors.add(author);
	}
	if ((this->content.authors.size() < 1) || (*(this->content.authors.end() - 1)) != utils::getUserName()) {
		this->content.authors.add(utils::getUserName());
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> ProjectInfoData::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::ProjectInfo>();

	mes->set_createdtime(this->content.createTime);
	mes->set_lastsavedtime(this->content.lastSavedTime);
	mes->set_spenttime(this->content.spentTime);

	mes->set_createdplatform(this->content.createPlatform.toStdString());
	mes->set_lastsavedplatform(this->content.lastSavedPlatform.toStdString());

	auto authors = mes->mutable_authors();
	for (auto& s : this->content.authors) {
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
