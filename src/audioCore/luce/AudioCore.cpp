#include "AudioCore.h"
#include "../AudioCore.h"

namespace luce {
	LUCE_METHOD(getAllAudioDeviceList) {
		auto list = AudioCore::getAllAudioDeviceList();

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

	LUCE_METHOD_LIST(AudioCore);
	LUCE_STATIC_METHOD_LIST(AudioCore,
		getAllAudioDeviceList,
		setAudioInputDevice,
		setAudioOutputDevice,
		getAudioInputDeviceName,
		getAudioOutputDeviceName
	);

	LUCE_NEW(AudioCore) {
		return 0;
	}

	int loadAudioCore(lua_State* L) {
		LUCE_REG(L, AudioCore);
		return 0;
	}
}
