#include "DataWrite.hpp"

void writeRecoveryStringValue(const std::string& str) {
	writeRecoveryStringValue(str.c_str(), str.size());
}

bool writeRecoveryActionResult(bool result) {
	writeRecoveryBoolValue(result);
	return result;
}
