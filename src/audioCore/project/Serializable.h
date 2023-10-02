#pragma once

#include <JuceHeader.h>
#include <google/protobuf/message.h>

class Serializable {
public:
	Serializable() = default;
	virtual ~Serializable() = default;

public:
	virtual bool parse(const google::protobuf::Message* data) = 0;
	virtual std::unique_ptr<google::protobuf::Message> serialize() const = 0;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Serializable)
};
