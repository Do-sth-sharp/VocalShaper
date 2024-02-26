#include "Renderer.h"

#include "../AudioCore.h"
#include "../Utils.h"
#include "PlayPosition.h"
#include "../plugin/PluginLoader.h"
#include "../misc/VMath.h"

class RenderThread final : public juce::Thread {
public:
	RenderThread() = delete;
	RenderThread(Renderer* renderer);

	void prepare(const juce::File& dir,
		const juce::String& name, const juce::String& extension);

public:
	void run() override;

private:
	Renderer* const renderer = nullptr;
	juce::File dir;
	juce::String name = "untitled";
	juce::String extension = ".wav";

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenderThread)
};

RenderThread::RenderThread(Renderer* renderer)
	: Thread("Render Thread"), renderer(renderer) {}

void RenderThread::prepare(const juce::File& dir,
	const juce::String& name, const juce::String& extension) {
	if (this->isThreadRunning()) { return; }
	this->dir = dir;
	this->name = name;
	this->extension = extension;
}

void RenderThread::run() {
	/** Check Renderer */
	if (!this->renderer) { return; }

	/** Get Main Graph */
	auto mainGraph = AudioCore::getInstance()->getGraph();
	if (!mainGraph) { return; }

	/** Reset Play Head State */
	AudioCore::getInstance()->stop();

	/** Buffer Audio */
	{
		double bufferArea = this->renderer->audioBufferArea;
		int bufferLength = bufferArea * this->renderer->sampleRate;
		int bufferBlockNum = std::ceil(bufferLength / this->renderer->bufferSize);

		for (int i = 0; i < bufferBlockNum; i++) {
			/** Stop */
			if (juce::Thread::currentThreadShouldExit()) {
				break;
			}

			/** Render */
			juce::AudioSampleBuffer audio(1, this->renderer->bufferSize);
			juce::MidiBuffer midi;
			mainGraph->processBlock(audio, midi);
		}
	}

	/** Set Play Head State */
	AudioCore::getInstance()->setPositon(0);
	AudioCore::getInstance()->play();

	/** Rendering Mode */
	this->renderer->setRendering(true);

	/** Get Total Time */
	double totalLength = mainGraph->getTailLengthSeconds();
	totalLength = std::min(totalLength, INT_MAX / PlayPosition::getInstance()->getSampleRate());

	/** Reset Play Position */
	PlayPosition::getInstance()->setPositionInSamples(0);

	/** Render Each Block */
	juce::GenericScopedLock graphLocker(mainGraph->getCallbackLock());
	while (PlayPosition::getInstance()->getPosition()
		->getTimeInSeconds().orFallback(0) < totalLength) {
		/** Stop */
		if (juce::Thread::currentThreadShouldExit()) {
			break;
		}

		/** Render */
		juce::AudioSampleBuffer audio(1, this->renderer->bufferSize);
		juce::MidiBuffer midi;
		mainGraph->processBlock(audio, midi);
	}

	/** Reset Rendering Mode */
	this->renderer->setRendering(false);

	/** Unisolate Main Graph */
	juce::MessageManager::callAsync(
		[] {
			if (auto ac = AudioCore::getInstanceWithoutCreate()) {
				ac->stop();
				ac->setIsolation(false);
			}});
	
	/** Save Audio */
	this->renderer->saveFile(this->dir, this->name, this->extension);

	/** Clear Buffer */
	this->renderer->releaseBuffer();
}

Renderer::Renderer() {
	/** Render Thread */
	this->renderThread = std::unique_ptr<juce::Thread>(new RenderThread(this));
}

Renderer::~Renderer() {
	if (this->renderThread) {
		this->renderThread->stopThread(3000);
	}
}

