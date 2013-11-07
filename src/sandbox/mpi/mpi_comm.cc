/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/runtime/Context.h"

#include "eckit/mpi/Comm.h"
#include "eckit/grib/GribFieldSet.h"
#include "eckit/grib/GribField.h"
#include "eckit/grib/GribCompute.h"


using namespace eckit;

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

class TestMPIComm : public Tool {
public:

    TestMPIComm(int argc,char **argv): Tool(argc,argv) {
        mpi::Comm::instance().init( Context::instance().argc(), Context::instance().argvs() );
    }

    ~TestMPIComm() {
        mpi::Comm::instance().finalize();
    }

    virtual void run();
};

//-----------------------------------------------------------------------------

void TestMPIComm::run()
{
    mpi::Comm& comm = mpi::Comm::instance();

    ASSERT( comm.isInitialised() );
    ASSERT( comm.isActive() );

    Log::info() << "Hello World from process "
                << comm.rank()
                << " of "
                << comm.size() << std::endl;

    GribFieldSet fs("/tmp/data.grib");
    Log::info() << fs << endl;

    GribFieldSet m;
    compute::mean(fs, m);

    Log::info() << m << endl;
    m.write("/tmp/mean.grib");
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestMPIComm app(argc,argv);
    app.start();
    return 0;
}

