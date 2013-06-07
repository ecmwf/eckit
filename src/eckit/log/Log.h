/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Log.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#ifndef eckit_log_Log_h
#define eckit_log_Log_h

#include "eckit/log/CodeLocation.h"
#include "eckit/log/Channel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// Singleton holding global streams for logging
///
/// @warning these streams are thread safe. A lock is
///          set when one of the stream is return, and is reset
///          on end of line. Don't forget to call endl.

class Log {
    
public: // types

	/// Output formats
	enum 
    {
		compactFormat = 0,
		normalFormat  = 1,
		fullFormat    = 2,
		monitorFormat = 3
	};

public: // methods

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

    /// characters to identify origin of monitoring messages
    enum { Unix = 'U', App  = 'X' }; 

    /// stream for application monitor
	static  ostream& monitor(char,long);

    /// manipulator that will print the last error message as in perror(2)
	static	ostream& syserr(ostream&);

private:

	Log();  ///< Private, non-instanciatable class
	~Log(); ///< Private, non-instanciatable class

};

//-----------------------------------------------------------------------------

ostream& setformat(ostream&,int);
int format(ostream&);

//-----------------------------------------------------------------------------

#define DEBUG_HERE     std::cerr << " DEBUG @ " << Here() << std::endl;
#define DEBUG_VAR(x)   std::cerr << #x << " : [" << x << "] @ " << Here() << std::endl;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
