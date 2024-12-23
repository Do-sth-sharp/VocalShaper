#include "ActionBase.h"
#include "ActionDispatcher.h"

#include "../misc/Renderer.h"
#include "../source/SourceIO.h"
#include "../plugin/Plugin.h"
#include "../plugin/PluginLoader.h"
#include "../project/ProjectInfoData.h"
#include "../recovery/DataWrite.hpp"
#include "../ara/ARAGlobalState.h"

bool ActionUndoableBase::perform() {
	/** Perform */
	bool result = this->performInternal(false);

	/** Output */
	if (result) {
		this->outputInternal();
	}
	else {
		this->errorInternal();
	}

	/** Result */
	return result;
}

bool ActionUndoableBase::undo() {
	/** Perform */
	bool result = this->performInternal(true);

	/** Output */
	if (result) {
		this->outputInternal();
	}
	else {
		this->errorInternal();
	}

	/** Result */
	return result;
}

void ActionUndoableBase::output(const juce::String& mes) {
	this->outputTemp += mes;
	if (!mes.endsWithChar('\n')) {
		this->outputTemp += '\n';
	}
}

bool ActionUndoableBase::checkShield() {
	/** Get Shield Mask */
	int shield = this->getShieldMask();

	/** Check Rendering */
	if (shield & ShieldRendering) {
		if (Renderer::getInstance()->getRendering()) {
			this->output("Don't do this while rendering.");
			return false;
		}
	}

	/** Check Source IO */
	if (shield & ShieldSourceIO) {
		if (SourceIO::getInstance()->isThreadRunning()) {
			this->output("Don't do this while source IO running.");
			return false;
		}
	}

	/** Check Plugin Loading */
	if (shield & ShieldPluginLoad) {
		if (PluginLoader::getInstance()->isRunning()) {
			this->output("Don't do this while loading plugin.");
			return false;
		}
	}

	/** Check Plugin Scaning */
	if (shield & ShieldPluginScan) {
		if (Plugin::getInstance()->pluginSearchThreadIsRunning()) {
			this->output("Don't do this while scaning plugin.");
			return false;
		}
	}

	/** Check ARA Analysising */
	if (shield & ShieldARAAnalysis) {
		if (ARAGlobalState::hasSourceAnalysising()) {
			this->output("Don't do this while ARA analysising.");
			return false;
		}
	}

	return true;
}

void ActionUndoableBase::writeRecovery(bool isUndo) {
	/** Get Data */
	juce::MemoryBlock recoveryData;
	juce::MemoryOutputStream stream(recoveryData, false);
	this->getRecoveryData(stream);

	/** Write Head */
	auto actionCode = this->getActionType();
	if (isUndo) {
		writeRecoveryActionCodeUndo(actionCode);
	}
	else {
		writeRecoveryActionCodeDo(actionCode);
	}

	/** Write Data */
	writeRecoveryStringValue(
		(const char*)(recoveryData.getData()),
		recoveryData.getSize());
}

void ActionUndoableBase::writeRecoveryResult(bool succeed) {
	writeRecoveryActionResult(succeed);
}

void ActionUndoableBase::outputInternal() {
	ActionDispatcher::getInstance()->outputInternal(this->outputTemp);
	this->outputTemp.clear();
}

void ActionUndoableBase::errorInternal() {
	ActionDispatcher::getInstance()->errorInternal(this->outputTemp);
	this->outputTemp.clear();
}

bool ActionUndoableBase::performInternal(bool isUndo) {
	/** Output Execute Type */
	this->output("[" + juce::String{ isUndo ? "Undo" : "Do" } + "] " + this->getName());

	/** Check Shield */
	if (!this->checkShield()) {
		return false;
	}

	/** Unsave Project */
	if (this->getProjectChange()) {
		ProjectInfoData::getInstance()->unsave();
	}

	/** Write Recovery Data */
	if (this->getActionType() != ActionType::ActionNone) {
		this->writeRecovery(isUndo);
	}

	/** Perform */
	bool result = isUndo ? this->undoAction() : this->doAction();

	/** Write Recovery Result */
	if (this->getActionType() != ActionType::ActionNone) {
		this->writeRecoveryResult(result);
	}

	/** Get Status */
	auto statusStr = this->getStatusStr();
	this->output("Status: " + (statusStr.isNotEmpty() ? statusStr : juce::String{ "No Status" }));

	/** Result */
	this->output("Result: " + juce::String{ result ? "Succeed" : "Failed" });
	return result;
}
