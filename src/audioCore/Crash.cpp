#include "AudioCore.h"

extern "C" char* audioCoreCreateProjectDataBlock() {
	auto data = AudioCore::getInstance()->serialize();
	return (char*)(data.release());
}

extern "C" void audioCoreFreeProjectDataBlock(char* mem) {
	auto data = std::unique_ptr<google::protobuf::Message>((google::protobuf::Message*)mem);
}

extern "C" int audioCoreGetProjectDataSize(const char* mem) {
	if (auto data = (google::protobuf::Message*)mem) {
		return data->ByteSizeLong();
	}
	return 0;
}

extern "C" int audioCoreCopyProjectData(char* dst, int size, const char* mem) {
	if (auto data = (google::protobuf::Message*)mem) {
		return data->SerializeToArray(dst, size);
	}
	return 0;
}
