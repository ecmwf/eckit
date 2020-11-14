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

#ifndef eckit_io_rados_RadosObject_h
#define eckit_io_rados_RadosObject_h

#include <memory>
#include <string>


namespace eckit {

class Stream;


class RadosObject {
public:
    RadosObject(Stream&);

    RadosObject(const std::string& path);
    RadosObject(const std::string& pool, const std::string& oid);

    RadosObject(const RadosObject& other, size_t part);

    const std::string& pool() const { return pool_; }
    const std::string& oid() const { return oid_; }
    std::string str() const;

private:
    std::string pool_;
    std::string oid_;

    void print(std::ostream&) const;
    void encode(Stream&) const;


    friend std::ostream& operator<<(std::ostream& s, const RadosObject& o) {
        o.print(s);
        return s;
    }

    friend Stream& operator<<(Stream& s, const RadosObject& o) {
        o.encode(s);
        return s;
    }
};


}  // namespace eckit

#endif
