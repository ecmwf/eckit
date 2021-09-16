/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <numeric>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/mpi/Comm.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::types;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_rank_size") {
    EXPECT_NO_THROW(mpi::comm().size());
    EXPECT_NO_THROW(mpi::comm().rank());
}

CASE("test_broadcast") {
    size_t root = 0;

    int d[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> expect(d, d + 10);

    Log::info() << "Test value" << std::endl;
    {
        float val;
        if (mpi::comm().rank() == root) {
            val = 3.14f;
        }
        float* pval = &val;
        EXPECT_NO_THROW(mpi::comm().broadcast(val, root));
        EXPECT_NO_THROW(mpi::comm().broadcast(pval, pval + 1, root));

        // check results
        EXPECT(is_approximately_equal(val, 3.14f, 0.0001f));
    }
    Log::info() << "Test vector" << std::endl;
    {
        std::vector<int> data(10);
        if (mpi::comm().rank() == root) {
            data.assign(d, d + 10);
        }
        EXPECT_NO_THROW(mpi::comm().broadcast(data, root));

        // check results
        EXPECT(data.size() == 10u);
        EXPECT(data == expect);
    }

    Log::info() << "Test raw data" << std::endl;
    {
        std::vector<int> data(10);
        if (mpi::comm().rank() == root) {
            data.assign(d, d + 10);
        }

        EXPECT_NO_THROW(mpi::comm().broadcast(data.begin(), data.end(), root));

        // check results
        EXPECT(data == expect);
    }

    Log::info() << "Test vector<pair<int,int>>" << std::endl;
    {
        std::vector< std::pair<int,int> > data(5);
        if (mpi::comm().rank() == root) {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                data[i].first=d[j++];
                data[i].second=d[j++];
            }
        }

        EXPECT_NO_THROW(mpi::comm().broadcast(data, root));

        // check results
        std::vector<int> unpacked(10);
        {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                unpacked[j++] = data[i].first;
                unpacked[j++] = data[i].second;
            }
        }
        EXPECT(unpacked == expect);
    }

    Log::info() << "Test vector<pair<long,long>>" << std::endl;
    {
        std::vector< std::pair<long,long> > data(5);
        if (mpi::comm().rank() == root) {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                data[i].first=d[j++];
                data[i].second=d[j++];
            }
        }

        EXPECT_NO_THROW(mpi::comm().broadcast(data, root));

        // check results
        std::vector<int> unpacked(10);
        {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                unpacked[j++] = data[i].first;
                unpacked[j++] = data[i].second;
            }
        }
        EXPECT(unpacked == expect);
    }

    Log::info() << "Test vector<pair<long long,long long>>" << std::endl;
    {
        std::vector< std::pair<long long,long long> > data(5);
        if (mpi::comm().rank() == root) {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                data[i].first=d[j++];
                data[i].second=d[j++];
            }
        }

        EXPECT_NO_THROW(mpi::comm().broadcast(data, root));

        // check results
        std::vector<int> unpacked(10);
        {
            size_t j=0;
            for(size_t i=0; i<5; ++i) {
                unpacked[j++] = data[i].first;
                unpacked[j++] = data[i].second;
            }
        }
        EXPECT(unpacked == expect);
    }

}

CASE("test_gather_scalar") {
    size_t size = mpi::comm().size();
    size_t rank = mpi::comm().rank();

    std::vector<size_t> recv(size);

    size_t send = 777 + rank;

    size_t root = 0; /* master */

    EXPECT_NO_THROW(mpi::comm().gather(send, recv, root));

    if (rank == root) {
        std::vector<size_t> expected(size);
        for (size_t j = 0; j < recv.size(); ++j) {
            expected[j] = 777 + j;
        }

        EXPECT(recv == expected);
    }
}

CASE("test_gather_nscalars") {
    for (size_t N = 1; N < 10; ++N) {

        size_t size = mpi::comm().size();
        size_t rank = mpi::comm().rank();

        std::vector<long> recv(size * N);

        std::vector<long> send(N);
        for (size_t n = 0; n < N; ++n) {
            send[n] = long(rank * 2 + n + 1);
        }

        std::vector<long> expected(size * N);
        for (size_t j = 0; j < size; ++j) {
            for (size_t n = 0; n < N; ++n) {
                expected[j * N + n] = long(j * 2 + n + 1);
            }
        }

        size_t root = 0; /* master */

        EXPECT_NO_THROW(mpi::comm().gather(send, recv, root));

        if (rank == root) {
            EXPECT(recv == expected);
        }

        std::vector<long> recv2(size * N);

        EXPECT_NO_THROW(mpi::comm().gather(send.begin(), send.end(), recv2.begin(), recv2.end(), root));

        if (rank == root) {
            EXPECT(recv2 == expected);
        }
    }
}

