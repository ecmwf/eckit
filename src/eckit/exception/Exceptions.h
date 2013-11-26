/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Exceptions_h
#define eckit_Exceptions_h

#include <errno.h>

#include "eckit/eckit.h"

#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/log/SavedStatus.h"
#include "eckit/compat/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void handle_panic(const char*);
void handle_panic(const char*, const CodeLocation&);

/// @brief General purpose exception
/// Derive other exceptions from this class and implement then in the class that throws them.
class Exception : public std::exception {

public: // methods

    /// Constructor with message
    Exception(const string& what, const CodeLocation& location = CodeLocation() );

    /// Destructor
    /// @throws nothing
    ~Exception() throw();
    
    virtual const char *what() const throw() {  return what_.c_str(); }
    virtual bool retryOnServer() const        { return false; }
    virtual bool retryOnClient() const        { return false; }
    virtual bool terminateApplication() const { return false; }

    static bool throwing();
    static void exceptionStack(std::ostream&,bool callStack = false);

    const std::string& callStack() const { return callStack_; }

protected: // methods
    
    void reason(const string&);
    Exception();

    virtual void print(std::ostream&) const;
    
private: // members
    
    string            what_;      ///< description
    string            callStack_; ///< call stack
    SavedStatus       save_;      ///< saved monitor status to recover after destruction
    Exception*        next_;
    CodeLocation      location_;  ///< where exception was first thrown
    

    friend std::ostream& operator<<(std::ostream& s,const Exception& p)
    {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

class SeriousBug : public Exception {
public:
    SeriousBug(const string& w) : Exception(string("Serious Bug:") + w) {}
    SeriousBug(const string&,const string&);
    SeriousBug(const string&,int);
};

class TooManyRetries : public Exception {
public:
    TooManyRetries(const int);
};

class TimeOut : public Exception {
public:
    TimeOut(const string&, const unsigned long);
};

class FailedSystemCall : public Exception {
public:
    FailedSystemCall(const string&);
    FailedSystemCall(const string&,const CodeLocation&);
    FailedSystemCall(const char*,const CodeLocation&,int);
    FailedSystemCall(const string&,const char*,const CodeLocation&,int);
};

class AssertionFailed : public Exception {
public:
    AssertionFailed(const string&);
    AssertionFailed(const char*, const CodeLocation& );
};

class BadParameter : public Exception {
public:
    BadParameter(const string& s);
};

class NotImplemented : public Exception {
public:
    NotImplemented( const CodeLocation& );
};

class Stop : public Exception {
public:
    Stop(const string&);
};

class Abort : public Exception {
public:
    Abort(const string&);
};

class Cancel : public Exception {
public:
    Cancel(const string&);
};

class Retry : public Exception {
public:
    Retry(const string&);
};

class UserError : public Exception {
public:
    UserError(const string&);
    UserError(const string&,const string&);
    UserError(const string&,int);
};

class OutOfRange : public Exception {
public:
    OutOfRange(unsigned long long, unsigned long long);
};

class MethodNotYetImplemented : public Exception {
public:
    MethodNotYetImplemented(const string& msg);
};

// File errors

class FileError : public Exception {
protected:
    FileError(const string&);
    FileError()					{  }
};

class CantOpenFile : public FileError {
    bool retry_;
    virtual bool retryOnServer() const { return retry_; }
public:
    CantOpenFile(const string&,bool retry = false);
};

class WriteError : public FileError {
public:
    WriteError(const string&);
};

class ReadError : public FileError {
public:
    ReadError(const string&);
};

class ShortFile : public ReadError {
public:
    ShortFile(const string&);
};

class RemoteException : public Exception {
public:
    RemoteException(const string& msg, const string& from);
};

class UnexpectedState : public Exception {
public:
    UnexpectedState(const string& msg) : Exception(msg) {}
};

//-----------------------------------------------------------------------------

inline void SysCall(long long code,const char *msg,const CodeLocation& loc)
{
    if(code<0)
        throw FailedSystemCall(msg,loc,errno);
}

//-----------------------------------------------------------------------------

template<class T>
inline void SysCall(long long code,const char *msg,const T& ctx,const CodeLocation& loc)
{
    if(code<0)
    {
        StrStream os;
        os << ctx << StrStream::ends;
        throw FailedSystemCall(string(os),msg,loc,errno);
    }
}

//-----------------------------------------------------------------------------

inline void ThrCall(int code,const char *msg, const CodeLocation& loc)
{
    if(code != 0) // Threads return errno in return code
        handle_panic(msg,loc);
    /*
    {
        throw FailedSystemCall(msg,line,file,proc,code);
    }
    */
}

inline void Assert(int code,const char *msg, const CodeLocation& loc )
{
    if(code != 0)
        throw AssertionFailed(msg,loc);
}

inline void Panic(int code,const char *msg, const CodeLocation& loc )
{
    if(code != 0)
        handle_panic(msg,loc);
}

//-----------------------------------------------------------------------------

/// For compatibility
class OutOfMemory : public Exception {
    virtual bool terminateApplication() const { return true; }
    virtual const char *what() const throw() {  return "OutOfMemory"; }
public:
    OutOfMemory();

};

//-----------------------------------------------------------------------------

#define THRCALL(a)    ::eckit::ThrCall(a,#a,Here())
#define SYSCALL(a)    ::eckit::SysCall(a,#a,Here())
#define SYSCALL2(a,b) ::eckit::SysCall(a,#a,b,Here())
#define ASSERT(a)     ::eckit::Assert(!(a),#a,Here())
#define PANIC(a)      ::eckit::Panic((a),#a,Here())
#define NOTIMP  throw ::eckit::NotImplemented(Here())

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
