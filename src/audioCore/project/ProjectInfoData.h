#pragma once

#include <JuceHeader.h>
#include <stack>
#include "Serializable.h"

class ProjectInfoData final : public Serializable,
	private juce::DeletedAtShutdown {
public:
	ProjectInfoData() = default;

	void init();
	void update();

	void push();
	void pop();
	void release();

	bool checkSaved() const;
	void unsave();

public:
	bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) override;
	std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const override;

private:
	struct Content final {
		uint32_t createTime = 0, lastSavedTime = 0, spentTime = 0, lastUpdateTime = 0;
		juce::String createPlatform, lastSavedPlatform;
		juce::StringArray authors;
		bool saved = true;
	} content;
	std::stack<Content> traceback;

public:
	static ProjectInfoData* getInstance();
	static void releaseInstance();

private:
	static ProjectInfoData* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectInfoData)
};
