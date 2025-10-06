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
/// @date   Jul 97

#ifndef eckit_SharedInt_h
#define eckit_SharedInt_h

#include "eckit/os/Semaphore.h"


namespace eckit {

class SharedInt : public Semaphore {
public:

    SharedInt(const PathName&, int n = 1);

    ~SharedInt();

    void use(int n = 0);
    void use(int n, short v);

    void release(int n = 0);
    void release(int n, short v);

    int free(int n = 0) const;
    int limit(int n = 0) const;

    void newLimit(short val, unsigned short n = 0);

private:

    SharedInt(const SharedInt&)            = delete;
    SharedInt& operator=(const SharedInt&) = delete;
};

}  // namespace eckit

#endif
