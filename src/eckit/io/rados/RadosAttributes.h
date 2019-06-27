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

#include <memory>
#include <string>
#include <map>

#include "eckit/utils/Translator.h"


namespace eckit {

class Stream;


class RadosAttributes  {
public:

    template<class T>
    void set(const std::string& name,  T value) {
        attrs_[name] = Translator<T, std::string>()(value);
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
