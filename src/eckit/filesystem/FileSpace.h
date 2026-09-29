// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   FileSpace.h
/// @date   Mar 1998
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_FileSpace_h
#define eckit_FileSpace_h

#include "eckit/filesystem/PathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileSpace {

public:  // methods

    const PathName& sameFileSystem(const PathName&) const;
    const std::vector<PathName>& fileSystems() const { return fileSystems_; }

    bool owns(const PathName&) const;

    const PathName& selectFileSystem(const std::string&) const;
    const PathName& selectFileSystem() const;

    const std::string& selectionStrategy() const;

    const std::string& name() const { return name_; }

    static bool exists(const std::string&);
    static const FileSpace& lookUp(const std::string&);

private:  // methods

    FileSpace(const std::string&);

    FileSpace(const FileSpace&)            = delete;
    FileSpace& operator=(const FileSpace&) = delete;
    FileSpace(FileSpace&&)                 = delete;
    FileSpace& operator=(FileSpace&&)      = delete;

    ~FileSpace();

    void load() const;

    const PathName& find(const PathName&, bool&) const;

private:  // members

    std::string name_;
    time_t last_;
    std::vector<PathName> fileSystems_;

    mutable std::string strategy_;  ///< default strategy to use when selecting filesystem
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
