// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Pipeline.h
// Baudouin Raoult - ECMWF Feb 97

#ifndef eckit_Pipeline_h
#define eckit_Pipeline_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/TransferWatcher.h"
#include "eckit/thread/Mutex.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Pipeline {
public:

    // -- Contructors

    Pipeline(TransferWatcher& = TransferWatcher::dummy());

    Pipeline(const Pipeline&)            = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&)                 = delete;
    Pipeline& operator=(Pipeline&&)      = delete;

    // -- Destructor

    virtual ~Pipeline();

    // -- Methods

    Length copy(DataHandle&, DataHandle&);

    bool error();
    void error(const std::string&);
    void restart(RestartTransfer&);


private:

    virtual void execute(DataHandle& in, DataHandle& out) = 0;


private:  // members

    Mutex mutex_;

    long count_;
    long bufSize_;

    Length inBytes_;
    Length outBytes_;

    bool error_;
    std::string why_;

    bool restart_;

    Offset restartFrom_;
    TransferWatcher& watcher_;

    // -- Friends

    friend class PipelineExecutor;
    friend class PipelineReader;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
