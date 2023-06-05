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

#ifndef eckit_io_rados_RadosHandle_h
#define eckit_io_rados_RadosHandle_h

#include <memory>

#include "eckit/io/DataHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {


class RadosHandle : public eckit::DataHandle {

public:  // methods
    RadosHandle(const RadosObject&);
    RadosHandle(const std::string&);

    RadosHandle(Stream&);

    ~RadosHandle() override;

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

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
    Length estimate() override;

    void print(std::ostream&) const override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

private:  // members
    RadosObject object_;

    uint64_t offset_;
    bool opened_;
    bool write_;

    void open();

    static ClassSpec classSpec_;
    static Reanimator<RadosHandle> reanimator_;
};

}  // namespace eckit

#endif
