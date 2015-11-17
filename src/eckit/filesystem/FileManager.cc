/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/FileName.h"
#include "eckit/filesystem/FileManager.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#if 0
typedef std::map<std::string,FileManager*>    FileManagerMap;
typedef std::map<std::string,FileManagerFactory*> FactoryManagerMap;

static Mutex* local_mutex         = 0;
static FileManagerMap*    m = 0;
static FactoryManagerMap* f = 0;

// May need local_mutex here


static pthread_once_t once = PTHREAD_ONCE_INIT;
static void init(void)
{
    local_mutex = new Mutex;
    m = new FileManagerMap();
    f = new FactoryManagerMap();
}

#endif

static Once<Mutex> local_mutex;
static std::map<std::string,FileManager*> m;

//-----------------------------------------------------------------------------

FileManager::FileManager(const std::string& name):
        name_(name)
{
    
    AutoLock<Mutex> lock(local_mutex);

    ASSERT(m.find(name) == m.end());
    m[name] = this;
}

FileManager::~FileManager()
{
    AutoLock<Mutex> lock(local_mutex);
    m.erase(name_);
   
}

FileManager& FileManager::lookUp(const std::string& name)
{
 
    AutoLock<Mutex> lock(local_mutex);
    std::map<std::string, FileManager*>::const_iterator j = m.find(name);
    
    Log::info() << "Looking for FileManager [" << name << "]" << std::endl;

    if (j == m.end())
    {
	Log::error() << "No FileManager for [" << name << "]" << std::endl;
	Log::error() << "Managers are:" << std::endl;
	for(j = m.begin() ; j != m.end() ; ++j)
	  Log::error() << "   " << *((*j).second) << std::endl;
        throw SeriousBug(std::string("No FileManager called ") + name);
    }

    return *((*j).second);
}

//-----------------------------------------------------------------------------

#if 0

ManagerFactory::ManagerFactory(const std::string& name):
        name_(name)
{
    pthread_once(&once,init);
    AutoLock<Mutex> lock(local_mutex);

    ASSERT(f->find(name) == f->end());
//cout << "Creating FileManager " << name_ << std::endl;
    (*f)[name] = this;
}

ManagerFactory::~ManagerFactory()
{
    AutoLock<Mutex> lock(local_mutex);
    ASSERT(f->find(name_) != f->end());
//cout << "Destroying FileManager " << name_ << std::endl;
    f->erase(name_);
}

FileManager* ManagerFactory::build(const std::string& name)
{
    pthread_once(&once,init);
    AutoLock<Mutex> lock(local_mutex);

    PathName config("~/etc/servers");
    std::ifstream in(config.localPath());

    if (!in) throw CantOpenFile(config);

    std::string s,n;

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

    throw SeriousBug(std::string("Cannot find a tape manager for server ") + name);
}

#endif

void FileManager::print(std::ostream& s) const
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
    LocalFileManager(const std::string& name) : FileManager(name) {}
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
    MarsFSFileManager(const std::string& name) : FileManager(name) {}
};

//-----------------------------------------------------------------------------

static LocalFileManager  manager1("unix");
static LocalFileManager  manager3("file");
static MarsFSFileManager manager2("marsfs");

//-----------------------------------------------------------------------------

} // namespace eckit
