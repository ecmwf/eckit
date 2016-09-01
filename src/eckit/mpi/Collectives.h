/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Collectives_h
#define eckit_mpi_Collectives_h

#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
class Collectives {

    friend Comm;

private: // methods

    ///
    /// All reduce a pair
    ///

    static void allReduce(const eckit::mpi::Comm& comm, const std::pair<T,int>& send, std::pair<T,int>& recv, Operation::Code op)  {
        NOTIMP;
    }

    ///
    /// All reduce a vector of pairs
    ///

    static void allReduce(const eckit::mpi::Comm& comm, const std::vector< std::pair<T,int> >& send, std::vector< std::pair<T,int> >& recv, Operation::Code op)  {
        NOTIMP;
    }

    ///
    /// All to all vector of vector
    ///
    static void allToAll(const eckit::mpi::Comm& comm, const std::vector< std::vector<T> >& sendvec, std::vector< std::vector<T> >& recvvec)  {

        size_t mpi_size = comm.size();

        ASSERT( sendvec.size() == mpi_size );
        ASSERT( recvvec.size() == mpi_size );

        // Get send-information
        std::vector<int> sendcounts(mpi_size);
        std::vector<int> senddispls(mpi_size);
        int sendcnt;
        senddispls[0] = 0;
        sendcounts[0] = sendvec[0].size();
        sendcnt = sendcounts[0];

        for(size_t jproc = 1; jproc < mpi_size; ++jproc)
        {
            senddispls[jproc] = senddispls[jproc-1] + sendcounts[jproc-1];
            sendcounts[jproc] = sendvec[jproc].size();
            sendcnt += sendcounts[jproc];
        }


        // Get recv-information
        std::vector<int> recvcounts(mpi_size);
        std::vector<int> recvdispls(mpi_size);
        int recvcnt;

        comm.allToAll(sendcounts.data(), 1, recvcounts.data(), 1);

        recvdispls[0] = 0;
        recvcnt = recvcounts[0];
        for(size_t jproc = 1; jproc < mpi_size; ++jproc)
        {
            recvdispls[jproc] = recvdispls[jproc-1] + recvcounts[jproc-1];
            recvcnt += recvcounts[jproc];
        }

        // Communicate
        std::vector<T> sendbuf(sendcnt);
        std::vector<T> recvbuf(recvcnt);
        int cnt = 0;
        for(size_t jproc = 0; jproc < mpi_size; ++jproc)
        {
            for(int i = 0; i < sendcounts[jproc]; ++i)
            {
                sendbuf[cnt++] = sendvec[jproc][i];
            }
        }

        comm.allToAllv( sendbuf.data(), sendcounts.data(), senddispls.data(),
                        recvbuf.data(), recvcounts.data(), recvdispls.data());

        cnt=0;
        for(size_t jproc = 0; jproc < mpi_size; ++jproc)
        {
            recvvec[jproc].resize(recvcounts[jproc]);
            for(int i = 0; i < recvcounts[jproc]; ++i)
            {
                recvvec[jproc][i] = recvbuf[cnt++];
            }
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
