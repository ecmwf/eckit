/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/FileName.h"
#include "eclib/FileManager.h"
#include "eclib/AutoLock.h"
#include "eclib/Mutex.h"
#include "eclib/PathName.h"
#include "eclib/PathName.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

#if 0
typedef map<string,FileManager*>    FileManagerMap;
typedef map<string,FileManagerFactory*> FactoryManagerMap;

static Mutex* mutex         = 0;
static FileManagerMap*    m = 0;
static FactoryManagerMap* f = 0;

// May need mutex here


static pthread_once_t once = PTHREAD_ONCE_INIT;
static void init(void)
{
    mutex = new Mutex;
    m = new FileManagerMap();
    f = new FactoryManagerMap();
}

#endif

static Mutex mutex;
static map<string,FileManager*> m;

//-----------------------------------------------------------------------------

FileManager::FileManager(const string& name):
        name_(name)
{
    
    AutoLock<Mutex> lock(mutex);

    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

FileManager::~FileManager()
{
    AutoLock<Mutex> lock(mutex);
    m.erase(name_);
   
}

FileManager& FileManager::lookUp(const string& name)
{
 
    AutoLock<Mutex> lock(mutex);
    map<string, FileManager*>::const_iterator j = m.find(name);
    
    Log::info() << "Looking for FileManager [" << name << "]" << endl;

    if (j == m.end())
    {
	Log::error() << "No FileManager for [" << name << "]" << endl;
	Log::error() << "Managers are:" << endl;
	for(j = m.begin() ; j != m.end() ; ++j)
	  Log::error() << "   " << *((*j).second) << endl;
        throw SeriousBug(string("No FileManager called ") + name);
    }

    return *((*j).second);
}

//-----------------------------------------------------------------------------

#if 0

ManagerFactory::ManagerFactory(const string& name):
        name_(name)
{
    pthread_once(&once,init);
    AutoLock<Mutex> lock(mutex);

    ASSERT(f->find(name) == f->end());
//cout << "Creating FileManager " << name_ << endl;
    (*f)[name] = this;
}

ManagerFactory::~ManagerFactory()
{
    AutoLock<Mutex> lock(mutex);
    ASSERT(f->find(name_) != f->end());
//cout << "Destroying FileManager " << name_ << endl;
    f->erase(name_);
}

FileManager* ManagerFactory::build(const string& name)
{
    pthread_once(&once,init);
    AutoLock<Mutex> lock(mutex);

    PathName config("~/etc/servers");
    ifstream in(config.localPath());

    if (!in) throw CantOpenFile(config);

    string s,n;

    while (in >> s >> n)
        if ( s[0] != '#' && s != "" && n != "")
        {
            if (s == name)
            {
                if (f->find(n) == f->end())
                    throw SeriousBug(
                        "No tape manager called " + n
                        + " was found, try to lookup server " + s);

                return (*f)[n]->make(name);
            }
        }

    // Use our own name

    if (f->find(name) != f->end())
        return (*f)[name]->make(name);

    throw SeriousBug(string("Cannot find a tape manager for server ") + name);
}

#endif

void FileManager::print(ostream& s) const
{
    s << "FileManager[" << name_ << "]";
}

//-----------------------------------------------------------------------------

class LocalFileManager : public FileManager {
      
    virtual bool exists(const FileName& f) {
        return PathName(f.name()).exists();
    }
    
    virtual DataHandle*  newWriteHandle(const FileName& f) {
        return PathName(f.name()).fileHandle();
    }
    
    virtual DataHandle*  newReadHandle(const FileName& f) {
        return PathName(f.name()).fileHandle();
    }
    
    virtual DataHandle*  newReadHandle(const FileName& f,const OffsetList& ol, const LengthList& ll) {
        return PathName(f.name()).partHandle(ol,ll);
    }

public:
    LocalFileManager(const string& name) : FileManager(name) {}
};

//-----------------------------------------------------------------------------

class MarsFSFileManager : public FileManager {
  
    virtual bool exists(const FileName& f) {
        return PathName(f.scheme() + ":" + f.name()).exists();
    }
    
    virtual DataHandle*  newWriteHandle(const FileName& f) {
        return PathName(f.scheme() + ":" + f.name()).fileHandle();
    }
    
    virtual DataHandle*  newReadHandle(const FileName& f) {
        return PathName(f.scheme() + ":" + f.name()).fileHandle();
    }
    
    virtual DataHandle*  newReadHandle(const FileName& f,const OffsetList& ol, const LengthList& ll) {
        return PathName(f.scheme() + ":" + f.name()).partHandle(ol,ll);
    }

public:
    MarsFSFileManager(const string& name) : FileManager(name) {}
};

//-----------------------------------------------------------------------------

static LocalFileManager  manager1("unix");
static LocalFileManager  manager3("file");
static MarsFSFileManager manager2("marsfs");

//-----------------------------------------------------------------------------

} // namespace eclib
