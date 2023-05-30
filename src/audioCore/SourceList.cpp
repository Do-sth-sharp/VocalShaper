#include "SourceList.h"

int SourceList::match(double t) const {
	/** Empty */
	if (this->list.isEmpty()) { return -1; }

	/** Search Range */
	int sLow = 0, sHigh = this->list.size();

	/** Try To Use Last Index */
	if (this->lastIndex >= 0 && this->lastIndex < this->list.size()) {
		/** Try Last Index */
		auto& lastSeq = this->list.getReference(this->lastIndex);
		if (std::get<0>(lastSeq) <= t && std::get<1>(lastSeq) > t) {
			/** Last Index Match */
			return this->lastIndex;
		}
		else if (std::get<1>(lastSeq) <= t) {
			/** After Last Index */
			if (this->lastIndex < this->list.size() - 1) {
				/** Try Next Index */
				auto& nextSeq = this->list.getReference(this->lastIndex + 1);
				if (std::get<0>(nextSeq) <= t && std::get<1>(nextSeq) > t) {
					/** Next Index Match */
					return ++(this->lastIndex);
				}
				else if (std::get<1>(nextSeq) <= t) {
					/** After Next Index */
					sLow = this->lastIndex + 1;
				}
				else {
					/** Between Last Index And Next */
					return -1;
				}
			}
			else {
				/** Last Index Is End */
				return -1;
			}
		}
		else {
			/** Before Last Index */
			sHigh = this->lastIndex;
		}
	}

	/** Binary Search */
	int result = this->binarySearchFast(sLow, sHigh, t);
	if (result > -1) {
		return this->lastIndex = result;
	}

	/** Seq Unexists */
	return -1;
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

const juce::CriticalSection& SourceList::getLock() const noexcept {
	return this->list.getLock();
}

int SourceList::binarySearchFast(int low, int high, double t) const {
	while (low <= high) {
		int mid = low + (high - low) / 2;

		auto& current = this->list.getReference(mid);
		if (t < std::get<0>(current)) {
			high = mid - 1;
			continue;
		}
		else if (t >= std::get<1>(current)) {
			low = mid + 1;
			continue;
		}
		return mid;
	}
	
	return -1;
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
