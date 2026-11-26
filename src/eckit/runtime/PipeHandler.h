// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 96

#ifndef eckit_PipeHandler_h
#define eckit_PipeHandler_h

#include "eckit/container/ClassExtent.h"
#include "eckit/runtime/Dispatcher.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/serialisation/PipeStream.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class Request>
class PipeHandler : public ProcessControler, public ClassExtent<PipeHandler<Request> >, public DefaultHandler<Request> {
public:

    PipeHandler();
    ~PipeHandler();

    void handle(const std::vector<Request*>&);
    void pick(std::list<Request*>&, std::vector<Request*>&);
    void idle();

    bool busy() const { return busy_; }
    void ready(bool&);
    void age(time_t&);

    void start() override;
    void stop() override;

protected:  // methods

    bool canPick();

private:  // methods

    virtual void endBatch(Stream&);

    void send(Request*);
    void receive(Request*);

    void run() override;

private:  // members

    PipeStream* pipe_;
    bool busy_;
    time_t last_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/runtime/PipeHandler.cc"

#endif
