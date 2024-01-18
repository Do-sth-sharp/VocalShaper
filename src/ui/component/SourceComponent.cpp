#include "SourceComponent.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/DragSourceType.h"
#include "../misc/CoreActions.h"
#include "../../audioCore/AC_API.h"

SourceComponent::SourceComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forSourceView());

	/** TODO Comp */
}

void SourceComponent::update(int index, bool selected) {
	this->selected = selected;
	if (index >= 0 && index < quickAPI::getSourceNum()) {
		this->index = index;
		this->type = (int)(quickAPI::getSourceType(index));
		this->name = quickAPI::getSourceName(index);
		this->typeName = quickAPI::getSourceTypeName(index);
		this->length = quickAPI::getSourceLength(index);
		this->channels = quickAPI::getSourceChannelNum(index);
		this->tracks = quickAPI::getSourceTrackNum(index);
		this->events = quickAPI::getSourceEventNum(index);
		this->synthesizer = quickAPI::getSourceSynthesizerName(index);
		this->sampleRate = quickAPI::getSourceSampleRate(index);
	}

	this->repaint();
}

bool SourceComponent::isInterestedInDragSource(
	const SourceDetails& dragSourceDetails) {
	auto& des = dragSourceDetails.description;

	if (this->type != quickAPI::SourceType::SynthSource) { return false; }

	if ((int)(des["type"]) != (int)(DragSourceType::Plugin)) { return false; }
	if (!des["instrument"]) { return false; }

	return true;
}

void SourceComponent::itemDropped(const SourceDetails& dragSourceDetails) {
	if (!this->isInterestedInDragSource(dragSourceDetails)) { return; }
	auto& des = dragSourceDetails.description;
	juce::String pid = des["id"].toString();

	CoreActions::setSourceSynthesizer(this->index, pid);
}
