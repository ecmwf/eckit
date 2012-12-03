/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/AutoLock.h"
#include "eclib/EtcTable.h"
#include "eclib/LocalPathName.h"
#include "eclib/Tokenizer.h"

static const vector<string> empty;

EtcTable::EtcTable(const string& name, int size, const string& dir):
    last_(0),
    dir_(dir),
    name_(name),
    size_(size)
{
}

EtcTable::~EtcTable()
{
}


const vector<string>& EtcTable::lookUp(const string& name)
{
    AutoLock<Mutex> lock(mutex_);
    if(last_ == 0) 
        load();
    
    for(vector<vector<string> >::const_iterator j = lines_.begin(); j != lines_.end() ; ++j)
    {
        const vector<string>& line = *j;
        if(match(name, line)) 
            return line;
    }

    return empty;

}

vector<string> EtcTable::keys()
{
    AutoLock<Mutex> lock(mutex_);
    if(last_ == 0) 
        load();

    vector<string> v;
    
    for(vector<vector<string> >::const_iterator j = lines_.begin(); j != lines_.end() ; ++j)
    {
        const vector<string>& line = *j;
        v.push_back(line[0]);
    }

    return v;
}

void EtcTable::load()
{

    last_ = 1; // TODP: Check timestamp

	LocalPathName path(string("~/") + dir_ + "/" + name_);
	ifstream in(path.localPath());

    Log::info() << "EtcTable::load " << path << endl;

    lines_.clear();

	//Log::info() << "Loading table " << path << endl;

	if(in.bad())
	{
		Log::error() << path << Log::syserr << endl;
		return;
	}

	char line[1024];
	while(in.getline(line,sizeof(line)))
	{
		Tokenizer parse(" ");
		vector<string> s;
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
            Log::warning() << "Ignoring " << line << endl;

        lines_.push_back(s);
	}

}

