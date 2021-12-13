/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/types.h>  // FreeBSD: must appear before netinet/ip.h

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Select.h"
#include "eckit/log/Log.h"
#include "eckit/log/Seconds.h"
#include "eckit/memory/Zero.h"
#include "eckit/net/IPAddress.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/os/AutoAlarm.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"

namespace eckit {
namespace net {


static in_addr none = {INADDR_NONE};

static Once<Mutex> local_mutex;

TCPSocket::UnknownHost::UnknownHost(const std::string& host) :
    Exception(std::string("Unknown host ") + host) {}


TCPSocket::TCPSocket() :
    socket_(-1),
    localPort_(-1),
    remotePort_(-1),
    remoteAddr_(none),
    localAddr_(none),
    debug_(false),
    newline_(true),
    mode_(0) {}

// This contructor performs a change of ownership of the socket
TCPSocket::TCPSocket(net::TCPSocket& other) :
    socket_(other.socket_),
    localPort_(other.localPort_),
    remotePort_(other.remotePort_),
    remoteHost_(other.remoteHost_),
    remoteAddr_(other.remoteAddr_),
    localHost_(other.localHost_),
    localAddr_(other.localAddr_),
    debug_(false),
    newline_(true),
    mode_(0) {
    other.socket_     = -1;  // Detach socket from other
    other.remoteAddr_ = none;
    other.remoteHost_ = std::string();
    other.remotePort_ = -1;
}

TCPSocket::~TCPSocket() {
    close();
}

// This contructor performs a change of ownership of the socket
TCPSocket& TCPSocket::operator=(net::TCPSocket& other) {
    socket_ = other.socket_;

    localAddr_  = other.localAddr_;
    localHost_  = other.localHost_;
    localPort_  = other.localPort_;
    remoteAddr_ = other.remoteAddr_;
    remoteHost_ = other.remoteHost_;
    remotePort_ = other.remotePort_;

    debug_   = other.debug_;
    newline_ = other.newline_;
    mode_    = other.mode_;

    other.socket_ = -1;  // Detach socket from other

    other.remoteAddr_ = none;
    other.remoteHost_ = std::string();
    other.remotePort_ = -1;

    return *this;
}

void TCPSocket::closeOutput() {
    SYSCALL(::shutdown(socket_, SHUT_WR));
}

void TCPSocket::closeInput() {
    SYSCALL(::shutdown(socket_, SHUT_RD));
}

long TCPSocket::write(const void* buf, long length) {

    // Allow zero length packets
    if (length == 0)
        return ::write(socket_, buf, length);

    long requested = length;

    if (debug_) {

        if (mode_ != 'w') {
            newline_ = true;
            std::cout << std::endl
                      << std::endl;
            mode_ = 'w';
        }

        const char* p = reinterpret_cast<const char*>(buf);
        for (long i = 0; i < std::min(length, 512L); i++) {
            if (newline_) {
                std::cout << ">>> ";
                newline_ = false;
            }

            if (p[i] == '\r') {
                std::cout << "\\r";
            }
            else if (p[i] == '\n') {
                std::cout << "\\n"
                          << std::endl;
                newline_ = true;
            }
            else {
                std::cout << (isprint(p[i]) ? p[i] : '.');
            }
        }

        if (length > 512) {
            std::cout << "..." << std::endl;
            newline_ = true;
        }
    }

    long sent     = 0;
    const char* p = static_cast<const char*>(buf);

    while (length > 0) {
        long len                         = 0;
        size_t retries                   = 0;
        const size_t maxTCPSocketRetries = 10 * 60;  // 10 minutes

        errno = 0;
        len   = ::write(socket_, p, length);

        while (len == 0) {

            Log::warning() << "Socket write returns zero (" << *this << ")" << Log::syserr << std::endl;

            if (++retries >= maxTCPSocketRetries) {
                Log::warning() << "Giving up." << std::endl;
                break;
            }

            Log::warning() << "Sleeping...." << std::endl;
            ::sleep(1);

            errno = 0;
            len   = ::write(socket_, p, length);
        }

        if (len < 0) {
            Log::error() << "Socket write failed (" << *this << ")" << Log::syserr << std::endl;
            return len;
        }

        if (len == 0) {
            Log::warning() << "Socket write incomplete (" << *this << ") " << sent << " out of " << requested
                           << std::endl;
            return sent;
        }

        sent += len;
        length -= len;
        p += len;
    }

    return sent;
}

long TCPSocket::read(void* buf, long length) {
    if (length <= 0)
        return length;

    static bool useSelectOnTCPSocket = Resource<bool>("useSelectOnTCPSocket", false);
    long received                    = 0;
    char* p                          = static_cast<char*>(buf);
    bool nonews                      = false;

    while (length > 0) {
        long len;
        if (useSelectOnTCPSocket) {
            static long socketSelectTimeout = Resource<long>("socketSelectTimeout", 0);
            Select select(socket_);
            bool more = socketSelectTimeout > 0;
            while (more) {
                more = false;
                if (!select.ready(socketSelectTimeout)) {
                    SavedStatus save;

                    Log::warning() << "No news from " << remoteHost() << " from " << Seconds(socketSelectTimeout)
                                   << std::endl;

                    Log::status() << "No news from " << remoteHost() << " from " << Seconds(socketSelectTimeout)
                                  << std::endl;

                    // FIXME: enable the nonews flag here?
                    // nonews = true;

                    // Time out, write 0 bytes to check that peer is alive
                    if (::write(socket_, nullptr, 0) != 0) {
                        Log::error() << "TCPSocket::read write" << Log::syserr << std::endl;
                        return -1;
                    }
                    more = true;
                    break;
                }
            }

            len = -1;

            if (nonews) {
                AutoAlarm alarm(60, true);
                Log::status() << "Resuming transfer" << std::endl;
                len = ::read(socket_, p, length);
            }
            else
                len = ::read(socket_, p, length);
        }
        else {
            len = ::read(socket_, p, length);
        }

        if (len < 0) {
            Log::error() << "Socket read failed (" << *this << ")" << Log::syserr << std::endl;
            return len;
        }

        if (len == 0) {
            return received;
        }

        if (debug_) {

            if (mode_ != 'r') {
                newline_ = true;
                std::cout << std::endl
                          << std::endl;
                mode_ = 'r';
            }

            for (long i = 0; i < std::min(len, 512L); i++) {
                if (newline_) {
                    std::cout << "<<< ";
                    newline_ = false;
                }

                if (p[i] == '\r') {
                    std::cout << "\\r";
                }
                else if (p[i] == '\n') {
                    std::cout << "\\n"
                              << std::endl;
                    newline_ = true;
                }
                else {
                    std::cout << (isprint(p[i]) ? p[i] : '.');
                }
            }

            if (len > 512) {
                std::cout << "..." << std::endl;
                newline_ = true;
            }
        }


        received += len;
        length -= len;
        p += len;
    }

    return received;
}

void TCPSocket::close() {
    if (socket_ != -1) {
        SYSCALL(::close(socket_));
    }
    socket_     = -1;
    remotePort_ = localPort_ = -1;
    localHost_ = remoteHost_ = "";
    localAddr_ = remoteAddr_ = none;
}

static jmp_buf env;

static void catch_alarm(int) {
    longjmp(env, 1);
}


// This should be in the TCPClient.cc, but I want to reuse the Mutex
// to lock any call to NIS with the same one

TCPSocket& TCPClient::connect(const std::string& remote, int port, int retries, int timeout) {
    std::string host = hostName(remote);

    in_addr_t addr;
    in_addr_t none = (in_addr_t)-1;
    hostent* him;

    sockaddr_in sin;
    ::memset(&sin, 0, sizeof(sin));


    {  // Block for local_mutex

        AutoLock<Mutex> lock(local_mutex);

        sin.sin_port   = htons(port);
        sin.sin_family = AF_INET;

        addr                = ::inet_addr(remote.c_str());
        sin.sin_addr.s_addr = addr;


        if (addr == none) {

// For some reason, sgi verion of gethostbyname_r is broken
#if 0
            hostent_data data;
            hostent      host;

            // Due to a STUPID aix bug we need to clear
            zero(host);
            zero(data);

            if (gethostbyname_r(remote.c_str(), &host, &data))
                him = 0;
            else
                him = &host;
#else
            him = ::gethostbyname(remote.c_str());
#endif
            if (him == nullptr) {
                //              Log::error() << "Unknown host [" << remote << "]" << std::endl;
                throw UnknownHost(remote);
            }

            sin.sin_family = him->h_addrtype;
            ::memcpy(&sin.sin_addr, him->h_addr_list[0], him->h_length);
        }

    }  // End of local_mutex

    int tries  = 0;
    int status = 0;


    do {
        int save_errno = 0;

        bind();

        if (timeout) {
            if (setjmp(env) == 0) {
                void (*old)(int) = signal(SIGALRM, catch_alarm);
                alarm(timeout);
                status     = ::connect(socket_, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
                save_errno = errno;
                alarm(0);
                /// @todo change this to sigaction
                signal(SIGALRM, old);
            }
            else {
                throw TimeOut("connect", timeout);
            }
        }
        else {
            status     = ::connect(socket_, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
            save_errno = errno;
        }

        if (status < 0) {
            errno = save_errno;
            Log::error() << "connect to " << host << " " << port << Log::syserr << std::endl;

            Log::status() << "Connect: " << host << ":" << port << Log::syserr << " " << tries << '/' << retries
                          << std::endl;

            ::close(socket_);
            socket_ = -1;
            errno   = save_errno;

            switch (errno) {
                case ECONNREFUSED:
                    if (++tries >= retries) {
                        if (retries >= 0)
                            throw TooManyRetries(tries);
                    }
                    ::sleep(5);
                    break;

                case EINPROGRESS:
                    // TODO: Potential file descriptor leak
                    /* ::close(socket_); */
                    /* socket_ = -1; */
                    throw FailedSystemCall("connect");

#if 0
            case ETIMEDOUT:
                Log::info() << "Waiting for network "
                            << host << ":" << port
                            << Log::syserr << std::endl;
                Log::status() << "Waiting for network "
                              << host << ":" << port
                              << Log::syserr << std::endl;
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

                    Log::status() << "Waiting for network " << host << ":" << port << Log::syserr << std::endl;

#if 0
                if (++tries >= retries) {
                    if (retries != 0)
                        throw TooManyRetries(tries);
                }
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

    } while (status < 0);

    remotePort_ = ntohs(sin.sin_port);
    remoteAddr_ = sin.sin_addr;
    remoteHost_ = addrToHost(sin.sin_addr);

    register_ignore_sigpipe();

    return *this;
}

void set_socket_buffer_size(int& socket, const char* ssock, const int& stype, const int size ) {
    Log::debug() << "Setting " << ssock << " buffer size " << size << std::endl;

    int flg           = 0;
    socklen_t flgsize = sizeof(flg);

    if (getsockopt(socket, SOL_SOCKET, stype, &flg, &flgsize) < 0)
            Log::warning() << "getsockopt " << ssock << " " << Log::syserr << std::endl;

   if (flg != size) {
        if (setsockopt(socket, SOL_SOCKET, stype, &size, sizeof(size)) < 0)
            Log::warning() << "setsockopt " << ssock << " " << Log::syserr << std::endl;

        if (getsockopt(socket, SOL_SOCKET, stype, &flg, &flgsize) < 0)
            Log::warning() << "getsockopt " << ssock << " " << Log::syserr << std::endl;

        bool warn = (flg != size);
#if defined(__linux__)
        // For Linux we ignore if the effective size is 2x what is set
        // see Linux 'man 7 socket'
        // when set using setsockopt() the Linux kernel doubles the socket buffer size 
        // to allow space for bookkeeping overhead and  this  doubled  value  is
        // returned  by  getsockopt(). The minimum (doubled) value for this option is 2048.
        warn &= !(flg == 2 * size);  
#endif
        if(warn)
            Log::warning() << "Attempt to set " << stype << " buffer size to " << size << " but kernel set size to " << flg << std::endl;
    }
}


int TCPSocket::createSocket(int port, const SocketOptions& opts) {

    localPort_ = port;

    int s = ::socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        throw FailedSystemCall("::socket");
    }

    if (opts.reuseAddr()) {
        int flg = 1;
        if (::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flg, sizeof(flg)) < 0)
            Log::warning() << "setsockopt SO_REUSEADDR" << Log::syserr << std::endl;
    }

    if (opts.keepAlive()) {
        int flg = 1;
        if (::setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &flg, sizeof(flg)) < 0)
            Log::warning() << "setsockopt SO_KEEPALIVE" << Log::syserr << std::endl;
    }

    if (opts.reusePort()) {
#ifdef SO_REUSEPORT
        int flg = 1;
        SYSCALL(::setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &flg, sizeof(flg)));
#endif
    }

    if (opts.noLinger()) {
#ifdef SO_LINGER  ///< turn off SO_LINGER
        linger ling;
        ling.l_onoff  = 0;
        ling.l_linger = 0;
        if (::setsockopt(s, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0)
            Log::warning() << "setsockopt SO_LINGER" << Log::syserr << std::endl;
#endif

#ifdef SO_DONTLINGER
        if (::setsockopt(s, SOL_SOCKET, SO_DONTLINGER, NULL, 0) < 0)
            Log::warning() << "setsockopt SO_DONTLINGER" << Log::syserr << std::endl;
#endif
    }

    if (opts.ipLowDelay()) {
        int tos = IPTOS_LOWDELAY;
        if (::setsockopt(s, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0)
            Log::warning() << "setsockopt IP_TOS" << Log::syserr << std::endl;
    }

    if (opts.tcpNoDelay()) {
        int flag = 1;
        if (::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
            Log::warning() << "setsockopt TCP_NODELAY" << Log::syserr << std::endl;
    }

    receiveBufferSize_ = receiveBufferSize_ ? receiveBufferSize_ : opts.receiveBufferSize();
    if (receiveBufferSize_) {
        set_socket_buffer_size(s, "SO_RCVBUF", SO_RCVBUF, receiveBufferSize_);
    }

    sendBufferSize_ = sendBufferSize_ ? sendBufferSize_ : opts.sendBufferSize();
    if (sendBufferSize_) {
        set_socket_buffer_size(s, "SO_SNDBUF", SO_SNDBUF, sendBufferSize_);
    }

    sockaddr_in sin;
    ::memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_port   = htons(localPort_);
    sin.sin_family = AF_INET;

    std::string addr = bindingAddress();

    if (addr.length() == 0)
        sin.sin_addr.s_addr = INADDR_ANY;
    else
        sin.sin_addr.s_addr = ::inet_addr(addr.c_str());

    while (::bind(s, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) == -1) {
        Log::warning() << "bind port " << localPort_ << " " << addr << Log::syserr << std::endl;
        ::sleep(5);
    }

    AutoLock<Mutex> lock(local_mutex);
#ifdef SGI
    int len = sizeof(sin);
#else
    socklen_t len = sizeof(sin);
#endif
    ::getsockname(s, reinterpret_cast<sockaddr*>(&sin), &len);
    if (localPort_ != 0) {
        int gotPort = ntohs(sin.sin_port);
        if (localPort_ != gotPort) {
            std::ostringstream msg;
            msg << "TCPSocket::newSocket() asking for port " << localPort_ << " but got " << gotPort << std::endl;
            throw eckit::SeriousBug(msg.str(), Here());
        }
    }

    localPort_ = ntohs(sin.sin_port);
    localAddr_ = sin.sin_addr;
    localHost_ = addrToHost(sin.sin_addr);

    if (localHost_ == "0.0.0.0") {
        if (addr.length() == 0) {
            AutoLock<Mutex> lock(local_mutex);
            localHost_ = Resource<std::string>("host", "");
            if (localHost_.length() == 0) {
                localHost_ = eckit::Main::hostname();
            }
        }
        else {
            localHost_ = addr;
        }
    }

    // Set the socket 'close on exec'
    SYSCALL(fcntl(s, F_SETFD, FD_CLOEXEC));

    return s;
}

void TCPSocket::bind() {}


static std::map<uint32_t, std::string> cache;

std::string TCPSocket::addrToHost(in_addr addr) {
    AutoLock<Mutex> lock(local_mutex);

    // For some reason IBM's gethostbyaddr_r dumps core
    // from time to time, so let's cache the result
    // to minimise the cores

    std::map<uint32_t, std::string>::iterator j = cache.find(addr.s_addr);

    if (j != cache.end())
        return (*j).second;

    hostent* h;

// For some reason, sgi verion of gethostbyname_r is broken
#ifdef _AIX


    hostent_data data;
    hostent host;

    // Due to a STUPID aix bug we need to clear
    zero(host);
    zero(data);

    if (gethostbyaddr_r(reinterpret_cast<char*>(&addr), sizeof(addr), AF_INET, &host, &data))
        h = 0;
    else
        h = &host;
#else
    h = gethostbyaddr(reinterpret_cast<char*>(&addr), sizeof(addr), AF_INET);
#endif

    std::string s      = h ? std::string(h->h_name) : IPAddress(addr).asString();
    cache[addr.s_addr] = s;
    return s;
}


std::string TCPSocket::hostName(const std::string& h, bool full) {
    in_addr_t addr = ::inet_addr(h.c_str());
    if (addr == (in_addr_t)-1) {
        if (full)
            return h;
        else
            return h.substr(0, h.find('.'));
    }

    struct in_addr a;
    a.s_addr      = addr;
    std::string s = addrToHost(a);

    if (!full && !isdigit(s[0])) {
        return s.substr(0, s.find('.'));
    }

    return s;
}

void TCPSocket::register_ignore_sigpipe() {
#if 0
    ::signal(SIGPIPE, SIG_IGN);
#else
    struct sigaction act;
    eckit::zero(act);
    act.sa_handler = SIG_IGN;
    act.sa_flags = SA_RESTART;
    SYSCALL(::sigaction(SIGPIPE, &act, nullptr));  //< shouldn't fail -- see ERROR conditions in man(2) sigaction
#endif
}

int TCPSocket::socket() {
    return socket_;
}

in_addr TCPSocket::remoteAddr() const {
    return remoteAddr_;
}

const std::string& TCPSocket::remoteHost() const {
    return remoteHost_;
}

int TCPSocket::remotePort() const {
    return remotePort_;
}

in_addr TCPSocket::localAddr() const {
    ((net::TCPSocket*)this)->bind();
    return localAddr_;
}

const std::string& TCPSocket::localHost() const {
    ((net::TCPSocket*)this)->bind();
    return localHost_;
}

int TCPSocket::localPort() const {
    ((net::TCPSocket*)this)->bind();
    return localPort_;
}

std::string TCPSocket::bindingAddress() const {
    return "";
}

long TCPSocket::rawRead(void* buf, long length) {
    return ::read(socket_, buf, length);
}

bool TCPSocket::stillConnected() const {
    if (socket_ == -1)
        return false;

    fd_set r;
    fd_set e;
    fd_set w;

    FD_ZERO(&r);
    FD_SET(socket_, &r);
    FD_ZERO(&e);
    FD_SET(socket_, &e);
    FD_ZERO(&w);
    FD_SET(socket_, &w);

    ::timeval tv = {0, 0};

    if (::select(socket_ + 1, &r, &w, &e, &tv) >= 0) {
        if (!FD_ISSET(socket_, &r))
            return true;

        int n = 0;
        if (::ioctl(socket_, FIONREAD, &n) < 0) {
            Log::info() << "TCPSocket::stillConnected(FIONREAD) failed " << Log::syserr << std::endl;
            return false;
        }

        if (n == 0) {
            Log::warning() << "TCPSocket::stillConnected => connection lost" << std::endl;
            return false;
        }

        return true;
    }
    else {
        Log::info() << "TCPSocket::stillConnected(select) failed " << Log::syserr << std::endl;
        return false;
    }
}

void TCPSocket::debug(bool on) {
    debug_   = on;
    newline_ = true;
    mode_    = 0;
}

void TCPSocket::print(std::ostream& s) const {
    s << "TCPSocket[fd=" << socket_ << ",remote[" << remoteHost() << ":" << remotePort() << " (" << remoteAddr_ << ")]"
      << ",local[" << localHost() << ":" << localPort() << " (" << localAddr_
      << ")]"
      // << ",localPort=" << localPort_
      // << ",remotePort=" << remotePort_
      // << ",remoteHost=" << remoteHost_
      // << ",remoteAddr=" << remoteAddr_
      // << ",localHost=" << localHost_
      // << ",localAddr_=" << localAddr_
      << "]";
}

std::ostream& operator<<(std::ostream& s, in_addr a) {
    s << IPAddress(a);
    return s;
}


}  // namespace net
}  // namespace eckit
