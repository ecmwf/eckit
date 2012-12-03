/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <arpa/inet.h>

#include "eclib/FTPHandle.h"
#include "eclib/Log.h"
#include "eclib/TCPServer.h"

ClassSpec FTPHandle::classSpec_ = {&DataHandle::classSpec(),"FTPHandle",};
Reanimator<FTPHandle> FTPHandle::reanimator_;

const char* FTPHandle::FTPError::what() const throw()
{
	return "FTP Error";
}

void FTPHandle::print(ostream &s) const
{
	s << "FTPHandle[file=" << remote_ << ",host=" << host_ << ",port=" << port_ << ']';
}

void FTPHandle::encode(Stream& s) const
{
	DataHandle::encode(s);
	s << remote_;
	s << host_;
	s << port_;
}

FTPHandle::FTPHandle(Stream& s):
	DataHandle(s)
{
	s >> remote_;
	s >> host_;
	s >> port_;
}

string FTPHandle::readLine()
{
	string s;
	char c;
	while(cmds_.read(&c,1)==1 && c!='\n')
		s += c;
	Log::info() << "receive " << s << endl;
	return s;
}

void FTPHandle::ftpCommand(const string& s)
{
	Log::info() << "send " << s << endl;
	cmds_.write(s.c_str(),s.length());
	cmds_.write("\r\n",2);
	string out = readLine();	

	if(atoi(out.c_str()) / 100 == 5)
		throw FTPError();
}

FTPHandle::FTPHandle(const string& host,const string& remote,int port):
    remote_(remote),
	host_(host),
	port_(port)
{
}

void FTPHandle::open(const string& cmd)
{
	
	cmds_.connect(host_,port_);

	readLine();
	ftpCommand("USER mab");
	ftpCommand("PASS 04clave");
	ftpCommand("TYPE I");

	TCPServer server(0); // Any port

	int port    = htons(server.localPort());
	string addr = ::inet_ntoa(server.localAddr());

	char p[1024];
	snprintf(p,1024,"PORT %s,%d,%d",addr.c_str(),port/256,port%256);
	char *q = p;
	while(*q)
	{
		if(*q=='.')
			*q=',';
		q++;
	}
	ftpCommand(p);

	snprintf(p,1024,"%s %s",cmd.c_str(),remote_.c_str());

	ftpCommand(p);

	data_ = server.accept();
}

Length FTPHandle::openForRead()
{
	open("RETR");
	return 0;
}

void FTPHandle::openForWrite(const Length&)
{
	open("STOR");
}

void FTPHandle::openForAppend(const Length&)
{
	NOTIMP;
}

long FTPHandle::read(void* buffer,long length)
{
	return data_.read(buffer,length);
}

long FTPHandle::write(const void* buffer,long length)
{
	return data_.write(buffer,length);
}

void FTPHandle::close()
{
	data_.close();

	readLine();
	ftpCommand("QUIT");
	cmds_.close();
}

void FTPHandle::rewind()
{
	NOTIMP;
}


