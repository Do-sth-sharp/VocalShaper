#include "ARAVirtualDocument.h"

ARAVirtualDocument::ARAVirtualDocument(
	SeqSourceProcessor* seq,
	ARA::Host::DocumentController* controller)
	: seq(seq), controller(controller) {}
