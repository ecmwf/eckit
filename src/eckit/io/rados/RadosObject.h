// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
