/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ContextBehavior_h
#define eckit_ContextBehavior_h

#include <string>

#include "eckit/config/Configurable.h"
#include "eckit/config/ResourcePolicy.h"
#include "eckit/config/FileReadPolicy.h"
#include "eckit/log/Channel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class LogStream;

class ContextBehavior : public Configurable {
public:

    // -- Contructors

	ContextBehavior();

    // -- Destructor

	virtual ~ContextBehavior();

public: // interface methods

    virtual Channel& infoChannel()  = 0;
    virtual Channel& warnChannel()  = 0;
    virtual Channel& errorChannel() = 0;
    virtual Channel& debugChannel() = 0;

    virtual Channel& channel(int cat);

    int  debug() const  { return debug_; }
    void debug( int d ) { debug_ = d; }

    // From Configurable

    virtual void   reconfigure();
    virtual std::string name() const   { return "ContextBehavior"; }

    virtual ResourcePolicy resourcePolicy();
    virtual FileReadPolicy fileReadPolicy();

    virtual void configChannel(Channel& ,std::string);

public :

  enum ChannelCategory { ERROR=0, WARN=1, INFO=2, DEBUG=3 };

private: // methods

    // From Configurable

	virtual std::string kind() const  { return "ContextBehavior"; }

protected: // members

    int     debug_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_ContextBehavior_h
