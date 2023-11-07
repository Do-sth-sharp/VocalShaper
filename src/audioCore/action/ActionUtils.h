#pragma once

#include <JuceHeader.h>

#include "../misc/Renderer.h"
#include "../source/AudioIOList.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"


#define ACTION_CHECK_RENDERING(s) \
	do { \
		if(Renderer::getInstance()->getRendering()) { \
			this->output(s); \
			return false; \
		} \
	} while(false)

#define ACTION_CHECK_SOURCE_IO_RUNNING(s) \
	do { \
		if(AudioIOList::getInstance()->isThreadRunning()) { \
			this->output(s); \
			return false; \
		} \
	} while(false)

#define ACTION_CHECK_PLUGIN_LOADING(s) \
	do { \
		if(PluginLoader::getInstance()->isRunning()) { \
			this->output(s); \
			return false; \
		} \
	} while(false)

#define ACTION_CHECK_PLUGIN_SEARCHING(s) \
	do { \
		if(Plugin::getInstance()->pluginSearchThreadIsRunning()) { \
			this->output(s); \
			return false; \
		} \
	} while(false)
