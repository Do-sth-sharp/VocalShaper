#include "CloneableSourceManager.h"
#include "AudioIOList.h"
#include "../AudioCore.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../uiCallback/UICallback.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

bool CloneableSourceManager::addSource(std::unique_ptr<CloneableSource> src) {
	juce::ScopedWriteLock locker(this->getLock());
	src->prepareToPlay(this->sampleRate, this->bufferSize);
	this->sourceList.add(std::move(src));

	juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
		});

	return true;
}

bool CloneableSourceManager::removeSource(CloneableSource* src) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.removeObject(src, true);

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	return true;
}

bool CloneableSourceManager::removeSource(int index) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.remove(index, true);

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	return true;
}

CloneableSource::SafePointer<> CloneableSourceManager::getSource(int index) const {
	juce::ScopedReadLock locker(this->getLock());
	return this->sourceList[index];
}

int CloneableSourceManager::getSourceNum() const {
	juce::ScopedReadLock locker(this->getLock());
	return this->sourceList.size();
}

juce::ReadWriteLock& CloneableSourceManager::getLock() const {
	return this->lock;
}

bool CloneableSourceManager::setSourceSynthesizer(
	int index, const juce::String& identifier) {
	juce::ScopedReadLock locker(this->getLock());
	auto source = this->getSource(index);
	if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
		if (auto des = Plugin::getInstance()->findPlugin(identifier, true)) {
			PluginLoader::getInstance()->loadPlugin(*(des.get()),
				CloneableSource::SafePointer<CloneableSynthSource>(src));

			juce::MessageManager::callAsync([index] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return true;
		}
	}
	return false;
}

bool CloneableSourceManager::synthSource(int index) {
	juce::ScopedReadLock locker(this->getLock());
	auto source = this->getSource(index);
	if (auto src = dynamic_cast<CloneableSynthSource*>(source.getSource())) {
		src->synth();

		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}
	return false;
}

bool CloneableSourceManager::cloneSource(int index) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());
	
	/** Clone */
	if (auto source = this->getSource(index)) {
		if (auto ptr = source->cloneThis()) {
			bool result = this->addSource(std::move(ptr));

			/** Callback */
			juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return result;
		}
	}
	return false;
}

bool CloneableSourceManager::cloneSourceAsync(int index) {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		/** Create */
		auto dst = src->createThisType();
		auto ptr = dst.get();

		/** Add To List */
		if (!this->addSource(std::move(dst))) { return false; }

		/** Load Async */
		AudioIOList::getInstance()->clone(src, CloneableSource::SafePointer<>{ ptr });

		/** Callback */
		juce::MessageManager::callAsync([index = this->sourceList.size() - 1] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}

	return false;
}

bool CloneableSourceManager::saveSource(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		if (src->saveAs(utils::getSourceFile(path))) {
			src->setPath(path);

			/** Callback */
			juce::MessageManager::callAsync([index] {
				UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
				});

			return true;
		}
	}

	return false;
}

bool CloneableSourceManager::saveSourceAsync(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		AudioIOList::getInstance()->save(src, path);

		/** Callback */
		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}

	return false;
}

bool CloneableSourceManager::exportSource(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		bool result = src->exportAs(utils::getSourceFile(path));

		/** Callback */
		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return result;
	}

	return false;
}

bool CloneableSourceManager::exportSourceAsync(
	int index, const juce::String& path) const {
	/** Lock */
	juce::ScopedReadLock locker(this->getLock());

	/** Get Source */
	if (auto src = this->getSource(index)) {
		AudioIOList::getInstance()->exportt(src, path);

		/** Callback */
		juce::MessageManager::callAsync([index] {
			UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
			});

		return true;
	}

	return false;
}

bool CloneableSourceManager::reloadSourceAsync(
	int index, const juce::String& path, bool copy) {
	/** Lock */
	juce::ScopedWriteLock locker(this->getLock());

	/** Create */
	auto source = this->getSource(index);
	if (!source) { return false; }

	/** Load Async */
	AudioIOList::getInstance()->load(source, path, copy);

	/** Callback */
	juce::MessageManager::callAsync([index] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, index);
		});

	return true;
}

void CloneableSourceManager::prepareToPlay(double sampleRate, int bufferSize) {
	juce::ScopedWriteLock locker(this->getLock());
	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;
	for (auto i : this->sourceList) {
		i->prepareToPlay(sampleRate, bufferSize);
	}

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

void CloneableSourceManager::clearGraph() {
	juce::ScopedWriteLock locker(this->getLock());
	this->sourceList.clear(true);
	CloneableSource::resetIdCounter();

	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

bool CloneableSourceManager::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::SourceList*>(data);
	if (!mes) { return false; }

	juce::ScopedWriteLock locker(this->getLock());

	/** Clear List */
	this->clearGraph();

	/** Load Each Source */
	auto& list = mes->sources();
	for (auto& i : list) {
		juce::String path = i.path();

		switch (i.type()) {
		case vsp4::Source_Type_AUDIO:
			if (!this->createNewSourceThenLoadAsync<CloneableAudioSource>(path, false)) { continue; }
			break;
		case vsp4::Source_Type_MIDI:
			if (!this->createNewSourceThenLoadAsync<CloneableMIDISource>(path, false)) { continue; }
			break;
		case vsp4::Source_Type_SYNTH:
			if (!this->createNewSourceThenLoadAsync<CloneableSynthSource>(path, false)) { continue; }
			break;
		}

		auto ptrSrc = this->getSource(this->getSourceNum() - 1);
		if (!ptrSrc) { continue; }

		ptrSrc->setId(i.id());
		ptrSrc->setName(i.name());
		ptrSrc->setPath(i.path());

		if ((i.type() == vsp4::Source_Type_SYNTH) 
			&& i.has_synthesizer()) {
			auto& plugin = i.synthesizer();

			auto callback = [plugin, index = this->getSourceNum() - 1] {
				auto src = CloneableSourceManager::getInstance()->getSource(index);
				auto synthSource = dynamic_cast<CloneableSynthSource*>(src.getSource());
				if (!synthSource) { return; }
				synthSource->parse(&plugin);
			};

			auto pluginDes = Plugin::getInstance()->findPlugin(plugin.info().id(), true);
			PluginLoader::getInstance()->loadPlugin(
				*(pluginDes.get()), CloneableSource::SafePointer<CloneableSynthSource>(
					dynamic_cast<CloneableSynthSource*>(ptrSrc.getSource())), callback);
		}
	}

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});

	return true;
}

std::unique_ptr<google::protobuf::Message> CloneableSourceManager::serialize() const {
	auto mes = std::make_unique<vsp4::SourceList>();

	juce::ScopedReadLock locker(this->getLock());
	auto list = mes->mutable_sources();
	for (auto i : this->sourceList) {
		auto smes = i->serialize();
		if (!dynamic_cast<vsp4::Source*>(smes.get())) { return nullptr; }
		list->AddAllocated(dynamic_cast<vsp4::Source*>(smes.release()));
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

CloneableSourceManager* CloneableSourceManager::getInstance() {
	return CloneableSourceManager::instance
		? CloneableSourceManager::instance
		: (CloneableSourceManager::instance = new CloneableSourceManager());
}

void CloneableSourceManager::releaseInstance() {
	if (CloneableSourceManager::instance) {
		delete CloneableSourceManager::instance;
		CloneableSourceManager::instance = nullptr;
	}
}

CloneableSourceManager* CloneableSourceManager::instance = nullptr;
