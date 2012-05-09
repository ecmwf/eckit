/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>

#include "eclib/Log.h"
#include "eclib/Select.h"
#include "eclib/TCPServer.h"

TCPServer::TCPServer(int port,const string& addr):
    TCPSocket(),
    listen_(-1),
    port_(port),
    addr_(addr),
    closeExec_(true)
{
}

TCPServer::~TCPServer()
{
    if(listen_ >= 0)
        ::close(listen_);
}

// Accept a client

TCPSocket& TCPServer::accept(const string& message, int timeout, bool* connected)
{
    bind();


    sockaddr_in from;
#ifdef SGI
    int    fromlen = sizeof(from);
#else
    socklen_t fromlen = sizeof(from);
#endif

    for(;;)
    {
    int delay = timeout ? timeout : 10;

    Select select(listen_);
    Log::status() << message << endl;

    while(!select.ready(delay)) {
        if(timeout && !connected) throw TimeOut(message, timeout);
        if(connected) {
            *connected = false;
            return *this;
        }
        Log::status() << message << endl;
    }

        if((socket_ = ::accept(listen_,
            reinterpret_cast<sockaddr*>(&from), &fromlen))>=0)
            break;

        if(errno != EINTR)
            throw FailedSystemCall("accept");
    }

    remoteAddr_ = from.sin_addr;
    remoteHost_ = addrToHost(from.sin_addr);
    remotePort_ = from.sin_port;

    // Set the 'close on exec'

	if(closeExec_)
	  SYSCALL(fcntl(socket_,F_SETFD,FD_CLOEXEC));

    ::signal(SIGPIPE,SIG_IGN);

    Log::status() << "Get connection from " << remoteHost() << endl;

        if(connected) {
            *connected = true;
        }

    return *this;
}

void TCPServer::close()
{
    TCPSocket::close();
    if(listen_ >= 0)    
        ::close(listen_);
    listen_ = -1;
}

void TCPServer::bind()
{
    if(listen_ == -1)
    {
        listen_ = newSocket(port_);
        ::listen(listen_,5);
		
		//if(!willFork_)
		//  SYSCALL(fcntl(socket_,F_SETFD,FD_CLOEXEC));
    }
}


int TCPServer::socket()
{
    ((TCPServer*)this)->bind();
    return listen_;
}

string TCPServer::bindingAddress() const
{
    return addr_;
}
