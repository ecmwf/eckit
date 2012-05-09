/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StdPipe.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef StdPipe_H
#define StdPipe_H

#include <stdio.h>
#include <string>


// Simple wrapper around a stdio file

class StdPipe {
public:

// -- Contructors

	StdPipe(const std::string& name,const std::string& mode = "r");

// -- Destructor

	~StdPipe();

// -- Convertors

	operator FILE*() { return file_; } // don't call fclose !!!

private:

// No copy allowed

	StdPipe(const StdPipe&);
	StdPipe& operator=(const StdPipe&);

// -- Members

	FILE *file_;

};

#endif
