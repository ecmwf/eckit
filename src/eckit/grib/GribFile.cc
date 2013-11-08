// File GribFile.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldSet.h"
#include "EmosFile.h"
#include "GribField.h"
#include "GribHandle.h"

#include "eckit/io/Buffer.h"
#include "eckit/utils/Timer.h"

#include "eckit/container/Cache.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/AutoLock.h"


namespace eckit {


static Mutex mutex;
static Cache<const GribFile*, EmosFile*> cache_;

static EmosFile& getFile(const GribFile* file) {
    AutoLock<Mutex> lock(mutex);
    EmosFile* result = 0;

    if(cache_.fetch(file, result)) {
        return *result;
    }

    result = new EmosFile(file->path());
    ASSERT(result);
    cache_.update(file, result);

    return *result;
}

GribFile::GribFile(const PathName& path):
    path_(path)
{
}

GribFile::~GribFile()
{
    AutoLock<Mutex> lock(mutex);
    cache_.expire(this);
}

GribFile* GribFile::newGribFile(const PathName& path) {
    return new GribFile(path);
}

void GribFile::print(ostream& os) const
{
    os << "GribFile[" << path_ << "]";
}

GribFieldSet* GribFile::getFieldSet() const
{
    Timer timer("GribFile::getFieldSet");
    GribFieldSet* fs = new GribFieldSet();
    getFieldSet(*fs);
    return fs;
}

void GribFile::getFieldSet(GribFieldSet& fs) const
{
    Timer timer(string("Load ") + path_);

    Buffer buffer(1024*1024*64);
    EmosFile& file = getFile(this);
    GribFile* self = const_cast<GribFile*>(this);

    size_t len = 0;

    while( (len = file.readSome(buffer)) != 0 )
    {
            ASSERT(len < buffer.size());

            const char *p = buffer + len - 4;

            if(p[0] != '7' || p[1] != '7' || p[2] != '7' || p[3] != '7')
                throw eckit::SeriousBug("No 7777 found");

            Offset where = file.position();
            GribField* g = new GribField(self, Offset((unsigned long long)where - len), Length(len));

            fs.add(g);
    }
}

void GribFile::getBuffer(Buffer& buffer, const Offset& offset, const Length& length)
{
    EmosFile& file = getFile(this);
    file.seek(offset);

    Length len = file.readSome(buffer);
    ASSERT(len == length);
    const char *p = buffer + len - 4;

    if(p[0] != '7' || p[1] != '7' || p[2] != '7' || p[3] != '7')
        throw eckit::SeriousBug("No 7777 found");
}


}