CASE("test_gatherv_equal_stride") {
    size_t root = 0;
    size_t size = mpi::comm().size();
    size_t rank = mpi::comm().rank();

    size_t stride = 100;

    std::vector<long> send(stride);

    for (size_t i = 0; i < stride; ++i) {
        send[i] = long(rank * i);
    }

    std::vector<long> recv(size * stride);

    std::vector<int> displs(size);
    std::vector<int> recvcounts(size);

    for (size_t i = 0; i < size; ++i) {
        displs[i]     = int(i * stride);
        recvcounts[i] = int(stride);
    }

    EXPECT_NO_THROW(mpi::comm().gatherv(send, recv, recvcounts, displs, root));

    std::vector<long> expected(size * stride);
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < stride; ++j) {
            expected[i * stride + j] = long(i * j);
        }
    }

    if (rank == root) {
        EXPECT(recv == expected);
    }
}

CASE("test_gatherv_unequal_stride") {
    size_t root = 0;
    size_t size = mpi::comm().size();
    size_t rank = mpi::comm().rank();

    auto stride = [](size_t rank) { return 10 * (rank); };

    std::vector<long> send(stride(rank));

    for (size_t i = 0; i < stride(rank); ++i) {
        send[i] = long(rank * i);
    }

    std::vector<int> displs;
    std::vector<int> recvcounts;

    if (rank == root) {
        // displs and recvcounts only significant at root
        displs.resize(size);
        recvcounts.resize(size);
        for (size_t i = 0; i < size; ++i) {
            recvcounts[i] = int(stride(i));
            displs[i]     = int(i ? displs[i - 1] + recvcounts[i - 1] : 0);
        }
    }
    size_t recvsize = size_t(std::accumulate(recvcounts.begin(), recvcounts.end(), 0));

    std::vector<long> recv(recvsize);

    EXPECT_NO_THROW(mpi::comm().gatherv(send, recv, recvcounts, displs, root));

    if (rank == root) {

        EXPECT(recv.size() == recvsize);

        size_t e = 0;
        std::vector<long> expected(recvsize);
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < stride(i); ++j, ++e) {
                expected[e] = long(i * j);
            }
        }

        EXPECT(recv == expected);
    }
}

CASE("test_scatter_scalar") {
    size_t size = mpi::comm().size();
    std::vector<long> send(size);
    for (size_t j = 0; j < send.size(); ++j) {
        send[j] = long(j * j) - 1;
    }

    long recv = -999;

    size_t root = 0; /* master */

    EXPECT_NO_THROW(mpi::comm().scatter(send, recv, root));

    size_t rank = mpi::comm().rank();

    EXPECT(recv == long(rank * rank) - 1);
}

CASE("test_scatter_nscalars") {
    for (size_t N = 1; N < 4; ++N) {

        size_t size = mpi::comm().size();
        std::vector<long> send(size * N);
        for (size_t j = 0; j < send.size() / N; ++j) {
            for (size_t n = 0; n < N; ++n) {
                send[j * N + n] = long(j * j - n);
            }
        }

        std::vector<long> recv(N);

        size_t root = 0; /* master */

        EXPECT_NO_THROW(mpi::comm().scatter(send, recv, root));

        size_t rank = mpi::comm().rank();

        // check results
        std::vector<long> expected(N);
        for (size_t n = 0; n < N; ++n) {
            expected[n] = long(rank * rank - n);
        }

        EXPECT(recv == expected);
    }
}

CASE("test_scatterv") {
    /// TODO
}

