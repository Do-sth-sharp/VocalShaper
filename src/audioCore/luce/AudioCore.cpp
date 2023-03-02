#include "AudioCore.h"
#include "../AudioCore.h"

namespace luce {
	LUCE_METHOD(getAllAudioInputDeviceList) {
		auto list = AudioCore::getAllAudioInputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(getAllAudioOutputDeviceList) {
		auto list = AudioCore::getAllAudioOutputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(setAudioInputDevice) {
		auto audioCore = AudioCore::getInstance();
		audioCore->setAudioInputDevice(luaL_checkstring(L, 1));
		return 0;
	}

	LUCE_METHOD(setAudioOutputDevice) {
		auto audioCore = AudioCore::getInstance();
		audioCore->setAudioOutputDevice(luaL_checkstring(L, 1));
		return 0;
	}

	LUCE_METHOD(getAudioInputDeviceName) {
		auto audioCore = AudioCore::getInstance();
		lua_pushstring(L, audioCore->getAudioInputDeviceName().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(getAudioOutputDeviceName) {
		auto audioCore = AudioCore::getInstance();
		lua_pushstring(L, audioCore->getAudioOutputDeviceName().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(setCurrentAudioDeviceType) {
		auto audioCore = AudioCore::getInstance();
		audioCore->setCurrentAudioDeviceType(luaL_checkstring(L, 1));
		return 0;
	}
	
	LUCE_METHOD(getCurrentAudioDeivceType) {
		auto audioCore = AudioCore::getInstance();
		lua_pushstring(L, audioCore->getCurrentAudioDeivceType().toStdString().c_str());
		return 1;
	}

	LUCE_METHOD(playTestSound) {
		auto audioCore = AudioCore::getInstance();
		audioCore->playTestSound();
		return 0;
	}

	LUCE_METHOD(getAllMIDIInputDeviceList) {
		auto list = AudioCore::getAllMIDIInputDeviceList();

		lua_newtable(L);

		for (int i = 0; i < list.size(); i++) {
			lua_pushinteger(L, static_cast<lua_Integer>(i) + 1);
			lua_pushstring(L, list.getReference(i).toStdString().c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	LUCE_METHOD(getAllMIDIOutputDeviceList) {
		auto list = AudioCore::getAllMIDIOutputDeviceList();

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
		getMIDIDebugger
	);

	LUCE_NEW(AudioCore) {
		return 0;
	}

	int loadAudioCore(lua_State* L) {
		LUCE_REG(L, AudioCore);
		return 0;
	}
}
