/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2019

#pragma once

#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {


class RadosMultiObjWriteHandle : public eckit::DataHandle {

public:  // methods
    RadosMultiObjWriteHandle(const RadosObject&, bool persistent = false, const Length& maxObjectSize = 0, 
        size_t maxAioBuffSize = 1024, size_t maxHandleBuffSize = 1024);
    // RadosMultiObjWriteHandle(const eckit::URI&, const Length& maxObjectSize = 0);
    // RadosMultiObjWriteHandle(Stream&);

    ~RadosMultiObjWriteHandle() override;

    // -- Class methods

    // static const ClassSpec& classSpec() { return classSpec_; }

    std::string title() const;

public:  // methods
    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    void rewind() override;

    Offset position() override;

    void print(std::ostream&) const override;

    // From Streamable

    // void encode(Stream&) const override;
    // const ReanimatorBase& reanimator() const override { return reanimator_; }

private:  // members
    RadosObject object_;

    bool persistent_;
    Length maxObjectSize_;
    size_t maxAioBuffSize_;
    size_t maxHandleBuffSize_;

    size_t written_;
    Offset position_;
    size_t part_;
    bool opened_;

    std::vector<std::unique_ptr<DataHandle>> handles_;


    // static ClassSpec classSpec_;
    // static Reanimator<RadosMultiObjWriteHandle> reanimator_;
};

}  // namespace eckit
