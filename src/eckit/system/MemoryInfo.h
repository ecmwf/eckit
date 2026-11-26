// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#pragma once

#include <iosfwd>

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

struct MemoryInfo {

    MemoryInfo();

    // mallinfo
    size_t arena_;
    // size_t ordblks_;
    // size_t smblks_;
    // size_t hblks_;
    size_t hblkhd_;
    size_t usmblks_;
    size_t fsmblks_;
    size_t uordblks_;
    size_t fordblks_;
    size_t keepcost_;

    // getrusage
    size_t resident_size_;
    size_t virtual_size_;

    // /proc/pid/smap
    size_t mapped_shared_;
    size_t mapped_read_;
    size_t mapped_write_;
    size_t mapped_execute_;
    size_t mapped_private_;

    // mmap, smgget

    size_t mmap_count_;
    size_t mmap_size_;

    size_t shm_count_;
    size_t shm_size_;


    void print(std::ostream&) const;
    void delta(std::ostream&, const MemoryInfo& other) const;


    friend std::ostream& operator<<(std::ostream& s, const MemoryInfo& m) {
        m.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
