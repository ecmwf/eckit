/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClusterDisks.cc
// Baudouin Raoult - (c) ECMWF Jul 11


#include "eclib/AutoLock.h"
#include "eclib/ClusterDisks.h"
#include "eclib/FileSpace.h"
#include "eclib/JSON.h"
#include "eclib/LocalPathName.h"
#include "eclib/MappedArray.h"
#include "eclib/NodeInfo.h"
#include "eclib/StrStream.h"
#include "eclib/Tokenizer.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class ClusterDisk {
	bool active_;
	bool offLine_;
	time_t lastSeen_;
	char node_[256];
	char type_[256];
	char path_[2048];

public:
	ClusterDisk(const string& node, const string& type, const string& path) :
		active_(true), offLine_(false), lastSeen_(::time(0))
	{
		zero(node_);
		strncpy(node_, node.c_str(), sizeof(node_) - 1);
		zero(type_);
		strncpy(type_, type.c_str(), sizeof(type_) - 1);
		zero(path_);
		strncpy(path_, path.c_str(), sizeof(path_) - 1);
	}

	bool operator<(const ClusterDisk& other) const
	{
		if(strcmp(path_, other.path_) < 0)
			return true;
		return false;
	}

	void active(bool on)
	{
		active_ = on;
	}
	void offLine(bool on)
	{
		offLine_ = on;
	}

	bool active() const
	{
		return active_;
	}
	bool offLine() const
	{
		return offLine_;
	}
	time_t lastSeen() const
	{
		return lastSeen_;
	}
	void lastSeen(time_t n)
	{
		lastSeen_ = n;
	}

	const char* node() const
	{
		return node_;
	}
	const char* type() const
	{
		return type_;
	}
	const char* path() const
	{
		return path_;
	}

	friend ostream& operator<<(ostream&, const ClusterDisk&);

	void json(JSON& s) const
	{
		s.startObject();
		s << "lastSeen" << lastSeen_;
		s << "offLine" << offLine_;
		s << "node" << node_;
		s << "type" << type_;
		s << "path" << path_;
		s.endObject();
	}

};
ostream& operator<<(ostream& s, const ClusterDisk& d)
{
	s << "ClusterDisk[" << d.node_ << "," << d.type_ << "," << d.path_ << "," << (::time(0) - d.lastSeen_) << 
        "," << (d.offLine_ ? "off" : "on") << "-line" <<
        "]";
	return s;
}

inline unsigned long version(ClusterDisk*)
{
	return 1;
}

typedef MappedArray<ClusterDisk> DiskArray;
static DiskArray* clusterDisks = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static vector<string> localPaths;

static void init()
{
	LocalPathName path("~/etc/cluster/disks"); // Avoid recursion...
	clusterDisks = new DiskArray(path, 10240);
}



void ClusterDisks::reset()
{
	pthread_once(&once, init);
	AutoLock<DiskArray> lock(*clusterDisks);

	for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
		(*k).active(false);
}

void ClusterDisks::cleanup()
{
    /*
	pthread_once(&once, init);
	AutoLock<DiskArray> lock(*clusterDisks);

	for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
        if((*k).active() && (*k).offline())
        {
            Log::info() << "Forget " << (*k) << endl;
            (*k).active(false);
        }
    */
    reset();
}

void ClusterDisks::forget(const NodeInfo& info)
{
	if(info.name() == "marsfs")
	{
		time_t now = ::time(0);
		pthread_once(&once, init);
		AutoLock<DiskArray> lock(*clusterDisks);

		for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
		{
			if(info.node() == (*k).node())
			{
				(*k).active(false);
			}

			(*k).lastSeen(now);
		}
	}
}

void ClusterDisks::offLine(const NodeInfo& info)
{
	if(info.name() == "marsfs")
	{
		time_t now = ::time(0);
		pthread_once(&once, init);
		AutoLock<DiskArray> lock(*clusterDisks);

		//cout << "=========== ClusterDisks::forget "  << info << endl;

		for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
		{
			if(info.node() == (*k).node())
			{
				(*k).offLine(true);
			}
			(*k).lastSeen(now);
		}
	}
}

