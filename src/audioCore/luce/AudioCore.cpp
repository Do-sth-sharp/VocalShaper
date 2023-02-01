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

	LUCE_METHOD_LIST(AudioCore);
	LUCE_STATIC_METHOD_LIST(AudioCore,
		getAllAudioDeviceList
	);

	LUCE_NEW(AudioCore) {
		return 0;
	}

	int loadAudioCore(lua_State* L) {
		LUCE_REG(L, AudioCore);
		return 0;
	}
}
