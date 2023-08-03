/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/distributed/TransportStatistics.h"
#include "eckit/serialisation/Stream.h"


namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

TransportStatistics::TransportStatistics():
    sendCount_(0),
    receiveCount_(0),
    sendSize_(0),
    receiveSize_(0)
{
}

TransportStatistics::TransportStatistics(eckit::Stream &s) {

    s >> sendCount_;
    s >> receiveCount_;
    s >> sendSize_;
    s >> receiveSize_;
    s >> sendTiming_;
    s >> receiveTiming_;
    s >> barrierTiming_;
    s >> shutdownTiming_;
}

void TransportStatistics::encode(eckit::Stream &s) const {

    s << sendCount_;
    s << receiveCount_;
    s << sendSize_;
    s << receiveSize_;
    s << sendTiming_;
    s << receiveTiming_;
    s << barrierTiming_;
    s << shutdownTiming_;
}

TransportStatistics &TransportStatistics::operator+=(const TransportStatistics &other) {

    sendCount_ += other.sendCount_;
    receiveCount_ += other.receiveCount_;
    sendSize_ += other.sendSize_;
    receiveSize_ += other.receiveSize_;
    sendTiming_ += other.sendTiming_;
    receiveTiming_ += other.receiveTiming_;
    barrierTiming_ += other.barrierTiming_;
    shutdownTiming_ += other.shutdownTiming_;

    return *this;
}

inline static void divide(size_t& x, size_t n) {
    x = (x + 0.5) / n;
}

TransportStatistics &TransportStatistics::operator/=(size_t n) {

    divide(sendCount_, n);
    divide(receiveCount_, n);
    receiveSize_ /= n;
    sendSize_ /= n;
    sendTiming_ /= n;
    receiveTiming_ /= n;
    barrierTiming_ /= n;
    shutdownTiming_ /= n;

    return *this;
}

void TransportStatistics::report(std::ostream &out, const char *indent) const {
    reportCount(out, "Transport: messages sent", sendCount_, indent);
    reportBytes(out, "Transport: byte sent", sendSize_, indent);
    reportTime(out, "Transport: time in send", sendTiming_, indent);
    if (sendTiming_.elapsed_) {
        reportRate(out, "Transport: send rate", sendSize_ / sendTiming_.elapsed_, indent);
    }

    reportCount(out, "Transport: messages received", receiveCount_, indent);
    reportBytes(out, "Transport: byte received", receiveSize_, indent);
    reportTime(out, "Transport: time in receive", receiveTiming_, indent);
    if (receiveTiming_.elapsed_) {
        reportRate(out, "Transport: receive rate", receiveSize_ / receiveTiming_.elapsed_, indent);
    }

    reportTime(out, "Transport: barrier", barrierTiming_, indent);
    reportTime(out, "Transport: shutdown", shutdownTiming_, indent);

}

void TransportStatistics::csvHeader(std::ostream& out) const {
    out << "sends,sendSize,send,receives,receiveSize,receive,barrier,shutdown";
}

void TransportStatistics::csvRow(std::ostream& out) const {
    out << sendCount_ << ","
        << sendSize_ << ","
        << sendTiming_ << ","
        << receiveCount_ << ","
        << receiveSize_ << ","
        << receiveTiming_ << ","
        << barrierTiming_ << ","
        << shutdownTiming_;
}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
