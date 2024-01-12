#include "MessageModel.h"

void MessageModel::addMessage(const Message& mes) {
	this->list.add(mes);
	this->sendChangeMessage();
}

void MessageModel::addNow(const juce::String& mes, const Callback& callback) {
	this->addMessage({ juce::Time::getCurrentTime(), mes, callback });
}

void MessageModel::clear() {
	this->list.clear();
	this->sendChangeMessage();
}

const juce::Array<MessageModel::Message>& MessageModel::getList() const {
	return this->list;
}

int MessageModel::getNum() const {
	return this->list.size();
}

bool MessageModel::isEmpty() const {
	return this->getNum() == 0;
}

MessageModel* MessageModel::getInstance() {
	return MessageModel::instance ? MessageModel::instance
		: (MessageModel::instance = new MessageModel{});
}

void MessageModel::releaseInstance() {
	if (MessageModel::instance) {
		delete MessageModel::instance;
		MessageModel::instance = nullptr;
	}
}

MessageModel* MessageModel::instance = nullptr;
