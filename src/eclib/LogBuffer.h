/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File LogBuffer.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef LogBuffer_H
#define LogBuffer_H

#include <memory>

#include "eclib/machine.h"

#include "eclib/Context.h"

class Logger;

//
// This class assumes that iostream are thread safe during a single
// call to the << inserter
//

class LogBuffer: public streambuf {
public:

  typedef ostream& (*oproc)(ostream&);
  static ostream& noop(ostream& s) { return s; }

  LogBuffer( Logger* logger);

  ~LogBuffer();

  void source( const CodeLocation& where );
  
private:

  std::auto_ptr<Logger> logger_;
  
  bool      start_;
  
  char      buffer_[1024];
  char*     line_;
  int       len_;

  void		dumpBuffer(void);

protected:
  
  int	overflow(int);
  int	sync();

};

class StatusBuffer: public streambuf {
private:

  char  buffer_[1024];
  void	dumpBuffer(void);

protected:
  int	overflow(int);
  int	sync();

public:
  StatusBuffer();
  ~StatusBuffer();
};

class MessageBuffer: public streambuf {
private:

  char  buffer_[1024];
  void	dumpBuffer(void);

protected:
  int	overflow(int);
  int	sync();

public:
  MessageBuffer();
  ~MessageBuffer();
};

class UserOutStream;

class UserOutBuffer: public streambuf {
private:

  UserOutStream* owner_;
  char          buffer_[1024];
  void		    dumpBuffer(void);

protected:
  int	overflow(int);
  int	sync();

public:
  UserOutBuffer(UserOutStream*);
  ~UserOutBuffer();
};

class MonitorBuffer: public streambuf {
private:

  char  type_;
  long  code_;
  bool	start_;

  char  buffer_[1024];
  void	dumpBuffer(void);

protected:
  int	overflow(int);
  int	sync();

public:

  void flags(char type,long code) { type_ = type; code_ = code; }

  MonitorBuffer();
  ~MonitorBuffer();
};

class LogStream: public ostream {
public:

    LogStream( Logger* logger );

  	~LogStream();

	ostream& source( const CodeLocation& where )
	{
		LogBuffer *buf = dynamic_cast<LogBuffer *>(rdbuf());
		if (buf) buf->source(where);
		return *this;
	}
};

class InfoStream : public LogStream {
public:
    InfoStream() : LogStream( Context::instance().createInfoLogger() ){}
};

class DebugStream : public LogStream {
public:
	DebugStream() : LogStream(  Context::instance().createDebugLogger() ){}
};

class WarningStream : public LogStream {
public:
	WarningStream() : LogStream( Context::instance().createWarningLogger() ){}
};

class ErrorStream : public LogStream {
public:
	ErrorStream() : LogStream( Context::instance().createErrorLogger() ){}
};

class StatusStream: public ostream
{
public:
	StatusStream();
	~StatusStream();
};

class MessageStream: public ostream
{
public:
	MessageStream();
	~MessageStream();
};

class UserOutStream: public ostream
{
public:
	UserOutStream();
	~UserOutStream();
	virtual void out(const string&) = 0;
};

class UserInfoStream: public UserOutStream {
	virtual void out(const string&);
};

class UserWarningStream: public UserOutStream {
	virtual void out(const string&);
};

class UserErrorStream: public UserOutStream {
	virtual void out(const string&);
};

class MonitorStream: public ostream
{
public:
	void flags(char,long);
	MonitorStream();
	~MonitorStream();
};



#endif