bool Renderer::start(const juce::Array<int>& tracks, const juce::String& path,
	const juce::String& name, const juce::String& extension) {
	/** Async Protection */
	if (PluginLoader::getInstance()->isRunning()) { return false; }

	/** Thread Is Already Started */
	if (this->renderThread->isThreadRunning()) {
		return false;
	}

	/** Get Tasks */
	auto graph = AudioCore::getInstance()->getGraph();
	if (!graph) { return false; }

	Renderer::RenderTaskList tasks;
	for (auto& i : tracks) {
		if (i >= 0 && i < graph->getTrackNum()) {
			if (auto track = graph->getTrackProcessor(i)) {
				tasks.add({ track, i, track->getAudioChannelSet() });
			}
		}
	}

	/** Prepare Path */
	juce::File dir
		= utils::getProjectDir().getChildFile(path);
	if (!dir.isDirectory()) {
		return false;
	}

	/** Prepare Thread */
	if (auto thread = dynamic_cast<RenderThread*>(this->renderThread.get())) {
		thread->prepare(dir, name, extension);
	}
	else {
		return false;
	}

	/** Set Tasks */
	this->prepareToRender(tasks);

	/** Isolate Main Graph */
	AudioCore::getInstance()->setIsolation(true);

	/** Clear Play Head State */
	AudioCore::getInstance()->record(false);
	AudioCore::getInstance()->stop();

	/** Prepare Main Graph */
	{
		juce::GenericScopedLock locker(this->lock);
		graph->prepareToPlay(this->sampleRate, this->bufferSize);
	}

	/** Process Block To Close All MIDI Notes */
	{
		juce::AudioSampleBuffer audio(1, this->bufferSize);
		juce::MidiBuffer midi;
		graph->processBlock(audio, midi);
	}

	/** Start Async */
	juce::MessageManager::callAsync(
		[] {Renderer::getInstance()->startThreadInternal(); });

	return true;
}

void Renderer::startThreadInternal() {
	this->renderThread->startThread();
}

void Renderer::setRendering(bool rendering) {
	this->rendering = rendering;
}

void Renderer::updateSampleRateAndBufferSize(
	double sampleRate, int bufferSize) {
	juce::GenericScopedLock locker(this->lock);

	if (this->sampleRate != sampleRate) {
		/** Clear Buffer */
		for (auto& i : this->buffers) {
			auto& [id, channels, buffer] = i.second;

			int newSize =
				std::ceil(buffer.getNumSamples() * (sampleRate / this->sampleRate));
			buffer.setSize(channels.size(), newSize, false, true, true);
		}

		this->sampleRate = sampleRate;
	}
	this->bufferSize = bufferSize;
}

bool Renderer::getRendering() const {
	return this->rendering;
}

void Renderer::prepareToRender(const RenderTaskList& tasks) {
	juce::GenericScopedLock locker(this->lock);

	this->releaseBuffer();

	auto graph = AudioCore::getInstance()->getGraph();
	double totalLength = graph ? graph->getTailLengthSeconds() : 0;
	int bufferSize = std::ceil(totalLength * this->sampleRate);

	for (auto& [ptr, id, channels] : tasks) {
		juce::AudioBuffer<float> buffer;
		buffer.setSize(channels.size(), bufferSize, false, true, true);
		this->buffers.insert(std::make_pair(
			ptr, std::make_tuple(id, channels, buffer)));
	}
}

void Renderer::saveFile(const juce::File& dir,
	const juce::String& name, const juce::String& extension) {
	/** Lock */
	juce::GenericScopedLock locker(this->lock);

	/** Save Each Buffer */
	for (auto& i : this->buffers) {
		/** Stop */
		if (juce::Thread::currentThreadShouldExit()) {
			break;
		}

		/** Get Buffer */
		auto& [id, channels, buffer] = i.second;
		
		/** Create File */
		auto file = dir.getChildFile(name + "_" + juce::String(id) + extension);
		if (file.exists()) {
			file.deleteFile();
		}

		/** Create Audio Writer */
		auto writer = utils::createAudioWriter(file, this->sampleRate, channels);
		if (!writer) { continue; }

		/** Write Data */
		writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
	}
}

void Renderer::releaseBuffer() {
	juce::GenericScopedLock locker(this->lock);
	this->buffers.clear();
}

void Renderer::writeData(const Track* trackPtr,
	const juce::AudioBuffer<float>& buffer, int offset) {
	/** Check Rendering State */
	if (!this->rendering) { return; }

	/** Lock */
	juce::GenericScopedLock locker(this->lock);

	/** Find Buffer */
	auto bufferIt = this->buffers.find(trackPtr);
	if (bufferIt == this->buffers.end()) { return; }
	auto& dstBuffer = std::get<2>(bufferIt->second);

	/** Increase Buffer Size */
	if (dstBuffer.getNumSamples() - buffer.getNumSamples() < offset) {
		dstBuffer.setSize(dstBuffer.getNumChannels(),
			offset + buffer.getNumSamples(), true, false, true);
	}

	/** Copy Data */
	for (int i = 0; i < dstBuffer.getNumChannels(); i++) {
		vMath::copyAudioData(dstBuffer, buffer,
			offset, 0, i, i, buffer.getNumSamples());
	}
}

Renderer* Renderer::getInstance() {
	return Renderer::instance
		? Renderer::instance : (Renderer::instance = new Renderer());
}

void Renderer::releaseInstance() {
	if (Renderer::instance) {
		delete Renderer::instance;
		Renderer::instance = nullptr;
	}
}

Renderer* Renderer::instance = nullptr;