CASE("test_allReduce") {
    int d = int(mpi::comm().rank()) + 1;

    std::pair<double, int> v(-d, mpi::comm().rank());
    std::cout << "v : " << v << std::endl;
    std::cout << std::flush;
    mpi::comm().barrier();

    // check results
    int s = 0;
    int p = 1;
    for (size_t j = 0; j < mpi::comm().size(); ++j) {
        s += (j + 1);
        p *= (j + 1);
    }

    Log::info() << "Testing all_reduce" << std::endl;
    {
        int sum;
        int prod;
        int max;
        int min;

        std::pair<double, int> maxloc;
        std::pair<double, int> minloc;

        EXPECT_NO_THROW(mpi::comm().allReduce(d, sum, mpi::sum()));

        EXPECT(sum == s);

        EXPECT_NO_THROW(mpi::comm().allReduce(d, prod, mpi::prod()));

        EXPECT(prod == p);

        EXPECT_NO_THROW(mpi::comm().allReduce(d, max, mpi::max()));

        EXPECT(size_t(max) == mpi::comm().size());

        EXPECT_NO_THROW(mpi::comm().allReduce(d, min, mpi::min()));

        EXPECT(min == 1);

        EXPECT_NO_THROW(mpi::comm().allReduce(v, maxloc, mpi::maxloc()));

        EXPECT(maxloc.first == -double(1));
        EXPECT(maxloc.second == 0);

        EXPECT_NO_THROW(mpi::comm().allReduce(v, minloc, mpi::minloc()));

        EXPECT(minloc.first == -double(mpi::comm().size()));
        EXPECT(size_t(minloc.second) == mpi::comm().size() - 1);
    }

    std::vector<float> arr(5, mpi::comm().rank() + 1);
    std::cout << "arr : " << arr << std::endl;

    std::cout << std::flush;
    mpi::comm().barrier();

    Log::info() << "Testing all_reduce inplace" << std::endl;
    {
        int sum  = d;
        int prod = d;
        int max  = d;
        int min  = d;

        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(sum, mpi::sum()));
        EXPECT(sum == s);

        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(prod, mpi::prod()));
        EXPECT(prod == p);

        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(max, mpi::max()));
        EXPECT(size_t(max) == mpi::comm().size());

        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(min, mpi::min()));
        EXPECT(min == 1);

        std::vector<float> expected;

        expected                  = std::vector<float>(5, mpi::comm().size());
        std::vector<float> maxvec = arr;
        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(maxvec.begin(), maxvec.end(), mpi::max()));
        EXPECT(maxvec == expected);

        expected                  = std::vector<float>(5, 1);
        std::vector<float> minvec = arr;
        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(minvec.begin(), minvec.end(), mpi::min()));
        EXPECT(minvec == expected);

        expected                  = std::vector<float>(5, s);
        std::vector<float> sumvec = arr;
        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(sumvec.begin(), sumvec.end(), mpi::sum()));
        EXPECT(sumvec == expected);

        expected                   = std::vector<float>(5, p);
        std::vector<float> prodvec = arr;
        EXPECT_NO_THROW(mpi::comm().allReduceInPlace(prodvec.begin(), prodvec.end(), mpi::prod()));
        EXPECT(prodvec == expected);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_allGather") {
    // Scalar

    int send = mpi::comm().rank();
    std::vector<int> recv(mpi::comm().size());

    EXPECT_NO_THROW(mpi::comm().allGather(send, recv.begin(), recv.end()));

    std::vector<int> expected(mpi::comm().size());
    for (size_t j = 0; j < expected.size(); ++j) {
        expected[j] = int(j);
    }

    EXPECT(recv == expected);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_allGatherv") {
    std::vector<int> send(mpi::comm().rank(), mpi::comm().rank());
    mpi::Buffer<int> recv(mpi::comm().size());

    EXPECT_NO_THROW(mpi::comm().allGatherv(send.begin(), send.end(), recv));

    // check results
    std::vector<int> expected;
    for (size_t j = 0; j < mpi::comm().size(); ++j) {
        for (size_t i = 0; i < j; ++i)
            expected.push_back(j);
    }

    EXPECT(recv.buffer == expected);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_allToAll") {
    std::vector<std::vector<int> > send(mpi::comm().size(), std::vector<int>(1, mpi::comm().rank()));
    std::vector<std::vector<int> > recv(mpi::comm().size());

    EXPECT_NO_THROW(mpi::comm().allToAll(send, recv));

    // check results
    std::vector<std::vector<int> > expected(mpi::comm().size());
    for (size_t j = 0; j < mpi::comm().size(); ++j) {
        expected[j] = std::vector<int>(1, int(j));
    }

    EXPECT(recv.size() == expected.size());
    for (size_t i = 0; i < mpi::comm().size(); ++i) {
        EXPECT(recv[i] == expected[i]);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_nonblocking_send_receive") {
    mpi::Comm& comm = mpi::comm("world");
    int tag         = 99;
    mpi::Request sendreq;
    mpi::Request recvreq;
    double send;
    double recv = 1.;

    // Post a receive request
    if (comm.rank() == comm.size() - 1) {
        recvreq = comm.iReceive(recv, 0, tag);
    }

    // Post a send request
    if (comm.rank() == 0) {
        send    = 0.5;
        sendreq = comm.iSend(send, comm.size() - 1, tag);
    }

    // Wait for receiving to finish
    if (comm.rank() == comm.size() - 1) {
        mpi::Status recvstatus = comm.wait(recvreq);
        EXPECT(recvstatus.error() == 0);
        EXPECT(recvstatus.tag() == tag);
        EXPECT(recvstatus.source() == 0);
        EXPECT(is_approximately_equal(recv, 0.5, 1.e-9));
    }
    else {
        EXPECT(is_approximately_equal(recv, 1., 1.e-9));
    }

    // Wait for sending to finish
    if (comm.rank() == 0) {
        mpi::Status sendstatus = comm.wait(sendreq);
        EXPECT(sendstatus.error() == 0);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_blocking_send_nonblocking_receive") {
    mpi::Comm& comm = mpi::comm("world");
    int tag         = 99;
    double send;
    double recv[]       = {1., 1.};
    double recv_check[] = {0.5, 0.8};
    std::vector<mpi::Request> recvreqs;

    // Post 2 receive requests
    if (comm.rank() == 0) {
        recvreqs.push_back(comm.iReceive(&recv[0], 1, comm.size() - 1, tag));
        recvreqs.push_back(comm.iReceive(&recv[1], 1, comm.size() - 1, tag));
    }


    // Two blocking sends
    if (comm.rank() == comm.size() - 1) {
        send = 0.5;
        comm.send(send, 0, tag);

        send = 0.8;
        comm.send(send, 0, tag);
    }

    // Wait for receiving to finish
    for (size_t i = 0; i < recvreqs.size(); ++i) {
        mpi::Status recvstatus = comm.wait(recvreqs[i]);
        EXPECT(recvstatus.error() == 0);
        EXPECT(recvstatus.tag() == tag);
        EXPECT(recvstatus.source() == int(comm.size()) - 1);
        EXPECT(is_approximately_equal(recv[i], recv_check[i], 1.e-9));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_blocking_send_receive") {
    mpi::Comm& comm = mpi::comm("world");
    int tag         = 99;
    double send1, send2;
    double recv1 = 1.;
    double recv2 = 1.;

    // Send1
    if (comm.rank() == 0) {
        send1 = 0.1;
        comm.send(send1, comm.size() - 1, tag);
        send1 = 0.;  // should not matter, as send() copies to internal mpi buffer
    }

    // Send2
    if (comm.rank() == 0) {
        send2 = 0.2;
        comm.send(send2, comm.size() - 1, tag);
    }

    // Receive1
    if (comm.rank() == comm.size() - 1) {
        mpi::Status status = comm.receive(recv1, 0, tag);
        EXPECT(is_approximately_equal(recv1, 0.1, 1.e-9));
    }

    // Receive2
    if (comm.rank() == comm.size() - 1) {
        mpi::Status status = comm.receive(recv2, 0, tag);
        EXPECT(is_approximately_equal(recv2, 0.2, 1.e-9));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_broadcastFile") {
    mpi::Comm& comm = mpi::comm("world");
    size_t root     = 0;

    std::string str = "Hello World!\n";
    LocalPathName path(eckit::Main::instance().name() + "_broadcastFile.txt");
    if (comm.rank() == root) {
        std::ofstream file(path.c_str(), std::ios_base::out);
        file << str;
        file.close();
    }

    EXPECT(comm.broadcastFile(path, root).str() == str);
}

CASE("test_waitAll") {

    auto& comm = mpi::comm("world");
    int nproc  = comm.size();
    int irank  = comm.rank();

    std::vector<mpi::Request> rqr;
    std::vector<mpi::Request> rqs;

    std::vector<int> data(nproc, -1);

    for (int i = 0; i < nproc; i++) {
        rqr.push_back(comm.iReceive(&data[i], 1, i, 100));
    }

    comm.barrier();

    for (int i = 0; i < nproc; i++) {
        rqs.push_back(comm.iSend(&irank, 1, i, 100));
    }

    std::vector<mpi::Status> str = comm.waitAll(rqr);
    for (int i = 0; i < nproc; i++) {
        auto status = str[i];
        EXPECT(status.error() == 0);
        EXPECT(status.tag() == 100);
        EXPECT(status.source() == i);
    }

    // Test case where requests have been handled already
    {
        auto statuses = comm.waitAll(rqr);
        for (int i = 0; i < nproc; i++) {
            auto status = statuses[i];
            EXPECT(status.error() == 0);
            EXPECT(status.tag() == comm.anyTag());
            EXPECT(status.source() == comm.anySource());
        }
    }

    for (int i = 0; i < nproc; i++) {
        EXPECT(i == data[i]);
    }

    for (auto& st : str) {
        EXPECT(st.error() == 0);
    }

    std::vector<mpi::Status> sts = comm.waitAll(rqs);

    for (auto& st : sts) {
        EXPECT(st.error() == 0);
    }
}

CASE("test_waitAny") {

    auto& comm = mpi::comm("world");
    int nproc  = comm.size();
    int irank  = comm.rank();

    std::vector<mpi::Request> rqr;
    std::vector<mpi::Request> rqs;

    std::vector<int> data(nproc, -1);

    for (int i = 0; i < nproc; i++) {
        rqr.push_back(comm.iReceive(&data[i], 1, i, 100));
    }

    comm.barrier();

    for (int i = 0; i < nproc; i++) {
        rqs.push_back(comm.iSend(&irank, 1, i, 100));
    }

    int count = rqr.size();
    while (count > 0) {
        int ireq       = -1;
        mpi::Status st = comm.waitAny(rqr, ireq);
        EXPECT(st.error() == 0);
        EXPECT(st.tag() == 100);
        EXPECT(st.source() == ireq);
        count--;
    }

    // Test case where requests have been handled already
    {
        int ireq;
        mpi::Status st = comm.waitAny(rqr, ireq);
        EXPECT(st.error() == 0);
        EXPECT(st.source() == comm.anySource());
        EXPECT(st.tag() == comm.anyTag());
        EXPECT(ireq == comm.undefined());
    }

    for (int i = 0; i < nproc; i++) {
        EXPECT(i == data[i]);
    }

    std::vector<mpi::Status> sts = comm.waitAll(rqs);

    for (auto& st : sts) {
        EXPECT(st.error() == 0);
    }
}

CASE("test_probe") {

    auto& comm = mpi::comm("world");
    int nproc  = comm.size();
    int irank  = comm.rank();

    std::vector<mpi::Request> requests;

    std::vector<int> data(nproc, -1);

    for (int i = 0; i < nproc; i++) {
        requests.push_back(comm.iSend(&irank, 1, i, 100));
    }

    comm.barrier();

    auto count = nproc;
    while (count > 0) {
        auto status = comm.probe(comm.anySource(), comm.anyTag());
        auto sz     = comm.getCount<int>(status);

        EXPECT(status.error() == 0);
        EXPECT(status.tag() == 100);
        EXPECT(sz == 1);

        comm.receive(&data[status.source()], sz, status.source(), status.tag());
        --count;
    }

    for (int i = 0; i < nproc; i++) {
        EXPECT(i == data[i]);
    }

    std::vector<mpi::Status> sts = comm.waitAll(requests);

    for (auto& st : sts) {
        EXPECT(st.error() == 0);
    }
}

CASE("test_iProbe") {

    auto& comm = mpi::comm("world");
    int nproc  = comm.size();
    int irank  = comm.rank();

    std::vector<mpi::Request> requests;

    std::vector<int> data(nproc, -1);

    for (int i = 0; i < nproc; i++) {
        requests.push_back(comm.iSend(&irank, 1, i, 100));
    }

    comm.barrier();

    auto count = nproc;
    while (count > 0) {
        auto status = comm.iProbe(comm.anySource(), comm.anyTag());
        if (not status) {
            continue;
        }

        auto sz = comm.getCount<int>(status);

        EXPECT(status.error() == 0);
        EXPECT(status.tag() == 100);
        EXPECT(sz == 1);

        comm.receive(&data[status.source()], sz, status.source(), status.tag());

        --count;
    }

    for (int i = 0; i < nproc; i++) {
        EXPECT(i == data[i]);
    }

    std::vector<mpi::Status> sts = comm.waitAll(requests);

    for (auto& st : sts) {
        EXPECT(st.error() == 0);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    int failures = run_tests(argc, argv);
    eckit::mpi::finaliseAllComms();
    return failures;
}
