/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Resource.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Resource_h
#define eckit_Resource_h

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"
#include "eclib/Translator.h"
#include "eclib/Types.h"

class Url;

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Configurable;

class ResourceBase : private NonCopyable {

public: // methods

    /// Contructor
    ResourceBase(Configurable* owner, const std::string& str );

    /// Contructor for temporary resources that may depend on a input dictionary
    ResourceBase(const std::string& str, const StringDict& args );

    /// Destructor
	virtual ~ResourceBase();

	void reset() { inited_ = false; converted_ = false;  }
	void dump(ostream&) const;
	void html(ostream&,Url&);

	std::string name() const;

protected: // methods

    void init( const StringDict* args = 0 );
    void convert();

private: // members

    Configurable*       owner_;
	
    bool                inited_;
    bool                converted_;
    
    std::string         name_;        // In the config file
	std::string         environment_; // In the environment variables
	std::string         options_;     // For the command line options
    std::string         valueStr_;    // keeps the value in string form

private: // methods
    
    virtual void setValue(const string&) = 0;
	virtual string getValue() const      = 0;

};


template<class T> 
class Resource : public ResourceBase {

public: // methods

    Resource(const string& str,const T& value):
		ResourceBase(0,str),     value_(value) {}

    Resource(const string& str,const T& value, const StringDict& args ):
		ResourceBase(str,args),     value_(value) {}

	// Part of a configurable

    Resource(Configurable* owner,const string& str,const T& value):
		ResourceBase(owner,str), value_(value) {}

    /// @returns a copy of the resource value
    T value() { init(); convert(); return value_; }
    
    operator T&() { init(); convert(); return value_; }

private: // members

	T value_;

private: // overridden methods

	virtual void setValue(const string&);
	virtual string getValue() const;

};

//-----------------------------------------------------------------------------

template<class T> 
void Resource<T>::setValue(const string& s)
{
    value_ = Translator<string,T>()(s);
}

template<class T> 
string Resource<T>::getValue() const
{
    return Translator<T,string>()(value_);
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
