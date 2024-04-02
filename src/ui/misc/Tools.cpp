#include "Tools.h"

void Tools::setType(Type type) {
	this->type = type;
}

Tools::Type Tools::getType() const {
	return this->type;
}

void Tools::setAdsorb(double adsorbLevel) {
	this->adsorbLevel = adsorbLevel;
}

double Tools::getAdsorb() const {
	return this->adsorbLevel;
}

void Tools::setFollow(bool follow) {
	this->editorFollow = follow;
}

bool Tools::getFollow() const {
	return this->editorFollow;
}

Tools* Tools::getInstance() {
	return Tools::instance ? Tools::instance
		: (Tools::instance = new Tools{});
}

void Tools::releaseInstance() {
	if (Tools::instance) {
		delete Tools::instance;
		Tools::instance = nullptr;
	}
}

Tools* Tools::instance = nullptr;
