/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Log.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Log_h
#define eckit_Log_h

#include "eckit/log/CodeLocation.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// The Log class in designed for logging messages
// Each line is prefixed by a time stamp, a message
// level (debug, info, warning or error) as well as
// the application name.

// Warning: these streams are thread safe. A lock is
// set when one of the stream is return, and is reset
// on end of line. Don't forget to call endl.

class UserStream {
public:
	virtual void infoMsg(const string&)    = 0;
	virtual void warningMsg(const string&) = 0;
	virtual void errorMsg(const string&)   = 0;
	virtual void notifyClient(const string&)   = 0;
};

//-----------------------------------------------------------------------------

class Log {
public:

	// How to format outputs
	enum {
		compactFormat = 0,
		normalFormat  = 1,
		fullFormat    = 2,
		monitorFormat = 3
	};

// -- Class methods

	static	ostream& debug(int level = 1);    // Stream for debug output
	static	ostream& debug(const CodeLocation& where, int level = 1);
    
	static	ostream& info();     // Stream for informative messages
	static	ostream& info(const CodeLocation& where);
	
    static	ostream& warning();  // Stream for warning messages
	static	ostream& warning(const CodeLocation& where);
	
    static	ostream& error();    // Stream for error messages
	static	ostream& error(const CodeLocation& where);
	
    static	ostream& panic();    // Stream for panic messages
	static	ostream& panic(const CodeLocation& where);

	static	ostream& status();    // Stream for status messages (monitor)
	
    static	ostream& message();    // Stream for status messages (monitor)

	static	ostream& userInfo();     // Stream for informative messages
    
	static	ostream& userWarning();  // Stream for warning messages
	
    static	ostream& userError();    // Stream for error messages

	static void setUserStream(UserStream*);
	static void notifyClient(const string&); // Send messages to client

	// TODO: is this still necessary? can it be easily removed?

	enum {
		Unix   = 'U',
        Adsm   = 'A',
        Other  = 'X'
	};

	static  ostream& monitor(char,long); // Stream for operator monitor

    /// manipulator that will print the last error message as in perror(2)
	static	ostream& syserr(ostream&);

private:

	// Log instances do not exist

	Log();
	~Log();

};

ostream& setformat(ostream&,int);
int format(ostream&);

//-----------------------------------------------------------------------------

#define DEBUG_HERE     std::cerr << " DEBUG @ " << Here() << std::endl;
#define DEBUG_VAR(x)   std::cerr << #x << " : [" << x << "] @ " << Here() << std::endl;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
