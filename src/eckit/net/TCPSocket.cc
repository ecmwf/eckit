/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <setjmp.h>


#include "eckit/os/AutoAlarm.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/log/Log.h"
#include "eckit/thread/Mutex.h"
#include "eckit/config/Resource.h"
#include "eckit/io/Select.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/types/Seconds.h"

#ifdef AIX
//TODO: Add check to cmake
typedef void (*sighandler_t) (int);
#endif

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static in_addr none = { INADDR_NONE };

static Mutex mutex;

TCPSocket::UnknownHost::UnknownHost(const string& host):
    Exception(string("Unknown host ") + host)
{
}


TCPSocket::TCPSocket():
    remotePort_(-1),
    localPort_(-1),
    socket_(-1),
    remoteAddr_(none),
    localAddr_(none),
    bufSize_(0)
{
}

// This contructor performs a cahnge of ownership of the socket
TCPSocket::TCPSocket(TCPSocket& other):
    socket_(other.socket_),
    remotePort_(other.remotePort_),
    remoteAddr_(other.remoteAddr_),
    remoteHost_(other.remoteHost_),
    localPort_(other.remotePort_),
    localHost_(other.remoteHost_),
    localAddr_(other.remoteAddr_)
{
    other.socket_ = -1;  // Detach socket from other
}

TCPSocket::~TCPSocket()
{
    close();
}

// This contructor performs a cahnge of ownership of the socket
TCPSocket& TCPSocket::operator=(TCPSocket& other)
{
    socket_     = other.socket_;

    remotePort_ = other.remotePort_;
    remoteAddr_ = other.remoteAddr_;
    remoteHost_ = other.remoteHost_;
    localAddr_  = other.localAddr_ ;
    localHost_  = other.localHost_ ;
    localPort_  = other.remotePort_;

    other.socket_ = -1;  // Detach socket from other

    return *this;
}

void TCPSocket::closeOutput()
{
  SYSCALL(::shutdown(socket_, SHUT_WR));
}

void TCPSocket::closeInput()
{
  SYSCALL(::shutdown(socket_, SHUT_RD));
}

long TCPSocket::write(const void *buf,long length)
{

    // Allow zero length packets
    if(length == 0)
        return ::write(socket_,buf,length);

    long sent     = 0;
    const char *p = (const char*)buf;

    while(length > 0)
    {
        long len = ::write(socket_,p,length);

        if(len <  0) {
            Log::error() << "Socket write" << Log::syserr << endl;
            return len;
        }

        if(len == 0) return sent;

        sent   += len;
        length -= len;
        p      += len;

    }

    return sent;
}

//static  Resource<long> socketSelectTimeout("socketSelectTimeout", 0);

long TCPSocket::read(void *buf,long length)
{
    if(length <= 0 ) return length;

    long received = 0;
    char *p = (char*)buf;
    bool nonews = false;


    while(length > 0)
    {

        long len = -1;
#if 0
        Select select(socket_);
        bool more = socketSelectTimeout > 0;
        while(more)
        {
            more = false;
            if(!select.ready(socketSelectTimeout)) 
            {
                log::SavedStatus save;
				
				Log::warning() << "No news from " << remoteHost()
                              << " from " << Seconds(socketSelectTimeout) << endl;

                Log::status() << "No news from " << remoteHost()
                              << " from " << Seconds(socketSelectTimeout) << endl;

// For now ...
        //		nonews = true;

                // Time out, write 0 bytes to check that peer is alive
                if(::write(socket_,0,0) != 0)
                {
                    Log::error() << "TCPSocket::read write" <<
                        Log::syserr <<endl;
                    return -1;
                }
                more = true;
                break;
            }
        }

        len = -1;

        if(nonews)
        {
            AutoAlarm alarm(60,true);
            Log::status() << "Resuming transfer" << endl;
            len = ::read(socket_,p,length);
        }
        else
#endif
            len = ::read(socket_,p,length);

        if(len <  0) {
            Log::error() << "Socket read" << Log::syserr << endl;
            return len;
        }

        if(len == 0) return received;

        received  += len;
        length    -= len;
        p         += len;

    }

    return received;
}

