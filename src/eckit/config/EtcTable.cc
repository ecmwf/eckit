/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/AutoLock.h"
#include "eckit/config/EtcTable.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/parser/Tokenizer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static const std::vector<std::string> empty;

EtcTable::EtcTable(const std::string& name, int size, const std::string& dir):
    last_(0),
    dir_(dir),
    name_(name),
    size_(size)
{
}

EtcTable::~EtcTable()
{
}


const std::vector<std::string>& EtcTable::lookUp(const std::string& name)
{
    AutoLock<Mutex> lock(mutex_);
    if(last_ == 0) 
        load();
    
    for(std::vector<std::vector<std::string> >::const_iterator j = lines_.begin(); j != lines_.end() ; ++j)
    {
        const std::vector<std::string>& line = *j;
        if(match(name, line)) 
            return line;
    }

    return empty;

}

std::vector<std::string> EtcTable::keys()
{
    AutoLock<Mutex> lock(mutex_);
    if(last_ == 0) 
        load();

    std::vector<std::string> v;
    
    for(std::vector<std::vector<std::string> >::const_iterator j = lines_.begin(); j != lines_.end() ; ++j)
    {
        const std::vector<std::string>& line = *j;
        v.push_back(line[0]);
    }

    return v;
}

void EtcTable::load()
{

    last_ = 1; // TODP: Check timestamp

	LocalPathName path(std::string("~/") + dir_ + "/" + name_);
    std::ifstream in(path.localPath());

    Log::info() << "EtcTable::load " << path << std::endl;

    lines_.clear();

	//Log::info() << "Loading table " << path << std::endl;

	if(in.bad())
	{
		Log::error() << path << Log::syserr << std::endl;
		return;
	}

	char line[1024];
	while(in.getline(line,sizeof(line)))
	{
		Tokenizer parse(" ");
		std::vector<std::string> s;
		parse(line,s);

		size_t i = 0;
        while( i < s.size() )
		{
			if(s[i].length() == 0)
				s.erase(s.begin()+i);
			else
				i++;
		}

		if(s.size() == 0 || s[0][0] == '#')
			continue;

        if(size_ && s.size() != size_)
            Log::warning() << "Ignoring " << line << std::endl;

        lines_.push_back(s);
	}

}

//-----------------------------------------------------------------------------

} // namespace eckit
