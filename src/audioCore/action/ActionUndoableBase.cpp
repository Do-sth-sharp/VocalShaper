#include "ActionUndoableBase.h"

bool ActionUndoableBase::perform() {
	return this->doAction();
}