void TCPSocket::close()
{
    if(socket_ != -1) SYSCALL(::close(socket_));
    socket_ = -1;
    remotePort_  = localPort_  = -1;
    localHost_ = remoteHost_ = "";
    localAddr_ = remoteAddr_ = none;
}

static jmp_buf env;

static void catch_alarm(int sig)
{
    longjmp(env,1);
}


// This should be in the TCPClient.cc, but I want to reuse the Mutex
// to lock any call to NIS with the same one

TCPSocket& TCPClient::connect(const string& remote,int port,int retries, int timeout)
{
    string host = hostName(remote);

    in_addr_t addr;
    in_addr_t none = (in_addr_t)-1;
    hostent *him;

    sockaddr_in sin;
    ::memset(&sin,0,sizeof(sin));


    { // Block for mutex

        AutoLock<Mutex> lock(mutex);

        sin.sin_port   = htons(port);
        sin.sin_family = AF_INET;

        addr = ::inet_addr(remote.c_str());
        sin.sin_addr.s_addr = addr;


        if(addr == none) {

// For some reason, sgi verion of gethostbyname_r is broken
#if 0
            hostent_data data;
            hostent      host;

            // Due to a STUPID aix bug we need to clear
            zero(host);
            zero(data);

            if(gethostbyname_r(remote.c_str(),&host,&data))
                him = 0;
            else
                him = &host;
#else
            him=::gethostbyname(remote.c_str());
#endif
            if (him == 0)
            {
//				Log::error() << "Unknown host [" << remote << "]" << endl;
                throw UnknownHost(remote);
            }

            sin.sin_family = him->h_addrtype;
            ::memcpy(&sin.sin_addr,him->h_addr_list[0],him->h_length);
        }

    } // End of mutex

    int tries = 0;
    int status = 0;


    do
    {

        bind();

        if(timeout)
        {
            if(setjmp(env)==0)
            {
                void (*old)(int) = signal(SIGALRM,catch_alarm);
                alarm(timeout);
                status = ::connect(socket_,reinterpret_cast<sockaddr*>(&sin),sizeof(sin));
                alarm(0);
                /// @todo change this to sigaction
                signal(SIGALRM,old);
            }
            else
            {
                throw TimeOut("connect", timeout);
            }
        }
        else
        {
            status = ::connect(socket_,reinterpret_cast<sockaddr*>(&sin),sizeof(sin));
        }

        if (status < 0)
        {
            Log::error() << "connect to "
                << host << " " << port << Log::syserr << endl;

            Log::status() << "Connect: " << host << ":" << port << Log::syserr <<
                " " << tries << '/' << retries << endl;

            int save_errno = errno;
            ::close(socket_);
            socket_ = -1;
            errno = save_errno;

            switch(errno)
            {
                case ECONNREFUSED:
                    if(++tries >= retries)
                        if(retries >= 0)
                            throw TooManyRetries(tries);
                    ::sleep(5);
                    break;

                case EINPROGRESS:
                    //TODO: Potential file descriptor leak
                    /* ::close(socket_); */
                    /* socket_ = -1; */
                    throw FailedSystemCall("connect");
                    break;

#if 0
                case ETIMEDOUT:
                    Log::info() << "Waiting for network "
                        << host << ":" << port
                        << Log::syserr << endl;
                    Log::status() << "Waiting for network "
                        << host << ":" << port
                        << Log::syserr << endl;
                    break;
#endif
#if 0
                case ENETUNREACH:
                case ETIMEDOUT:
                case ECONNRESET:
                case EADDRNOTAVAIL:
                case EAGAIN:
#endif
                default:
                    Log::status() << "Waiting for network "
                        << host << ":" << port
                        << Log::syserr << endl;
#if 0
                    if(++tries >= retries)
                        if(retries != 0)
                            throw TooManyRetries(tries);
#endif
                    ::sleep(120);
                    break;

#if 0
                default:
                    throw FailedSystemCall("connect");
                    break;
#endif
            }

        }

    }while(status<0);

    remotePort_ = sin.sin_port;
    remoteAddr_ = sin.sin_addr;
    remoteHost_ = addrToHost(sin.sin_addr);

    /// @todo change this to sigaction
    ::signal(SIGPIPE,SIG_IGN);
    return *this;
}


