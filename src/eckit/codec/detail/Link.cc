// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/codec/detail/Link.h"

#include "eckit/codec/RecordItem.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::codec {

// bool Link::relative() const {
//     std::string path = RecordItem::URI{uri}.path;
//     if( path.size() == 0 ) {
//         return true;
//     }
//     if( path[0] == '/' ) {
//         return false;
//     }
//     if( path[0] == '~' ) {
//         return false;
//     }
//     return true;
// }

// Link operator/( const PathName& dir, const Link& link ) {
//     auto relative_path = PathName{ RecordItem::URI{link}.path };
//     auto absolute_uri = RecordItem::URI(link);
//     absolute_uri.path = dir / relative_path;
//     return Link{absolute_uri.str()};
// };


}  // namespace eckit::codec
