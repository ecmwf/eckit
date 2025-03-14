/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_value_CompositeParams_H
#define eckit_value_CompositeParams_H

#include "eckit/value/Params.h"

//----------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------

class CompositeParams {

public:  // methods

    CompositeParams();
    CompositeParams(const Params::List&);
    CompositeParams(Stream&);

    CompositeParams& push_front(const Params& p);
    CompositeParams& push_back(const Params& p);

    CompositeParams& pop_front() {
        plist_.pop_front();
        return *this;
    }
    CompositeParams& pop_back() {
        plist_.pop_back();
        return *this;
    }

    static const char* className() { return "eckit::CompositeParams"; }

private:  // methods

    friend Params::value_t getValue(const CompositeParams& p, const Params::key_t& key);
    friend void print(const CompositeParams& p, std::ostream& s);
    friend void encode(const CompositeParams& p, Stream& s);

private:  // members

    Params::List plist_;
};

//----------------------------------------------------------------------------

}  // namespace eckit

#endif
