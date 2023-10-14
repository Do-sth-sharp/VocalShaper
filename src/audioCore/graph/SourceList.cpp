#include "SourceList.h"
#include "../source/CloneableSourceManager.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

std::tuple<int, int> SourceList::match(double startTime, double endTime) const {
	/** Empty */
	if (this->list.isEmpty()) { return { -1, -1 }; }

	/** Search Range */
	int sLow = 0, sHigh = this->list.size() - 1;

	/** Result Var */
	int start = this->lastIndex;

	do {
		/** Try To Use Last Index */
		if (this->lastIndex >= 0 && this->lastIndex < this->list.size()) {
			/** Try Last Index */
			auto& lastSeq = this->list.getReference(this->lastIndex);
			if (std::get<0>(lastSeq) <= startTime && std::get<1>(lastSeq) > endTime) {
				/** Last Index Match */
				break;
			}
			else if (std::get<1>(lastSeq) <= startTime) {
				/** After Last Index */
				if (this->lastIndex < this->list.size() - 1) {
					/** Try Next Index */
					auto& nextSeq = this->list.getReference(this->lastIndex + 1);
					if (std::get<1>(lastSeq) <= startTime && std::get<1>(nextSeq) > startTime) {
						/** Next Index Match */
						start = this->lastIndex + 1;
						break;
					}
					else if (std::get<1>(nextSeq) <= startTime) {
						/** After Next Index */
						sLow = this->lastIndex + 1;
					}
				}
				else {
					/** Last Index Is End */
					return { -1, -1 };
				}
			}
			else {
				/** Before Last Index */
				sHigh = this->lastIndex;
			}
		}

		/** Binary Search */
		start = this->binarySearchStart(sLow, sHigh, startTime);
		if (start > -1) {
			break;
		}

		/** Seq Unexists */
		return { -1, -1 };
	} while (false);
	
	/** Found Result */
	int end = this->seqSearchEnd(start, this->list.size() - 1, endTime);

	if (end <= -1) { return { -1, -1 }; }
	return { start, this->lastIndex = end };
}

const SourceList::SeqBlock SourceList::get(int index) const {
	return this->list[index];
}

const SourceList::SeqBlock SourceList::getUnchecked(int index) const {
	return this->list.getUnchecked(index);
}

const SourceList::SeqBlock& SourceList::getReference(int index) const {
	return this->list.getReference(index);
}

int SourceList::size() const {
	juce::GenericScopedLock locker(this->getLock());
	return this->list.size();
}

bool SourceList::add(const SourceList::SeqBlock& block) {
	/** Get Lock */
	juce::GenericScopedLock locker(this->getLock());

	/** Get Insert Place */
	int index = this->list.isEmpty()
		? 0 : this->binarySearchInsert(0, this->list.size() - 1, std::get<0>(block));
	if (index < 0) { jassertfalse; return false; }

	/** Check Overlap */
	if ((index - 1) < this->list.size() && (index - 1) >= 0) {
		auto& last = this->list.getReference(index - 1);
		if (std::get<0>(block) < std::get<1>(last)) { return false; }
	}
	if (index < this->list.size() && index >= 0) {
		auto& next = this->list.getReference(index);
		if (std::get<0>(next) < std::get<1>(block)) { return false; }
	}

	/** Insert Block */
	this->list.insert(index, block);

	return true;
}

void SourceList::remove(int index) {
	juce::GenericScopedLock locker(this->getLock());
	if (index >= 0 && index < this->list.size()) {
		this->list.remove(index);
	}
}

const juce::CriticalSection& SourceList::getLock() const noexcept {
	return this->list.getLock();
}

void SourceList::clearGraph() {
	juce::GenericScopedLock locker(this->getLock());
	this->list.clear();
	this->lastIndex = -1;
}

bool SourceList::parse(const google::protobuf::Message* data) {
	auto mes = dynamic_cast<const vsp4::SourceInstanceList*>(data);
	if (!mes) { return false; }

	auto& list = mes->sources();
	for (auto& i : list) {
		if (auto ptrSrc = CloneableSourceManager::getInstance()->getSource(i.index())) {
			this->add({ i.startpos(), i.endpos(), i.offset(), ptrSrc, i.index() });
		}
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> SourceList::serialize() const {
	auto mes = std::make_unique<vsp4::SourceInstanceList>();

	juce::GenericScopedLock locker(this->getLock());
	auto list = mes->mutable_sources();
	for (auto& [startTime, endTime, offset, ptr, index] : this->list) {
		auto instance = std::make_unique<vsp4::SeqSourceInstance>();
		instance->set_index(index);
		instance->set_startpos(startTime);
		instance->set_endpos(endTime);
		instance->set_offset(offset);
		list->AddAllocated(instance.release());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

int SourceList::binarySearchInsert(int low, int high, double t) const {
	while (low <= high) {
		int mid = low + (high - low) / 2;

		auto& current = this->list.getReference(mid);
		if (mid == low) {
			if (t < std::get<0>(current)) {
				return 0;
			}
		}
		if (mid == high) {
			if (t >= std::get<0>(current)) {
				return high + 1;
			}
		}

		auto& next = this->list.getReference(mid + 1);
		if (t >= std::get<0>(current) && t < std::get<0>(next)) {
			return mid + 1;
		}
		else if (t < std::get<0>(current)) {
			high = mid - 1;
			continue;
		}
		else {
			low = mid + 1;
			continue;
		}
	}

	return -1;
}

int SourceList::binarySearchStart(int low, int high, double t) const {
	while (low <= high) {
		int mid = low + (high - low) / 2;

		auto& current = this->list.getReference(mid);
		if (mid == low) {
			if (t < std::get<1>(current)) {
				return low;
			}
		}
		if (mid == high) {
			if (t >= std::get<0>(current) && t < std::get<1>(current)) {
				return high;
			}
			else if (t >= std::get<1>(current)) {
				return -1;
			}
		}

		auto& next = this->list.getReference(mid + 1);
		if (t >= std::get<0>(current) && t < std::get<0>(next)) {
			return mid + 1;
		}
		else if (t < std::get<0>(current)) {
			high = mid - 1;
			continue;
		}
		else {
			low = mid + 1;
			continue;
		}
	}

	return -1;
}

int SourceList::seqSearchEnd(int low, int high, double t) const {
	for (int i = low; i <= high; i++) {
		auto& current = this->list.getReference(i);

		if (i == low && t < std::get<0>(current)) {
			return -1;
		}
		if (i == high && t >= std::get<0>(current)) {
			return i;
		}

		auto& next = this->list.getReference(i + 1);
		if (t >= std::get<0>(current) && t < std::get<0>(next)) {
			return i;
		}
	}

	return -1;
}
