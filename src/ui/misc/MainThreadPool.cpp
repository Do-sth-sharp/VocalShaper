#include "MainThreadPool.h"

MainThreadPool::MainThreadPool() {
	this->pool = std::make_unique<juce::ThreadPool>();
}

MainThreadPool::~MainThreadPool() {
	this->stopAll();
}

void MainThreadPool::stopAll() {
	this->pool->removeAllJobs(true, 30000, nullptr);
}

void MainThreadPool::runJob(const Job& job) {
	this->pool->addJob(job);
}

MainThreadPool* MainThreadPool::getInstance() {
	return MainThreadPool::instance ? MainThreadPool::instance
		: (MainThreadPool::instance = new MainThreadPool{});
}

void MainThreadPool::releaseInstance() {
	if (MainThreadPool::instance) {
		delete MainThreadPool::instance;
		MainThreadPool::instance = nullptr;
	}
}

MainThreadPool* MainThreadPool::instance = nullptr;
