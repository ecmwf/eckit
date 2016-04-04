/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/io/FileHandle.h"

#include "ReadTextFileHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

ReadTextFileHandler::ReadTextFileHandler(const std::string& name) : RequestHandler(name) {}

// read_text_file, _ = <path> 
Values ReadTextFileHandler::handle(ExecutionContext& context)
{
    vector<string> paths (context.environment().lookupList("_", context));

    List r;

    for (size_t i(0); i < paths.size(); ++i)
        r.append(readFile(paths[i]));

    return r;
}

std::string ReadTextFileHandler::readFile(const PathName& fileName)
{
	const size_t CHUNK_SIZE = 1024 * 4;
	char buffer[CHUNK_SIZE]; 

	FileHandle f(fileName);
	Length estimated = f.openForRead();
	
	std::string ret;
	size_t read, totalRead = 0;

	while ( (read = f.read(buffer, sizeof(buffer) / sizeof(char))) > 0 )
	{
		totalRead += read;
		ret.append(std::string(static_cast<char*>(buffer), read));
	}

	f.close();
	return ret;
}

} // namespace eckit
