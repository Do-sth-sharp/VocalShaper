#pragma once

#include <JuceHeader.h>
#include "AudioCore.h"
#include <lua.hpp>

class CommandBase : private juce::DeletedAtShutdown {
	JUCE_LEAK_DETECTOR(CommandBase);

protected:
	using CommandFuncResult = std::tuple<bool, juce::String>;
	virtual CommandFuncResult func(AudioCore*, lua_State*) = 0;

	static juce::AudioDeviceManager* getAudioDeviceManager(AudioCore* ac) {
		return ac->audioDeviceManager.get();
	};

	int interface(lua_State* L) {
		if (!L) { return 0; };
		AudioCore* ac = AudioCore::getInstance();

		bool sta = false;
		juce::String res;

		std::tie(sta, res) = this->func(ac, L);

		if (res.isNotEmpty() && res.getLastCharacter() != '\n') {
			res += '\n';
		}

		juce::String output;
		lua_getglobal(L, "res");
		if (lua_isstring(L, -1)) {
			output = juce::String::fromUTF8(lua_tostring(L, -1));
		}
		lua_pop(L, 1);
		bool state = true;
		lua_getglobal(L, "sta");
		if (lua_isboolean(L, -1)) {
			state = lua_toboolean(L, -1);
		}
		lua_pop(L, 1);

		output += res;
		state = state && sta;

		if (!sta) {
			lua_pushstring(L, output.toStdString().c_str());
			lua_error(L);
		}

		lua_pushboolean(L, state);
		lua_setglobal(L, "sta");
		lua_pushstring(L, output.toStdString().c_str());
		lua_setglobal(L, "res");
		return 0;
	};
};

#define AUDIOCORE_FUNC(n) \
	class n##Class final : public CommandBase { \
		JUCE_LEAK_DETECTOR(n##Class); \
		\
	private: \
		CommandFuncResult func(AudioCore*, lua_State*) override; \
		static n##Class* instance; \
		static n##Class* getInstance(){ \
			return n##Class::instance ? n##Class::instance : (n##Class::instance = new n##Class()); \
		} \
		\
	public: \
		static int lFunc(lua_State* L) { \
			return n##Class::getInstance()->interface(L); \
		}; \
	}; \
	\
	n##Class* n##Class::instance = nullptr; \
	\
	CommandBase::CommandFuncResult n##Class::func(AudioCore* audioCore, lua_State* L)

#define LUA_PUSH_AUDIOCORE_FUNC(L, n) \
	lua_pushcfunction(L, n##Class::lFunc)

#define LUA_ADD_AUDIOCORE_FUNC(L, s, n) \
	lua_pushstring(L, s); \
	LUA_PUSH_AUDIOCORE_FUNC(L, n); \
	lua_settable(L, -3)

#define LUA_ADD_AUDIOCORE_FUNC_DEFAULT_NAME(L, n) LUA_ADD_AUDIOCORE_FUNC(L, #n, n)

void regCommandAdd(lua_State* L);
void regCommandClear(lua_State* L);
void regCommandEcho(lua_State* L);
void regCommandList(lua_State* L);
void regCommandRemove(lua_State* L);
void regCommandSearch(lua_State* L);
void regCommandSet(lua_State* L);
void regCommandLoad(lua_State* L);
void regCommandSave(lua_State* L);