int TCPSocket::newSocket(int port)
{

    localPort_ = port;

    int s = ::socket(AF_INET, SOCK_STREAM, 0);

    if(s < 0)
        throw FailedSystemCall("::socket");

    int flg = 1 ;
    if(::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flg, sizeof(flg))<0)
        Log::warning() << "setsockopt SO_REUSEADDR" << Log::syserr << endl;

    flg = 1 ;
    if(::setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &flg, sizeof(flg))<0)
        Log::warning() << "setsockopt SO_KEEPALIVE" << Log::syserr << endl;


#ifdef SO_REUSEPORT
    flg = 1 ;
    if(::setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &flg, sizeof(flg))<0)
        Log::warning() << "setsockopt SO_REUSEPORT" << Log::syserr << endl;
#endif

#ifdef SO_LINGER
    linger ling;
    ling.l_onoff  = 0;
    ling.l_linger = 0;
    if(::setsockopt(s, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling))<0)
        Log::warning() << "setsockopt SO_LINGER" << Log::syserr << endl;
#endif

#ifdef SO_DONTLINGER
    if(::setsockopt(s, SOL_SOCKET, SO_DONTLINGER, NULL, 0)<0)
        Log::warning() << "setsockopt SO_DONTLINGER" << Log::syserr << endl;
#endif

/* #ifdef IPPROTO_IP */
/* #ifdef IP_TOS */
/* #ifndef IPTOS_LOWDELAY */
/* #define IPTOS_LOWDELAY 0x10 */
/* #endif */
    int tos = IPTOS_LOWDELAY;

    if(::setsockopt(s, IPPROTO_IP, IP_TOS, &tos, sizeof(tos))<0)
          Log::warning() << "setsockopt IP_TOS" << Log::syserr << endl;

/* #endif */
/* #endif */



#if 1
    int flag = 1;
    if(::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag))<0)
        Log::warning() << "setsockopt TCP_NODELAY" << Log::syserr << endl;
#endif

    if(bufSize_)
    {

        Log::info() << "SOCKET SIZE " << bufSize_ << endl;

        int flg = 0;
        socklen_t flgsize = sizeof(flg);

        if(getsockopt(s, SOL_SOCKET, SO_SNDBUF,&flg, &flgsize)<0)
            Log::warning() << "getsockopt SO_SNDBUF " << Log::syserr << endl;

        if(flg != bufSize_)
        {
            if(setsockopt(s, SOL_SOCKET, SO_SNDBUF,&bufSize_, sizeof(bufSize_)) < 0)
                Log::warning() << "setsockopt SO_SNDBUF " << Log::syserr << endl;
        }

        if(getsockopt(s, SOL_SOCKET, SO_RCVBUF,&flg, &flgsize)<0)
            Log::warning() << "getsockopt SO_RCVBUF " << Log::syserr << endl;

        if(flg != bufSize_)
        {
            if(setsockopt(s, SOL_SOCKET, SO_RCVBUF,&bufSize_, sizeof(bufSize_)) < 0)
                Log::warning() << "setsockopt SO_RCVBUF " << Log::syserr << endl;
        }
    }


    sockaddr_in sin;
    ::memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_port        = htons(localPort_);
    sin.sin_family      = AF_INET;

    string addr = bindingAddress();

    if(addr.length() == 0)
        sin.sin_addr.s_addr = INADDR_ANY;
    else
        sin.sin_addr.s_addr = ::inet_addr(addr.c_str());

    while(::bind(s,reinterpret_cast<sockaddr*>(&sin),sizeof(sin)) == -1)
    {
        Log::warning() << "bind port " << localPort_ << " " << addr << Log::syserr << endl;
        ::sleep(5);
    }

    AutoLock<Mutex> lock(mutex);
#ifdef SGI
    int    len = sizeof(sin);
#else
    socklen_t len = sizeof(sin);
