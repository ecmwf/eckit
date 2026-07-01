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
/// @author Nicolau Manubens
/// @date June 2019

#pragma once

#include <memory>
#include <string>

#include "eckit/io/rados/RadosNamespace.h"

namespace eckit {

// class Stream;


class RadosObject {
public:
    // RadosObject(Stream&);

    RadosObject(const eckit::URI&);
    RadosObject(const std::string& pool, const std::string& nspace, const std::string& oid);

    RadosObject(const RadosObject& other, size_t part);

    const eckit::RadosNamespace& nspace() const { return ns_; }
    const std::string& name() const { return oid_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const;

    bool exists() const;
    void ensureDestroyed();
    void ensureAllDestroyed();

    eckit::DataHandle* dataHandle() const;
    eckit::DataHandle* asyncDataHandle(size_t maxAioBuffSize = 1024 * 1024) const;
    eckit::DataHandle* rangeReadHandle(const eckit::Offset&, const eckit::Length&) const;
    eckit::DataHandle* multipartWriteHandle(const eckit::Length& maxPartSize = 0) const;
    eckit::DataHandle* asyncMultipartWriteHandle(const eckit::Length& maxPartSize = 0, 
        size_t maxAioBuffSize = 1024, size_t maxHandleBuffSize = 1024) const;
    eckit::DataHandle* multipartRangeReadHandle(const eckit::Offset&, const eckit::Length&) const;

private:
    eckit::RadosNamespace ns_;
    std::string oid_;

    // void print(std::ostream&) const;
    // void encode(Stream&) const;

    // friend std::ostream& operator<<(std::ostream& s, const RadosObject& o) {
    //     o.print(s);
    //     return s;
    // }

    // friend Stream& operator<<(Stream& s, const RadosObject& o) {
    //     o.encode(s);
    //     return s;
    // }
};


}  // namespace eckit
