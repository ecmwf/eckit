/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Jan 97

#ifndef eckit_TxnLog_h
#define eckit_TxnLog_h

#include "eckit/runtime/Main.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/transaction/TxnEvent.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// T should be a subclass of TxnEvent

template<class T>
struct TxnRecoverer {
   virtual ~TxnRecoverer() {}
	virtual void push(T*) = 0;
};

template<class T>
struct TxnFinder {
   virtual ~TxnFinder() {}
	virtual bool found(T&) = 0;
	virtual bool old() { return false; }
};


//----------------------------------------------------------------------------------------------------------------------


class TxnArray;


template<class T>
class TxnLog {
public:

// -- Contructors

	TxnLog(const std::string& = Main::instance().name());

// -- Destructor

	~TxnLog();

// -- Methods

	void begin(T&);
	void end(T&,bool);
	void update(T&);
	void recover(TxnRecoverer<T>&,bool,long);
	void find(TxnFinder<T>&);

private:

// No copy allowed

	TxnLog(const TxnLog<T>&);
	TxnLog<T>& operator=(const TxnLog<T>&);

// -- Methods

	PathName name(T& event);

// -- Members

	PathName           path_;
	PathName           next_;   // Should be declared after 'path_'
    TxnArray*          nextID_; // Should be declared after 'next_'

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#include "TxnLog.cc"

#endif
