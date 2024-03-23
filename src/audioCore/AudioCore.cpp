#include "AudioCore.h"

#include "debug/AudioDebugger.h"
#include "debug/MIDIDebugger.h"
#include "plugin/PluginLoader.h"
#include "plugin/Plugin.h"
#include "misc/PlayPosition.h"
#include "misc/PlayWatcher.h"
#include "misc/Renderer.h"
#include "misc/Device.h"
#include "misc/AudioLock.h"
#include "project/ProjectInfoData.h"
#include "action/ActionDispatcher.h"
#include "uiCallback/UICallback.h"
#include "Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

AudioDeviceChangeListener::AudioDeviceChangeListener(AudioCore* parent) : parent(parent) {};
void AudioDeviceChangeListener::changeListenerCallback(juce::ChangeBroadcaster* /*source*/) {
	if (parent) {
		/** Update Audio Buses */
		parent->updateAudioBuses();
	}
};

AudioCore::AudioCore() {
	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<MainGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** Mackie Control Hub */
	this->mackieHub = std::make_unique<MackieControlHub>();

	/** Audio Debug */
	this->audioDebugger = std::make_unique<AudioDebugger>();

	/** MIDI Debug */
	this->midiDebugger = std::make_unique<MIDIDebugger>();
	this->mainAudioGraph->setMIDIMessageHook(
		[debugger = juce::Component::SafePointer(dynamic_cast<MIDIDebugger*>(this->midiDebugger.get()))] 
		(const juce::MidiMessage& mes, bool isInput) {
			if (debugger) {
				debugger->addMessage(mes, isInput);
			}
		});

	/** Actions */
	ActionDispatcher::getInstance()->setOutput(
		[debugger = juce::Component::SafePointer{ this->audioDebugger.get() }]
		(const juce::String& mes) {
			if (auto ptr = dynamic_cast<AudioDebugger*>(debugger.getComponent())) {
				ptr->output(mes);
			}
		},
		[debugger = juce::Component::SafePointer{ this->audioDebugger.get() }]
		(const juce::String& mes) {
			if (auto ptr = dynamic_cast<AudioDebugger*>(debugger.getComponent())) {
				ptr->output("[ERROR]" + mes);
			}
			UICallbackAPI<const juce::String&>::invoke(UICallbackType::ErrorMessage, mes);
		}
	);

	/** Audio Device Listener */
	this->audioDeviceListener = std::make_unique<AudioDeviceChangeListener>(this);
	Device::getInstance()->addChangeListener(this->audioDeviceListener.get());

	/** Init Audio Device */
	this->initAudioDevice();

	/** Init Project */
	ProjectInfoData::getInstance()->init();

	/** Set Working Directory */
	juce::File defaultWorkingDir = utils::getDefaultWorkingDir();
	defaultWorkingDir.createDirectory();
	utils::setProjectDir(defaultWorkingDir);

	/** Start Play Watcher */
	PlayWatcher::getInstance()->startTimer(1000);
}

AudioCore::~AudioCore() {
	Renderer::releaseInstance();
	this->audioDebugger = nullptr;
	Device::releaseInstance();
	this->mainGraphPlayer->setProcessor(nullptr);
	PlayWatcher::releaseInstance();
	PlayPosition::releaseInstance();
	Plugin::releaseInstance();
	UICallback::releaseInstance();
}

juce::Component* AudioCore::getAudioDebugger() const {
	return this->audioDebugger.get();
}

juce::Component* AudioCore::getMIDIDebugger() const {
	return this->midiDebugger.get();
}

void AudioCore::setMIDIDebuggerMaxNum(int num) {
	if (auto dbg = dynamic_cast<MIDIDebugger*>(this->getMIDIDebugger())) {
		dbg->setMaxNum(num);
	}
}

int AudioCore::getMIDIDebuggerMaxNum() const {
	if (auto dbg = dynamic_cast<MIDIDebugger*>(this->getMIDIDebugger())) {
		return dbg->getMaxNum();
	}
	return -1;
}

void AudioCore::play() {
	PlayPosition::getInstance()->transportPlay(true);
}

void AudioCore::pause() {
	PlayPosition::getInstance()->transportPlay(false);
	this->mainAudioGraph->closeAllNote();
}

void AudioCore::stop() {
	PlayPosition::getInstance()->transportPlay(false);
	this->mainAudioGraph->closeAllNote();
	if (this->returnToStart) {
		PlayPosition::getInstance()->setPositionInSeconds(this->playStartTime);
	}
}

void AudioCore::rewind() {
	this->playStartTime = 0;
	this->mainAudioGraph->closeAllNote();
	PlayPosition::getInstance()->transportRewind();
}

void AudioCore::record(bool start) {
	PlayPosition::getInstance()->transportRecord(start);
}

