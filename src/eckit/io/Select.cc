/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <sys/ioctl.h>
#include <unistd.h>

#include "eckit/io/Select.h"
#include "eckit/net/TCPSocket.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Select::Select() :
    last_(-1) {
    FD_ZERO(&files_);
}

Select::Select(net::TCPSocket& p) :
    last_(-1) {
    FD_ZERO(&files_);
    add(p);
}

Select::Select(int fd) :
    last_(-1) {
    FD_ZERO(&files_);
    add(fd);
}

Select::~Select() {}

void Select::add(int fd) {
    ASSERT(fd >= 0 && fd < getdtablesize());
    FD_SET(fd, &files_);
    if (fd > last_)
        last_ = fd;
}

void Select::add(net::TCPSocket& p) {
    add(p.socket());
}

void Select::remove(int fd) {
    ASSERT(fd >= 0 && fd < getdtablesize());
    FD_CLR(fd, &files_);
}

void Select::remove(net::TCPSocket& p) {
    remove(p.socket());
}

bool Select::set(int fd) {
    ASSERT(fd >= 0 && fd < getdtablesize());
    return FD_ISSET(fd, &set_);
}

bool Select::set(net::TCPSocket& p) {
    return set(p.socket());
}

bool Select::ready(long sec) {

    int size = last_ + 1;

    ::timeval timeout;
    timeout.tv_sec  = sec;
    timeout.tv_usec = 0;

    for (;;) {

        // First check with ioctl, as select is not always trustworthy

        bool some = false;
        FD_ZERO(&set_);

        for (int i = 0; i < size; i++)
            if (FD_ISSET(i, &files_)) {
                int nbytes = 0;

                // cout << "ioctl(i,FIONREAD,&nbytes) " << i << " .... " << ioctl(i,FIONREAD,&nbytes) << " " << nbytes
                // << std::endl;
                nbytes = 0;


                // SYSCALL(ioctl(i,FIONREAD,&nbytes));
                //

                // On Linux, a socket in acceoct() mode will return "Invalid argument"
                // so we simply ignor ethe error here....
                if ((ioctl(i, FIONREAD, &nbytes) == 0) && (nbytes > 0)) {
                    FD_SET(i, &set_);
                    some = true;
                }
            }

        if (some)
            return true;

        for (;;) {

            set_         = files_;
            fd_set excep = files_;

            switch (::select(size, &set_, 0, &excep, &timeout)) {
                case -1:
                    if (errno != EINTR)
                        throw FailedSystemCall("select");
                    break;

                case 0:
                    return false;

                default:
                    return true;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
