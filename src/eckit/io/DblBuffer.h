/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Feb 97

#ifndef eckit_DblBuffer_h
#define eckit_DblBuffer_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/TransferWatcher.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {


class DblBuffer : private NonCopyable {
public:

    // -- Contructors

    DblBuffer(long count = 5, long size = 1024 * 1024, TransferWatcher& = TransferWatcher::dummy());

    // -- Destructor

    ~DblBuffer();

    // -- Methods

    Length copy(DataHandle&, DataHandle&);

    bool error();
    void error(const std::string&);
    void restart(RestartTransfer&);

private:  // methods

    Length copy(DataHandle&, DataHandle&, const Length&);

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

    friend class DblBufferTask;
};


}  // namespace eckit

#endif
