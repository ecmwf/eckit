/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File PipeHandler.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_PipeHandler_h
#define eckit_PipeHandler_h

#include "eckit/container/ClassExtent.h"
#include "eckit/serialisation/PipeStream.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/runtime/Dispatcher.h"

//-----------------------------------------------------------------------------

namespace eckit {

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

	void handle(const std::vector<Request*>&);
	void pick(std::list<Request*>&,std::vector<Request*>&);
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

} // namespace eckit

#include "eckit/runtime/PipeHandler.cc"

#endif
