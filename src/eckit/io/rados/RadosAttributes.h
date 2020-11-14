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

#ifndef eckit_io_rados_RadosAttributes_h
#define eckit_io_rados_RadosAttributes_h

#include <map>
#include <memory>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/utils/Translator.h"


namespace eckit {

class Stream;


class RadosAttributes {
public:
    template <class T>
    void set(const std::string& name, T value) {
        attrs_[name] = Translator<T, std::string>()(value);
    }


    void set(const std::string& name, const Length& value) { set(name, static_cast<long long>(value)); }

    void set(const std::string& name, const Offset& value) { set(name, static_cast<long long>(value)); }

    template <class T>
    bool get(const std::string& name, T& value) const {
        auto j = attrs_.find(name);
        if (j != attrs_.end()) {
            value = Translator<std::string, T>()((*j).second);
            return true;
        }
        return false;
    }

    bool get(const std::string& name, Length& value) {
        long long v;
        if (!get(name, v)) {
            return false;
        }
        value = Length(v);
        return true;
    }

    bool get(const std::string& name, Offset& value) {
        long long v;
        if (!get(name, v)) {
            return false;
        }
        value = Offset(v);
        return true;
    }

    const std::map<std::string, std::string>& attrs() const { return attrs_; }

private:
    std::map<std::string, std::string> attrs_;

    void print(std::ostream&) const;


    friend std::ostream& operator<<(std::ostream& s, const RadosAttributes& o) {
        o.print(s);
        return s;
    }
};


}  // namespace eckit

#endif
