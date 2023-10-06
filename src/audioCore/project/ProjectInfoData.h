#pragma once

#include <JuceHeader.h>
#include "Serializable.h"

class ProjectInfoData final : public Serializable,
	private juce::DeletedAtShutdown {
public:
	ProjectInfoData() = default;

	void init();
	void update();

public:
	bool parse(const google::protobuf::Message* data) override;
	std::unique_ptr<google::protobuf::Message> serialize() const override;

private:
	uint32_t createTime = 0, lastSavedTime = 0, spentTime = 0, lastUpdateTime = 0;
	juce::String createPlatform, lastSavedPlatform;
	juce::StringArray authors;

public:
	static ProjectInfoData* getInstance();
	static void releaseInstance();

private:
	static ProjectInfoData* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectInfoData)
};
