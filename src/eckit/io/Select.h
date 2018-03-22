/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Select.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_Select_h
#define eckit_Select_h

#if 0
#ifdef __hpux
#include <sys/time.h>
#else
#include <sys/select.h>
#endif
#endif

#include <sys/select.h>

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TCPSocket;

/// Wraps calls to select
class Select : private NonCopyable {

public:

// -- Contructors

	Select();
	Select(int);
	Select(TCPSocket&);

// -- Destructor

	~Select();

// -- Methods

	bool ready(long sec = 20);

	void add(TCPSocket&);
	void add(int);

	void remove(TCPSocket&);
	void remove(int);

	bool set(TCPSocket&);
	bool set(int);

private:

// -- Members

	fd_set files_;
	fd_set set_;
	int    last_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
