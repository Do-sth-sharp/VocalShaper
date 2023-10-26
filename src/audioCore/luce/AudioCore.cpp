#include "AudioCore.h"
#include "../AudioCore.h"
#include "../misc/Device.h"
#include <google/protobuf/message_lite.h>

namespace luce {
	LUCE_METHOD(getAllAudioInputDeviceList) {
		auto list = Device::getAllAudioInputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(getAllAudioOutputDeviceList) {
		auto list = Device::getAllAudioOutputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(setAudioInputDevice) {
		Device::getInstance()->setAudioInputDevice(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(setAudioOutputDevice) {
		Device::getInstance()->setAudioOutputDevice(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(getAudioInputDeviceName) {
		lua_pushstring(L, Device::getInstance()->getAudioInputDeviceName().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(getAudioOutputDeviceName) {
		lua_pushstring(L, Device::getInstance()->getAudioOutputDeviceName().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(setCurrentAudioDeviceType) {
		Device::getInstance()->setCurrentAudioDeviceType(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}
	
	LUCE_METHOD(getCurrentAudioDeivceType) {
		lua_pushstring(L, Device::getInstance()->getCurrentAudioDeivceType().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(playTestSound) {
		Device::getInstance()->playTestSound();
		return 0;
	}

	LUCE_METHOD(getAllMIDIInputDeviceList) {
		auto list = Device::getAllMIDIInputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(getAllMIDIOutputDeviceList) {
		auto list = Device::getAllMIDIOutputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(getAudioDebugger) {
		auto audioCore = AudioCore::getInstance();
		auto ptrDebuggger = audioCore->getAudioDebugger();

		using juce::Component;
		LUCE_PUSH_USERDATA(L, Component, debugger, (*ptrDebuggger));

		return 1;
	}

	LUCE_METHOD(getMIDIDebugger) {
		auto audioCore = AudioCore::getInstance();
		auto ptrDebuggger = audioCore->getMIDIDebugger();

		using juce::Component;
		LUCE_PUSH_USERDATA(L, Component, debugger, (*ptrDebuggger));

		return 1;
	}

	LUCE_METHOD(setPluginSearchPathListFilePath) {
		AudioConfig::setPluginSearchPathListFilePath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(setPluginListTemporaryFilePath) {
		AudioConfig::setPluginListTemporaryFilePath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(setPluginBlackListFilePath) {
		AudioConfig::setPluginBlackListFilePath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(setDeadPluginListPath) {
		AudioConfig::setDeadPluginListPath(juce::String::fromUTF8(luaL_checkstring(L, 1)));
		return 0;
	}

	LUCE_METHOD(shutdown) {
		AudioCore::releaseInstance();
		google::protobuf::ShutdownProtobufLibrary();
		return 0;
	}

	LUCE_METHOD_LIST(AudioCore);
	LUCE_STATIC_METHOD_LIST(AudioCore,
		getAllAudioInputDeviceList,
		getAllAudioOutputDeviceList,
		setAudioInputDevice,
		setAudioOutputDevice,
		getAudioInputDeviceName,
		getAudioOutputDeviceName,
		setCurrentAudioDeviceType,
		getCurrentAudioDeivceType,
		playTestSound,
		getAllMIDIInputDeviceList,
		getAllMIDIOutputDeviceList,
		getAudioDebugger,
		getMIDIDebugger,
		setPluginSearchPathListFilePath,
		setPluginListTemporaryFilePath,
		setPluginBlackListFilePath,
		setDeadPluginListPath,
		shutdown
	);

	LUCE_NEW(AudioCore) {
		return 0;
	}

	int loadAudioCore(lua_State* L) {
		LUCE_REG(L, AudioCore);
		return 0;
	}
}
