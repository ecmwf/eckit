// File GribFile.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFile_H
#define GribFile_H

#include "eckit/memory/Counted.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

class GribFieldSet;
class EmosFile;

class GribFile : public Counted {

public: // methods

    GribFieldSet* getFieldSet() const;
    void getFieldSet(GribFieldSet&) const;
    void getBuffer(Buffer&, const Offset&, const Length&);

    const PathName& path() const { return path_; }

    static GribFile* newGribFile(const PathName&);

protected: // members

    /// Use newGribFile() intead
    GribFile(const PathName&);
    ~GribFile();

protected: // methods

    void print(std::ostream&) const; // Change to virtual if base class

private: // members

    PathName path_;
    EmosFile* file_;

    friend std::ostream& operator<<(std::ostream& s,const GribFile& p) { p.print(s); return s; }

};

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

#endif
