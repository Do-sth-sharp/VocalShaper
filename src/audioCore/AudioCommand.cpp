#include "AudioCommand.h"
#include "AudioCore.h"
#include "CommandUtils.h"

AudioCommand::AudioCommand() {
	/** Init Lua State */
	this->cState = std::unique_ptr<lua_State, std::function<void(lua_State*)>>(
		luaL_newstate(), lua_close);

	/** Load Functions */
	lua_newtable(this->cState.get());

	regCommandAdd(this->cState.get());
	regCommandOther(this->cState.get());
	regCommandEcho(this->cState.get());
	regCommandList(this->cState.get());
	regCommandRemove(this->cState.get());
	regCommandSet(this->cState.get());
	regCommandLoad(this->cState.get());
	regCommandSave(this->cState.get());

	lua_setglobal(this->cState.get(), "AC");
}

const AudioCommand::CommandResult AudioCommand::processCommand(const juce::String& command) {
	/** Clear Result */
	lua_pushnil(this->cState.get());
	lua_setglobal(this->cState.get(), "sta");
	lua_pushnil(this->cState.get());
	lua_setglobal(this->cState.get(), "res");

	/** Do Command */
	if (luaL_dostring(this->cState.get(), command.toStdString().c_str())) {
		return AudioCommand::CommandResult{ false, command, juce::String::fromUTF8(luaL_checkstring(this->cState.get(), -1))};
	}

	/** Check Result */
	bool state = false;
	juce::String res = "Bad command return value!";
	if (lua_getglobal(this->cState.get(), "sta") == LUA_TBOOLEAN) {
		state = lua_toboolean(this->cState.get(), -1);
	}
	if (lua_getglobal(this->cState.get(), "res") == LUA_TSTRING) {
		res = juce::String::fromUTF8(lua_tostring(this->cState.get(), -1));
	}
	lua_pop(this->cState.get(), 2);

	return AudioCommand::CommandResult{ state, command, res };
}

void AudioCommand::processCommandAsync(const juce::String& command, AudioCommand::CommandCallback callback) {
	auto asyncFunc = [command, callback] {
		auto result = AudioCommand::getInstance()->processCommand(command);
		juce::MessageManager::callAsync([callback, result] { callback(result); });
	};
	juce::MessageManager::callAsync(asyncFunc);
}

AudioCommand* AudioCommand::getInstance() {
	return AudioCommand::instance ? AudioCommand::instance : (AudioCommand::instance = new AudioCommand());
}

AudioCommand* AudioCommand::instance = nullptr;
