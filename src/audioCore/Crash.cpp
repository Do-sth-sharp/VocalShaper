#include "AudioCore.h"

extern "C" char* audioCoreCreateProjectDataBlock() {
	auto data = AudioCore::getInstance()->serialize(Serializable::createSerializeConfigQuickly());
	return (char*)(data.release());
}

extern "C" void audioCoreFreeProjectDataBlock(char* mem) {
	auto data = std::unique_ptr<google::protobuf::Message>((google::protobuf::Message*)mem);
}

extern "C" size_t audioCoreGetProjectDataSize(const char* mem) {
	if (auto data = (google::protobuf::Message*)mem) {
		return data->ByteSizeLong();
	}
	return 0;
}

extern "C" int audioCoreCopyProjectData(char* dst, size_t size, const char* mem) {
	if (auto data = (google::protobuf::Message*)mem) {
		return data->SerializeToArray(dst, size);
	}
	return 0;
}
