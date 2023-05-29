#include "SourceList.h"

int SourceList::match(double t) const {
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
	int result = this->binarySearch(sLow, sHigh, t);
	if (result > -1) {
		return this->lastIndex = result;
	}

	/** Seq Unexists */
	return -1;
}

const juce::CriticalSection& SourceList::getLock() const noexcept {
	return this->list.getLock();
}

int SourceList::binarySearch(int low, int high, double t) const {
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
