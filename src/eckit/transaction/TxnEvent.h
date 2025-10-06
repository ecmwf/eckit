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
/// @date Jan 97

#ifndef eckit_TxnEvent_h
#define eckit_TxnEvent_h

#include "eckit/serialisation/Streamable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef unsigned long long TxnID;

class TxnEvent : public Streamable {
public:

    // -- Contructors

    TxnEvent();
    TxnEvent(Stream&);

    // -- Destructor

    ~TxnEvent() override;

    // -- Methods

    const TxnID& transactionID() const { return txnID_; }
    void transactionID(const TxnID& id) { txnID_ = id; }

    // -- Overridden methods

    // From Streamble

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }


    // -- Class members

    static const ClassSpec& classSpec() { return classSpec_; }

protected:

    // -- Methods

    virtual void print(std::ostream&) const;

private:

    // No copy allowed

    TxnEvent(const TxnEvent&);
    TxnEvent& operator=(const TxnEvent&);

    // -- Members

    TxnID txnID_;

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<TxnEvent> reanimator_;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const TxnEvent& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
