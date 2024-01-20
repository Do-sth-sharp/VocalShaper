#include "CloneableSource.h"
#include "SynthRenderThread.h"
#include "../misc/AudioLock.h"
#include "../uiCallback/UICallback.h"
#include <DMDA.h>
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

class SourceRenderHelper final : public CloneableSource::SourceRenderHelperBase {
public:
	SourceRenderHelper() = delete;
	explicit SourceRenderHelper(CloneableSource* src) {
		this->thread = std::make_unique<SynthRenderThread>(src);
	}
	SynthRenderThread* getImpl() const override {
		return this->thread.get();
	}

private:
	std::unique_ptr<SynthRenderThread> thread = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceRenderHelper)
};

std::atomic_int CloneableSource::globalCounter = 0;

CloneableSource::CloneableSource(const juce::String& name)
	: CloneableSource(CloneableSource::globalCounter, name) {
}

CloneableSource::CloneableSource(
	int id, const juce::String& name)
	: name(name) {
	this->setId(id);

	this->synthThread = std::unique_ptr<SourceRenderHelperBase>(
		new SourceRenderHelper(this));
}

std::unique_ptr<CloneableSource> CloneableSource::cloneThis() const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	auto dst = this->createThisType();
	if (this->clone(dst.get())) {
		dst->name = this->name;
		dst->isSaved = (bool)this->isSaved;
		dst->currentSampleRate = (double)this->currentSampleRate;

		return std::move(dst);
	}
	return nullptr;
}

bool CloneableSource::cloneAs(CloneableSource* dst) const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (this->clone(dst)) {
		dst->name = this->name;
		dst->isSaved = (bool)this->isSaved;
		dst->currentSampleRate = (double)this->currentSampleRate;

		return true;
	}
	return false;
}

void CloneableSource::initThis(
	double sampleRate, int channelNum, int sampleNum) {
	this->init(sampleRate, channelNum, sampleNum);
}

bool CloneableSource::loadFrom(const juce::File& file) {
	if (this->load(file)) {
		this->isSaved = true;
		return true;
	}
	return false;
}

bool CloneableSource::saveAs(const juce::File& file) const {
	if (this->save(file)) {
		this->isSaved = true;
		return true;
	}
	return false;
}

double CloneableSource::getSourceLength() const {
	return this->getLength();
}

bool CloneableSource::checkSaved() const {
	return this->isSaved;
}

void CloneableSource::changed() {
	this->isSaved = false;
}

int CloneableSource::getId() const {
	return this->id;
}

void CloneableSource::setName(const juce::String& name) {
	this->name = name;

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

const juce::String CloneableSource::getName() const {
	return this->name;
}

void CloneableSource::setPath(const juce::String& path) {
	juce::ScopedWriteLock locker(audioLock::getSourceLock());
	this->path = path;
}

const juce::String CloneableSource::getPath() const {
	juce::ScopedReadLock locker(audioLock::getSourceLock());
	return this->path;
}

void CloneableSource::prepareToPlay(double sampleRate, int bufferSize) {
	juce::ScopedReadLock locker(audioLock::getSourceLock());

	if (this->currentSampleRate == sampleRate && this->currentBufferSize == bufferSize) { return; }

	this->currentSampleRate = sampleRate;
	this->currentBufferSize = bufferSize;
	this->sampleRateChanged();
}

double CloneableSource::getSampleRate() const {
	return this->currentSampleRate;
}

int CloneableSource::getBufferSize() const {
	return this->currentBufferSize;
}

void CloneableSource::setSynthesizer(
	std::unique_ptr<juce::AudioPluginInstance> synthesizer,
	const juce::String& identifier) {
	/** Stop Render */
	this->synthThread->getImpl()->stopThread(3000);

	/** Set Synthesizer */
	this->synthesizer = std::move(synthesizer);
	this->pluginIdentifier = identifier;

	/** DMDA Hand Shake */
	DMDA::PluginHandler handShakeHandler(
		[](DMDA::Context* context) { context->handShake(); });
	this->synthesizer->getExtensions(handShakeHandler);

	/** Callback */
	juce::MessageManager::callAsync([] {
		UICallbackAPI<int>::invoke(UICallbackType::SourceChanged, -1);
		});
}

const juce::String CloneableSource::getSynthesizerName() const {
	if (this->synthesizer) {
		return this->synthesizer->getName();
	}
	return juce::String{};
}

bool CloneableSource::isSynthRunning() const {
	return this->synthThread->getImpl()->isThreadRunning();
}

void CloneableSource::stopSynth() {
	this->synthThread->getImpl()->stopThread(3000);
}

void CloneableSource::synth() {
	/** Check Synthesizer */
	if (!this->synthesizer) { return; }
	auto dstSource = this->synthThread->getImpl()->getDstSource();
	if (!dstSource) { return; }

	/** Stop Render */
	this->synthThread->getImpl()->stopThread(3000);

	/** Lock Buffer */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	/** Prepare Synthesizer */
	{
		double sampleRate = std::max(this->getSourceSampleRate(), dstSource->getSourceSampleRate());
		if (sampleRate <= 0) { sampleRate = this->getSampleRate(); }
		int bufferSize = this->getBufferSize();
		this->synthesizer->prepareToPlay(sampleRate, bufferSize);
	}

	/** Start Render */
	this->synthThread->getImpl()->startThread();
}

void CloneableSource::setDstSource(CloneableSource::SafePointer<> dst) {
	this->synthThread->getImpl()->setDstSource(dst);
}

CloneableSource::SafePointer<> CloneableSource::getDstSource() const {
	return this->synthThread->getImpl()->getDstSource();
}

void CloneableSource::setSynthesizerState(const juce::MemoryBlock& state) {
	if (this->synthesizer) {
		this->synthesizer->setStateInformation(
			state.getData(), state.getSize());
	}
}

const juce::MemoryBlock CloneableSource::getSynthesizerState() const {
	juce::MemoryBlock result;

	if (this->synthesizer) {
		this->synthesizer->getStateInformation(result);
	}

	return result;
}

void CloneableSource::resetIdCounter() {
	CloneableSource::globalCounter = 0;
}

void CloneableSource::setId(int id) {
	if (this->id == id) { return; }
	this->id = std::max(id, (int)CloneableSource::globalCounter);
	CloneableSource::globalCounter = this->id + 1;
}

void CloneableSource::prepareToRecordInternal(
	int inputChannels, double sampleRate,
	int blockSize, bool updateOnly) {
	this->isRecording = true;
	this->prepareToRecord(
		inputChannels, sampleRate, blockSize, updateOnly);
}

void CloneableSource::recordingFinishedInternal() {
	this->recordingFinished();
	this->isRecording = false;
}

bool CloneableSource::checkRecording() const {
	return this->isRecording;
}
