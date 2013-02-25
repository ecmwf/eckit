/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File PipeHandler.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eclib_PipeHandler_h
#define eclib_PipeHandler_h

#include "eclib/ClassExtent.h"
#include "eclib/PipeStream.h"
#include "eclib/ProcessControler.h"
#include "eclib/Dispatcher.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

template<class Request>
class PipeHandler : public ProcessControler,
					public ClassExtent<PipeHandler<Request> >,
					public DefaultHandler<Request> {
public:

// -- Contructors

	PipeHandler();

// -- Destructor

	~PipeHandler();

// -- Methods

	void handle(const vector<Request*>&);
	void pick(list<Request*>&,vector<Request*>&);
	void idle();

	bool busy() const { return busy_; }
	void ready(bool&) ;
	void age(time_t&) ;

// -- Overridden methods

	// From ProcessControler

	virtual void start();
	virtual void stop();

protected:

// -- Methods

	bool canPick();

private:

// -- Members

	PipeStream *pipe_;
	bool        busy_;
	time_t      last_;

// -- Methods

	virtual void endBatch(Stream&);

	void send(Request*);
	void receive(Request*);

	// From ProcessControler

	virtual void run();


};


//-----------------------------------------------------------------------------

} // namespace eclib

#include "eclib/PipeHandler.cc"

#endif
