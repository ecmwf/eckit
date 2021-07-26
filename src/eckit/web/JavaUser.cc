/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/web/JavaUser.h"
#include "eckit/web/JavaAgent.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


JavaUser::JavaUser(net::TCPSocket& socket) :
    net::NetUser(socket) {}

JavaUser::~JavaUser() {}

void JavaUser::serve(eckit::Stream& s, std::istream& in, std::ostream& out) {
    JavaAgent::serve(s, in, out);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
