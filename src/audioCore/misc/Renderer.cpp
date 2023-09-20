#include "Renderer.h"

#include "../AudioCore.h"
#include "../Utils.h"
#include "PlayPosition.h"

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

	/** Isolate Main Graph */
	AudioCore::getInstance()->setIsolation(true);

	/** Rendering Mode */
	this->renderer->setRendering(true);

	/** Get Total Time */
	double totalLength = mainGraph->getTailLengthSeconds();

	/** Reset Play Position */
	PlayPosition::getInstance()->setPositionInSamples(0);

	/** Render Each Block */
	while (PlayPosition::getInstance()->getPosition()
		->getTimeInSeconds().orFallback(0) < totalLength) {
		/** Stop */
		if (juce::Thread::currentThreadShouldExit()) {
			break;
		}

		/** Render */
		juce::AudioSampleBuffer audio;
		juce::MidiBuffer midi;
		mainGraph->processBlock(audio, midi);
	}

	/** Reset Rendering Mode */
	this->renderer->setRendering(false);

	/** Unisolate Main Graph */
	AudioCore::getInstance()->setIsolation(false);

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

bool Renderer::start(const RenderTaskList& tasks) {
	/** Thread Is Already Started */
	if (this->renderThread->isThreadRunning()) {
		return false;
	}

	/** Set Tasks */
	this->prepareToRender(tasks);

	/** Start Now */
	this->renderThread->startThread();

	return true;
}

void Renderer::setRendering(bool rendering) {
	this->rendering = rendering;
}

void Renderer::updateSampleRateAndBufferSize(
	double sampleRate, int bufferSize) {
	juce::GenericScopedLock locker(this->lock);

	if (this->sampleRate != sampleRate) {
		this->sampleRate = sampleRate;

		/** Clear Buffer */
		for (auto& i : this->buffers) {
			auto& [id, channels, buffer] = i.second;
			buffer.setSize(channels.size(), 0, false, true, true);
		}
	}
	this->bufferSize = bufferSize;
}

bool Renderer::getRendering() const {
	return this->rendering;
}

void Renderer::prepareToRender(const RenderTaskList& tasks) {
	juce::GenericScopedLock locker(this->lock);

	this->releaseBuffer();

	for (auto& [ptr, id, channels] : tasks) {
		this->buffers.insert(std::make_pair(
			ptr, std::make_tuple(id, channels,
				juce::AudioBuffer<float>{ channels.size(), 0 })));
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
	if (dstBuffer.getNumSamples() - buffer.getNumChannels() < offset) {
		dstBuffer.setSize(dstBuffer.getNumChannels(),
			offset + buffer.getNumChannels(), true, false, true);
	}

	/** Copy Data */
	for (int i = 0; i < dstBuffer.getNumChannels(); i++) {
		if (auto src = buffer.getReadPointer(i)) {
			dstBuffer.copyFrom(i, offset, src, buffer.getNumSamples());
		}
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
