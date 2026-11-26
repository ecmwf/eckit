// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Feb 97

#ifndef eckit_DblBuffer_h
#define eckit_DblBuffer_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/TransferWatcher.h"
#include "eckit/thread/Mutex.h"


namespace eckit {


class DblBuffer {
public:

    // -- Contructors

    DblBuffer(long count = 5, long size = 1024 * 1024, TransferWatcher& = TransferWatcher::dummy());

    DblBuffer(const DblBuffer&)            = delete;
    DblBuffer& operator=(const DblBuffer&) = delete;
    DblBuffer(DblBuffer&&)                 = delete;
    DblBuffer& operator=(DblBuffer&&)      = delete;

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