void AudioCore::setPositon(double pos) {
	/** Limit Pos */
	pos = std::max(0.0, pos);
	pos = std::min(this->mainAudioGraph->getTailLengthSeconds(), pos);

	/** Set Time */
	this->playStartTime = pos;
	this->mainAudioGraph->closeAllNote();
	PlayPosition::getInstance()->setPositionInSeconds(pos);
}

void AudioCore::setReturnToPlayStartPosition(bool returnToStart) {
	this->returnToStart = returnToStart;
}

bool AudioCore::getReturnToPlayStartPosition() const {
	return this->returnToStart;
}

juce::Optional<juce::AudioPlayHead::PositionInfo> AudioCore::getPosition() const {
	return PlayPosition::getInstance()->getPosition();
}

void AudioCore::setIsolation(bool isolation) {
	this->mainGraphPlayer->setProcessor(
		isolation ? nullptr : this->mainAudioGraph.get());
}

bool AudioCore::renderNow(const juce::Array<int>& tracks, const juce::String& path,
	const juce::String& name, const juce::String& extension) {
	return Renderer::getInstance()->start(tracks, path, name, extension);
}

bool AudioCore::isRendering() const {
	return Renderer::getInstance()->getRendering();
}

MainGraph* AudioCore::getGraph() const {
	return this->mainAudioGraph.get();
}

MackieControlHub* AudioCore::getMackie() const {
	return this->mackieHub.get();
}

bool AudioCore::save(const juce::String& name) {
	/** Get Save Dir */
	juce::String legalName = utils::getLegalFileName(name);
	juce::File projFile = utils::getProjectDir()
		.getChildFile("./" + legalName +
			utils::getProjectFormatsSupported(true)[0].trimCharactersAtStart("*"));
	juce::File projDir = projFile.getParentDirectory();
	projDir.createDirectory();

	/** Update Project Info */
	ProjectInfoData::getInstance()->push();
	ProjectInfoData::getInstance()->update();

	/** Get Project Data */
	auto mes = this->serialize();
	if (!dynamic_cast<vsp4::Project*>(mes.get())) { ProjectInfoData::getInstance()->pop(); return false; };
	auto proj = std::unique_ptr<vsp4::Project>(dynamic_cast<vsp4::Project*>(mes.release()));

	juce::MemoryBlock projData;
	projData.setSize(proj->ByteSizeLong());
	if (!proj->SerializeToArray(projData.getData(), projData.getSize())) { ProjectInfoData::getInstance()->pop(); return false; }
	
	/** Write Project File */
	juce::FileOutputStream projStream(projFile);
	if (!projStream.openedOk()) { ProjectInfoData::getInstance()->pop(); return false; }
	projStream.setPosition(0);
	projStream.truncate();
	if (!projStream.write(projData.getData(), projData.getSize())) { ProjectInfoData::getInstance()->pop(); return false; }

	/** Release Project Info Temp */
	ProjectInfoData::getInstance()->release();
	return true;
}

bool AudioCore::load(const juce::String& path) {
	/** Check Renderer */
	if (Renderer::getInstance()->getRendering()) { return false; }

	/** Load Project File */
	juce::File projFile = utils::getDefaultWorkingDir().getChildFile(path);
	if (!projFile.existsAsFile()) { return false; }
	juce::FileInputStream projStream(projFile);
	if (!projStream.openedOk()) { return false; }
	juce::MemoryBlock projData;
	projData.setSize(projStream.getTotalLength());
	if (!projStream.read(projData.getData(), projData.getSize())) { return false; }

	/** Parse Project */
	auto proj = std::make_unique<vsp4::Project>();
	if (!proj->ParseFromArray(projData.getData(), projData.getSize())) { return false; }

	/** Check Project Version */
	auto& projVer = proj->version();
	if (!utils::projectVersionHighEnough(
		{ projVer.major(), projVer.minor(), projVer.patch() })) {
		return false;
	}
	if (!utils::projectVersionLowEnough(
		{ projVer.major(), projVer.minor(), projVer.patch() })) {
		return false;
	}

	/** Reset PlayHead */
	this->record(false);
	this->stop();
	this->rewind();

	/** Set Working Directory */
	juce::File projDir = projFile.getParentDirectory();
	utils::setProjectDir(projDir);

	/** Change Graph */
	if (this->parse(proj.get())) {
		return true;
	}

	utils::setProjectDir(utils::getDefaultWorkingDir());
	return false;
}

bool AudioCore::newProj(const juce::String& workingPath) {
	/** Check Renderer */
	if (Renderer::getInstance()->getRendering()) { return false; }

	/** Reset PlayHead */
	this->record(false);
	this->stop();
	this->rewind();

	/** Reset Project Info */
	ProjectInfoData::getInstance()->init();

	/** Reset Graph */
	this->clearGraph();

	/** Set Working Dir */
	juce::File workingDir = juce::File::getSpecialLocation(
		juce::File::SpecialLocationType::userHomeDirectory)
		.getChildFile(workingPath);
	return workingDir.createDirectory()
		&& utils::setProjectDir(workingDir);
}

