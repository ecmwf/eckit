// File GribFile.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribFile.h"
#include "GribFieldSet.h"
#include "EmosFile.h"
#include "GribHandle.h"
#include "GribField.h"

#include "eckit/io/Buffer.h"


namespace eckit {

GribFile::GribFile(const PathName& path):
    path_(path)
{
}

GribFile::~GribFile()
{
}


GribFile* GribFile::newGribFile(const PathName& path) {
    return new GribFile(path);
}


void GribFile::print(ostream& os) const
{
    os << "GribFile[" << path_ << "]";
}

GribFieldSet* GribFile::getFieldSet(bool load) const
{
    Buffer buffer(1024*1024*64);
    GribFieldSet* fs = new GribFieldSet();
    EmosFile file(path_);
    GribFile* self = const_cast<GribFile*>(this);

    size_t len = 0;

    while( (len = file.readSome(buffer)) != 0 )
    {
            ASSERT(len < buffer.size());
            
            const char *p = buffer + len - 4;
            
            if(p[0] != '7' || p[1] != '7' || p[2] != '7' || p[3] != '7')
                throw eckit::SeriousBug("No 7777 found");
            
            Offset where = file.position();

            GribHandle* h = 0;
            if(load) {
                h = new GribHandle(buffer, len);
            }

            GribField* g = new GribField(self, Offset((unsigned long long)where - len), Length(len), h);

            fs->add(g);

    }

    return fs;

}


}
