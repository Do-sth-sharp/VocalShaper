#pragma once

#include <JuceHeader.h>

#include "../misc/Renderer.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../project/ProjectInfoData.h"
#include "../recovery/DataWrite.hpp"
#include "../recovery/ActionType.hpp"

#define ACTION_CHECK_RENDERING(s) \
	do { \
		if(Renderer::getInstance()->getRendering()) { \
			this->error(s); \
			return false; \
		} \
	} while(false)

#define ACTION_CHECK_SOURCE_IO_RUNNING(s) \
	do { \
		/** TODO */ \
	} while(false)

#define ACTION_CHECK_PLUGIN_LOADING(s) \
	do { \
		if(PluginLoader::getInstance()->isRunning()) { \
			this->error(s); \
			return false; \
		} \
	} while(false)

#define ACTION_CHECK_PLUGIN_SEARCHING(s) \
	do { \
		if(Plugin::getInstance()->pluginSearchThreadIsRunning()) { \
			this->error(s); \
			return false; \
		} \
	} while(false)

#define ACTION_UNSAVE_PROJECT() \
	do { \
		ProjectInfoData::getInstance()->unsave(); \
	} while(false)

#define ACTION_DATABLOCK struct DataBlock
#define ACTION_DB _data

#define ACTION_WRITE_TYPE(t) writeRecoveryActionCodeDo(ActionType::t)
#define ACTION_WRITE_TYPE_UNDO(t) writeRecoveryActionCodeUndo(ActionType::t)
#define ACTION_WRITE_DB() writeRecoveryBlockValue(this->ACTION_DB)
#define ACTION_WRITE_STRING(s) writeRecoveryStringValue(this->ACTION_DB.s.toStdString())
#define ACTION_RESULT(r) return writeRecoveryActionResult(r)
#define ACTION_DATA(n) this->ACTION_DB.n