#endif
    ::getsockname(s,reinterpret_cast<sockaddr*>(&sin),&len);
    localPort_ = ntohs(sin.sin_port);
    localAddr_ = sin.sin_addr;
    localHost_ = addrToHost(sin.sin_addr);

    if(localHost_ == "0.0.0.0") 
    {
        if(addr.length() == 0)
        {
            AutoLock<Mutex> lock(mutex);
            localHost_= Resource<string> ("host", "");
            if(localHost_.length() == 0)
            {
                char host[1024];
                SYSCALL(gethostname(host,sizeof(host)-1));
                localHost_ = host;
            }
		}
        else
        {
            localHost_ = addr;
        }
    }

    // Set the socket 'close on exec'
    SYSCALL(fcntl(s,F_SETFD,FD_CLOEXEC));

    return s;
}

void TCPSocket::bind()
{
}


static map<uint32_t,string> cache;

string TCPSocket::addrToHost(in_addr addr)
{
    AutoLock<Mutex> lock(mutex);

    // For some reason IBM's gethostbyaddr_r dumps core
    // from time to time, so let's cache the result
    // to minimise the cores

    map<uint32_t,string>::iterator j = cache.find(addr.s_addr);

    if(j != cache.end())
        return (*j).second;

    hostent *h;

// For some reason, sgi verion of gethostbyname_r is broken
#ifdef AIX



    hostent_data data;
    hostent      host;

    // Due to a STUPID aix bug we need to clear
    zero(host);
    zero(data);

    if(gethostbyaddr_r(reinterpret_cast<char*>(&addr),
        sizeof(addr),AF_INET,&host,&data))
        h = 0;
    else
        h = &host;
#else
    h = gethostbyaddr(reinterpret_cast<char*>(&addr),
        sizeof(addr),AF_INET);
#endif

    string s = h ? h->h_name : ::inet_ntoa(addr);
    cache[addr.s_addr] = s;
    return s;
}


string TCPSocket::hostName(const string& h, bool full)
{
    in_addr_t addr = ::inet_addr(h.c_str());
    if(addr == (in_addr_t)-1)
    {
        if(full)
            return h;
        else
            return h.substr(0,h.find('.'));
    }

    struct in_addr a;
    a.s_addr = addr;
    string s =  addrToHost(a);

    if(!full && !isdigit(s[0])) {
        return s.substr(0,s.find('.'));
    }

    return s;
}

int TCPSocket::socket()
{
    return socket_;
}

in_addr TCPSocket::remoteAddr() const
{
    return remoteAddr_;
}

const string& TCPSocket::remoteHost() const
{
    return remoteHost_;
}

int TCPSocket::remotePort() const
{
    return remotePort_;
}

in_addr TCPSocket::localAddr() const
{
    ((TCPSocket*)this)->bind();
    return localAddr_;
}

const string& TCPSocket::localHost() const
{
    ((TCPSocket*)this)->bind();
    return localHost_;
}

int TCPSocket::localPort() const
{
    ((TCPSocket*)this)->bind();
    return localPort_;
}

string TCPSocket::bindingAddress() const
{
    return "";
}

long TCPSocket::rawRead(void* buf,long length)
{
    return ::read(socket_,buf,length);
}

bool TCPSocket::stillConnected() const
{
    if(socket_ == -1)
        return false;

    fd_set r;
    fd_set e;
    fd_set w;

    FD_ZERO(&r); FD_SET(socket_, &r);
    FD_ZERO(&e); FD_SET(socket_, &e);
    FD_ZERO(&w); FD_SET(socket_, &w);

    timeval tv = { 0, 0};

    if(::select(socket_+1,&r, &w, &e, &tv) >= 0)
    {
        if(!FD_ISSET(socket_, &r))
            return true;

        int n = 0;
        if(::ioctl(socket_, FIONREAD, &n) < 0)
        {
            Log::info() << "TCPSocket::stillConnected(FIONREAD) failed " << Log::syserr << endl;
            return false;
        }

        if(n == 0) {
            Log::warning() << "TCPSocket::stillConnected => connection lost" << endl;
            return false;
        }

        return true;
    }
    else
    {
        Log::info() << "TCPSocket::stillConnected(select) failed " << Log::syserr << endl;
        return false;

    }

}

//-----------------------------------------------------------------------------

} // namespace eckit

