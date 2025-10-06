/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Pipeline.h
// Baudouin Raoult - ECMWF Feb 97

#ifndef eckit_Pipeline_h
#define eckit_Pipeline_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/TransferWatcher.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Pipeline : private NonCopyable {
public:

    // -- Contructors

    Pipeline(TransferWatcher& = TransferWatcher::dummy());

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