void ClusterDisks::update(const string& node, const string& type, const vector<string>& disks)
{

	pthread_once(&once, init);

	AutoLock<DiskArray> lock(*clusterDisks);

	for(DiskArray::iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if(type == (*k).type() && node == (*k).node())
		{
			(*k).active(false);
		}
	}

	for(vector<string>::const_iterator j = disks.begin(); j != disks.end(); ++j)
	{
		ClusterDisk c(node, type, *j);
		DiskArray::iterator k = lower_bound(clusterDisks->begin(), clusterDisks->end(), c);
		if(k != clusterDisks->end() && !(c < *k))
		{
			// Log::info() << "=========== Update " << node << "-" << type << " " << *j << endl;
			// TODO check for change of node or type
			*k = c;

		}
		else
		{
			ASSERT(!(*clusterDisks)[0].active());
			//	Log::info() << "================ New " << node << "-" << type << " " << * j << endl;
			(*clusterDisks)[0] = c;
			std::sort(clusterDisks->begin(), clusterDisks->end());
		}
	}

}

void ClusterDisks::list(ostream& out)
{
	pthread_once(&once, init);

	AutoLock<DiskArray> lock(*clusterDisks);
	for(DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if((*k).active())
			out << *k << endl;
	}
}

void ClusterDisks::json(JSON& j)
{
	pthread_once(&once, init);

	j.startList();

	AutoLock<DiskArray> lock(*clusterDisks);
	for(DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if((*k).active())
		{
			(*k).json(j);
		}
	}

	j.endList();

}

time_t ClusterDisks::lastModified(const string& type)
{

	pthread_once(&once, init);

	AutoLock<DiskArray> lock(*clusterDisks);

	time_t last = 0;

	for(DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if((*k).active() && ((*k).type() == type))
			last = ::max(last, (*k).lastSeen());
	}

	return last;
}

void ClusterDisks::load(const string& type, vector<string>& disks)
{

	pthread_once(&once, init);

	AutoLock<DiskArray> lock(*clusterDisks);
	for(DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if((*k).active() && ((*k).type() == type))
		{
			disks.push_back(string("marsfs://") + (*k).node() + (*k).path());
		}
	}

}

string ClusterDisks::node(const string& path)
{

	pthread_once(&once, init);
	vector<string> nodes;

	DiskArray::const_iterator j = clusterDisks->end();

	AutoLock<DiskArray> lock(*clusterDisks);
	for(DiskArray::const_iterator k = clusterDisks->begin(); k != clusterDisks->end(); ++k)
	{
		if((*k).active() && (path.find((*k).path()) == 0))
		{
			if(j != clusterDisks->end())
			{
				StrStream os;
				os << "Two nodes found for [" << path << "] "
				<< "marsfs://" << (*j).node() << "/" << (*j).path() << "and "
				<< "marsfs://" << (*k).node() << "/" << (*k).path()
				<< StrStream::ends;

				throw SeriousBug(string(os));
			}
			j = k;
		}
	}





	if(j == clusterDisks->end())
	{
        // Look for local names
       
        // This is ineficent, but is should be called very rarely

        if(LocalPathName(path).exists())
            return NodeInfo::thisNode().node();

        
        LocalPathName df("~/etc/disks/df");
        ifstream in(df.localPath());
        char line[1024];
        while(in.getline(line,sizeof(line)))
        {
            if(line[0] != 0  || line[0] != '#')
            {
                Tokenizer tokenize(", \t");
                vector<string> tokens;
                tokenize(line, tokens);
                if(tokens.size() == 2)
                {
                    const FileSpace &fs = FileSpace::lookUp(tokens[0]);
                    const vector<PathName>& v = fs.fileSystems();
                    for(size_t j = 0; j<v.size(); ++j)
                    {
                        if(path.find(v[j].asString()) == 0)
                        {
                            Log::info() << "ClusterDisks::node [" << path << "] is on " << v[j] << endl;
                            return "local";
                        }
                    }
                }
            }
        }

    
		StrStream os;
		os << "No node found for [" << path << "]" << StrStream::ends;
		throw SeriousBug(string(os));
	}

	return (*j).node();

}

//-----------------------------------------------------------------------------

} // namespace eclib

