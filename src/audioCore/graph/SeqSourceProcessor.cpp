#include "SeqSourceProcessor.h"
#include "../misc/PlayPosition.h"
#include "../source/CloneableSourceManager.h"
#include "../source/CloneableAudioSource.h"
#include "../source/CloneableMIDISource.h"
#include "../source/CloneableSynthSource.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

SeqSourceProcessor::SeqSourceProcessor(const juce::AudioChannelSet& type)
	: audioChannels(type) {
	/** Set Channel Layout */
	this->setChannelLayoutOfBus(true, 0, type);
	this->setChannelLayoutOfBus(false, 0, type);
}

int SeqSourceProcessor::addSeq(const SourceList::SeqBlock& block) {
	return this->srcs.add(block);
}

void SeqSourceProcessor::removeSeq(int index) {
	this->srcs.remove(index);
}

int SeqSourceProcessor::getSeqNum() const {
	return this->srcs.size();
}

const SourceList::SeqBlock SeqSourceProcessor::getSeq(int index) const {
	juce::GenericScopedLock locker(this->srcs.getLock());
	return this->srcs.get(index);
}

void SeqSourceProcessor::setTrackName(const juce::String& name) {
	this->trackName = name;
}

const juce::String SeqSourceProcessor::getTrackName() const {
	return this->trackName;
}

void SeqSourceProcessor::setTrackColor(const juce::Colour& color) {
	this->trackColor = color;
}

const juce::Colour SeqSourceProcessor::getTrackColor() const {
	return this->trackColor;
}

void SeqSourceProcessor::closeAllNote() {
	this->noteCloseFlag = true;
}

void SeqSourceProcessor::prepareToPlay(
	double /*sampleRate*/, int /*maximumExpectedSamplesPerBlock*/) {}

void SeqSourceProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Buffer Is Empty */
	if (buffer.getNumSamples() <= 0) { return; }

	/** Clear Audio Channel */
	auto dspBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(
		0, buffer.getNumChannels());
	dspBlock.fill(0);

	/** Clear MIDI Buffer */
	midiMessages.clear();

	/** Close Note */
	if (this->noteCloseFlag) {
		this->noteCloseFlag = false;

		for (auto& i : this->activeNoteSet) {
			midiMessages.addEvent(
				juce::MidiMessage::noteOff(std::get<0>(i), std::get<1>(i)), 0);
		}
	}

	/** Get Play Head */
	auto playHead = dynamic_cast<PlayPosition*>(this->getPlayHead());
	if (!playHead) { return; }

	/** Get Current Position */
	juce::Optional<juce::AudioPlayHead::PositionInfo> position = playHead->getPosition();
	if (!position) { return; }

	/** Check Play State */
	if (!position->getIsPlaying()) { return; }

	/** Get Time */
	double startTime = position->getTimeInSeconds().orFallback(-1);
	double sampleRate = this->getSampleRate();
	double duration = buffer.getNumSamples() / sampleRate;
	double endTime = startTime + duration;

	int startTimeInSample = position->getTimeInSamples().orFallback(-1);
	int durationInSample = buffer.getNumSamples();
	int endTimeInSample = startTimeInSample + durationInSample;

	/** Copy Source Data */
	{
		juce::ScopedTryReadLock managerLocker(CloneableSourceManager::getInstance()->getLock());
		if (managerLocker.isLocked()) {
			juce::GenericScopedLock srcLocker(this->srcs.getLock());

			/** Find Hot Block */
			auto index = this->srcs.match(startTime, endTime);

			for (int i = std::get<0>(index);
				i <= std::get<1>(index) && i < this->srcs.size() && i >= 0; i++) {
				/** Get Block */
				auto [blockStartTime, blockEndTime, blockOffset, blockPointer, blockIndex] = this->srcs.getUnchecked(i);
				int blockStartTimeInSample = blockStartTime * sampleRate;
				int blockEndTimeInSample = blockEndTime * sampleRate;
				int blockOffsetInSample = blockOffset * sampleRate;

				if (CloneableSource::SafePointer<> ptr = blockPointer) {
					/** Caculate Time */
					int blockLengthInSample = ptr->getSourceLength() * sampleRate;
					int dataStartTimeInSample = blockStartTimeInSample + std::max(blockOffsetInSample, 0);
					int dataEndTimeInSample =
						std::min(blockEndTimeInSample, blockStartTimeInSample + blockOffsetInSample + blockLengthInSample);

					if (dataEndTimeInSample > dataStartTimeInSample) {
						int hotStartTimeInSample = std::max(startTimeInSample, dataStartTimeInSample);
						int hotEndTimeInSample = std::min(endTimeInSample, dataEndTimeInSample);

						if (hotEndTimeInSample > hotStartTimeInSample) {
							int dataTimeInSample = blockStartTimeInSample + blockOffsetInSample;
							int bufferOffsetInSample = hotStartTimeInSample - startTimeInSample;
							int hotOffsetInSample = hotStartTimeInSample - dataTimeInSample;
							int hotLengthInSample = hotEndTimeInSample - hotStartTimeInSample;

							if (auto p = dynamic_cast<CloneableAudioSource*>(ptr.getSource())) {
								/** Copy Audio Data */
								p->readData(buffer, bufferOffsetInSample, hotOffsetInSample, hotLengthInSample);
							}
							else if (auto p = dynamic_cast<CloneableSynthSource*>(ptr.getSource())) {
								/** Copy Audio Data */
								p->readData(buffer, bufferOffsetInSample, hotOffsetInSample, hotLengthInSample);
							}
							else if (auto p = dynamic_cast<CloneableMIDISource*>(ptr.getSource())) {
								/** Copy MIDI Message */
								p->readData(midiMessages, 
									dataTimeInSample - startTimeInSample,
									hotOffsetInSample, 
									hotEndTimeInSample - dataTimeInSample);
							}
						}
					}
				}
			}
		}
	}

	/** Set Note State */
	for (auto i : midiMessages) {
		auto mes = i.getMessage();
		if (mes.isNoteOn(false)) {
			this->activeNoteSet.insert({ mes.getChannel(), mes.getNoteNumber() });
		}
		else if (mes.isNoteOff(true)) {
			this->activeNoteSet.erase({ mes.getChannel(), mes.getNoteNumber() });
		}
	}
}

double SeqSourceProcessor::getTailLengthSeconds() const {
	juce::GenericScopedLock locker(this->srcs.getLock());

	int size = this->srcs.size();
	return (size > 0) ? std::get<1>(this->srcs.getUnchecked(size - 1)) : 0;
}

void SeqSourceProcessor::clearGraph() {
	this->setTrackName(juce::String{});
	this->setTrackColor(juce::Colour{});

	this->srcs.clearGraph();
}

bool SeqSourceProcessor::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::SeqTrack*>(data);
	if (!mes) { return false; }

	auto& info = mes->info();
	this->setTrackName(info.name());
	this->setTrackColor(juce::Colour{ info.color() });

	auto& sources = mes->sources();
	if (!this->srcs.parse(&sources)) { return false; }

	return true;
}

std::unique_ptr<google::protobuf::Message> SeqSourceProcessor::serialize() const {
	auto mes = std::make_unique<vsp4::SeqTrack>();

	mes->set_type(static_cast<vsp4::TrackType>(utils::getTrackType(this->audioChannels)));
	auto info = mes->mutable_info();
	info->set_name(this->getTrackName().toStdString());
	info->set_color(this->getTrackColor().getARGB());

	auto srcs = this->srcs.serialize();
	if (!dynamic_cast<vsp4::SourceInstanceList*>(srcs.get())) { return nullptr; }
	mes->set_allocated_sources(dynamic_cast<vsp4::SourceInstanceList*>(srcs.release()));

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}
