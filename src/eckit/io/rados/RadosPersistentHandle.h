/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
/// @date   March 2024

#pragma once

// #include <memory>

#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

/// @note: persists writes on flush by default, or on every write if requested

class RadosPersistentHandle : public eckit::RadosHandle {

public:  // methods
    RadosPersistentHandle(const RadosObject&, bool persist_on_write = false, size_t maxAioBuffSize = 1024 * 1024);

    // ~RadosPersistentHandle() override;

public:  // methods
    // Length openForRead() override;
    // void openForWrite(const Length&) override;
    // void openForAppend(const Length&) override;

    // long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    // void rewind() override;

    // Length size() override { return len_; }
    // Length estimate() override { return size(); }
    // Offset position() override { return offset_; }
    // Offset seek(const Offset&) override;
    // virtual bool canSeek() const override { return true; }

    void print(std::ostream&) const override;

private:  // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> comps_;

    bool persist_on_write_;
    size_t maxAioBuffSize_;

};

}  // namespace eckit
