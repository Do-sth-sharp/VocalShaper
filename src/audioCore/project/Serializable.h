#pragma once

#include <JuceHeader.h>
#include <google/protobuf/message.h>
#include "SerializeConfig.h"

class Serializable {
public:
	Serializable() = default;
	virtual ~Serializable() = default;

	static SerializeConfig createSerializeConfigQuickly();
	static ParseConfig createParseConfigQuickly();

public:
	virtual bool parse(
		const google::protobuf::Message* data,
		const ParseConfig& config) = 0;
	virtual std::unique_ptr<google::protobuf::Message> serialize(
		const SerializeConfig& config) const = 0;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Serializable)
};
