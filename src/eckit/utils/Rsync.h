/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_Rsync_H
#define eckit_utils_Rsync_H

namespace eckit {

class DataHandle;
class PathName;

class Rsync {

public:  // methods
    Rsync(bool statistics = false);

    ~Rsync();

    void syncData(const PathName& source, const PathName& target);
    void syncRecursive(const PathName& source, const PathName& target);

    bool shouldUpdate(const PathName& source, const PathName& target);

    void computeSignature(DataHandle& input, DataHandle& output);
    void computeDelta(DataHandle& signature, DataHandle& input, DataHandle& output);
    void updateData(DataHandle& input, DataHandle& delta, DataHandle& output);

private:  // members
    size_t block_len_;
    size_t strong_len_;

    bool statistics_;
};

}  // end namespace eckit

#endif