void AudioCore::clearGraph() {
	/** Clear MainGraph */
	this->mainAudioGraph->clearGraph();
}

bool AudioCore::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::Project*>(data);
	if (!mes) { return false; }

	/** Set Info */
	ProjectInfoData::getInstance()->init();
	if (!ProjectInfoData::getInstance()->parse(&(mes->info()))) { return false; }

	/** Set Tempo */
	{
		juce::MemoryBlock tempoData(mes->tempoevents().c_str(), mes->tempoevents().length());
		juce::MemoryInputStream tempoStream(tempoData, false);
		juce::MidiFile tempoFile;
		tempoFile.readFrom(tempoStream);

		{
			juce::ScopedTryWriteLock locker(audioLock::getPositionLock());
			auto& tempos = PlayPosition::getInstance()->getTempoSequence();
			tempos.clear();
			if (tempoFile.getNumTracks() > 0) {
				tempos = *(tempoFile.getTrack(0));
			}
			PlayPosition::getInstance()->updateTempoTemp();
		}
	}

	/** Load Graph */
	if (!this->mainAudioGraph->parse(&(mes->graph()))) { return false; }

	return true;
}

std::unique_ptr<google::protobuf::Message> AudioCore::serialize() const {
	auto mes = std::make_unique<vsp4::Project>();

	/** Get Info */
	auto info = ProjectInfoData::getInstance()->serialize();
	if (!dynamic_cast<vsp4::ProjectInfo*>(info.get())) { return nullptr; }
	mes->set_allocated_info(dynamic_cast<vsp4::ProjectInfo*>(info.release()));

	/** Get Version */
	auto [majorVer, minorVer, patchVer] = utils::getAudioPlatformVersion();
	auto version = mes->mutable_version();
	version->set_major(majorVer);
	version->set_minor(minorVer);
	version->set_patch(patchVer);

	/** Get Tempo */
	{
		auto& tempos = PlayPosition::getInstance()->getTempoSequence();
		juce::MidiFile tempoFile;
		tempoFile.addTrack(tempos);

		juce::MemoryBlock tempoData;
		juce::MemoryOutputStream tempoStream(tempoData, false);
		tempoFile.writeTo(tempoStream);
		tempoStream.flush();
		
		mes->set_tempoevents(tempoData.getData(), tempoData.getSize());
	}

	/** Get Graph */
	auto graph = this->mainAudioGraph->serialize();
	if (!dynamic_cast<vsp4::MainGraph*>(graph.get())) { return nullptr; }
	mes->set_allocated_graph(dynamic_cast<vsp4::MainGraph*>(graph.release()));

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void AudioCore::initAudioDevice() {
	/** Init With Default Device */
	Device::getInstance()->initialise(1, 2, nullptr, true);

	/** Add Main Graph To Main Player */
	this->mainGraphPlayer->setProcessor(this->mainAudioGraph.get());

	/** Add Player To Default Device */
	Device::getInstance()->setAudioCallback(this->mainGraphPlayer.get());

	/** Update Audio Buses */
	this->updateAudioBuses();
}

void AudioCore::updateAudioBuses() {
	/** Link Audio Bus To Sequencer And Mixer */
	if (auto mainGraph = this->mainAudioGraph.get()) {
		/** Get Input Channel Num */
		auto inputChannelNum = Device::getInstance()->getAudioInputDeviceChannelNum();
		auto outputChannelNum = Device::getInstance()->getAudioOutputDeviceChannelNum();

		/** Set Buses Layout Of Main Graph */
		mainGraph->setAudioLayout(inputChannelNum, outputChannelNum);

		/** Change Main Graph SampleRate And Set Play Head */
		mainGraph->setPlayHead(PlayPosition::getInstance());
		mainGraph->prepareToPlay(
			Device::getInstance()->getAudioSampleRate(),
			Device::getInstance()->getAudioBufferSize());
	}

	/** Add MIDI Callback */
	Device::getInstance()->updateMidiCallback(this->mainGraphPlayer.get());

	/** Set MIDI Output */
	{
		auto midiOutput = Device::getInstance()->getDefaultMidiOutput();
		this->mainGraphPlayer->setMidiOutput(midiOutput);
	}

	/** Update Mackie Control Devices */
	this->mackieHub->removeUnavailableDevices();
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::getInstanceWithoutCreate() {
	return AudioCore::instance;
}

void AudioCore::releaseInstance() {
	if (AudioCore::instance) {
		delete AudioCore::instance;
		AudioCore::instance = nullptr;
	}
}

AudioCore* AudioCore::instance = nullptr;
