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
/// @date   June 2017

#ifndef eckit_filesystem_URI_h
#define eckit_filesystem_URI_h

#include "eckit/eckit.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class DataHandle;

class URI {

public: // methods

	URI(const std::string&);

	~URI();

    bool exists() const;

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;

	const std::string& name() const { return name_; }
	const std::string& scheme() const { return scheme_; }

protected: // methods

	void print(std::ostream&) const;

private: // members

	std::string name_;
	std::string scheme_;

    friend std::ostream& operator<<(std::ostream& s,const URI& p) { p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
