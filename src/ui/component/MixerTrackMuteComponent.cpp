#include "MixerTrackMuteComponent.h"
#include "../../audioCore/AC_API.h"

MixerTrackMuteComponent::MixerTrackMuteComponent() {}

void MixerTrackMuteComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->mute = quickAPI::getMixerTrackMute(index);

		this->repaint();
	}
}
