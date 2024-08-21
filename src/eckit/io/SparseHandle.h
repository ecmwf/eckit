/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#pragma once

namespace eckit {
    
// The user promises not to read outside the bounds specified by the offsets and lengths.
// Raise out of bounds exception if the user tries to read outside the bounds.
// WIP WIP WIP
class SparseHandle : public DataHandle {
public:
    SparseHandle(std::unique_ptr<DataHandle> dh, OffsetList offsets, LengthList lengths):
        internal_(std::move(dh)),
        offsets_(std::move(offsets)),
        lengths_(std::move(lengths)) {
        ASSERT(offsets_.size() == lengths_.size());
        
    }

    DataHandle& internal() { return *internal_; }

    // -- Overridden methods

    long read(void* data, long size) override {
        NOTIMP;
    }

    Offset seek(const Offset& off) override {
        // simply move the position
        position_ = off;
        return position_;
    }
    bool canSeek() const override { return true; } // even if the internal is not seekable.

    Offset position() override { return position_; }

    void print(std::ostream& s) const override {
        s << "SparseHandle[ " << *internal_ << " ]";
    }


private:  // members

    std::unique_ptr<DataHandle> internal_;
    OffsetList offsets_;
    LengthList lengths_;

    Offset position_;

};

} // namespace eckit

// // TODO: This doesnt belong here.

// namespace eckit{

// // This could possible be a DataHandle, instead of a wrapper around it (or probably actually both).
// //   Almost certain needs to be a datahandle for it to be easily integrated into fdb/gribjump
// // A data handle that only allows access to certain offsets of the underlying data handle.
// class SparseHandle {
// private:
// public:
//     SparseHandle(DataHandle*, const std::vector<std::pair<Offset, Length>>&);
//     ~SparseHandle();

// private:
//     // Merge ranges into the existing ranges, taking special care
//     // to combine contiguous and overlapping ranges where possible.
//     void insert(const std::vector<std::pair<Offset, Length>>& ranges);

//     // Check that offset,length is in bounds
//     void validateRange(Offset offset, Length length);

//     void print(std::ostream& s) const override;

// private:
//     DataHandle* internal_; // Is it owning?
//     std::vector<std::pair<Offset, Length>> ranges_;
// };

// SparseHandle::SparseHandle(DataHandle* dh, const std::vector<std::pair<Offset, Length>>& ranges): 
//     internal_(dh) {
//     insert(ranges);
// }

// void SparseHandle::print(std::ostream& s) const {
//     s << "SparseHandle[ " << *internal_ << " ]";
// }

// void SparseHandle::insert(const std::vector<std::pair<Offset, Length>>& ranges){
//     if (ranges.empty()) return;

//     // Sort ranges by the start offset
//     std::vector<std::pair<size_t, size_t>> sorted_ranges = ranges;
//     std::sort(sorted_ranges.begin(), sorted_ranges.end());

//     // Merge overlapping or adjacent ranges
//     // TODO: This is AI logic, it's possibly lies.
//     ranges_.push_back(sorted_ranges[0]);
//     for (size_t i = 1; i < sorted_ranges.size(); ++i) {
//         Offset current_start = sorted_ranges[i].first;
//         Offset current_end = current_start + sorted_ranges[i].second;
//         Offset last_start = ranges_.back().first;
//         Offset last_end = last_start + ranges_.back().second;

//         if (current_start <= last_end) {
//             // Extend the last range
//             ranges_.back().second = std::max(last_end, current_end) - last_start;
//         } else {
//             // Add a new range
//             ranges_.emplace_back(current_start, current_end - current_start);
//         }
//     }
// }

// void SparseHandle::validateRange(Offset offset, Length length) {

//     for (const auto& range : ranges_) {
//         size_t range_start = range.first;
//         size_t range_end = range_start + range.second;

//         if (offset >= range_start && (offset + length) <= range_end) {
//             return; // or return true
//         }
//     }

//     if (!is_valid) {
//         throw OutOfRange("Operation is out of the allowed range."); // or return false
//     }
// }


// SparseHandle::~SparseHandle()
// {
// }

    
// } // namespace eckit
