#include "Format.h"

namespace eckit {

std::ostream_iterator<const char&> makeOrForwardOutputiterator(std::ostream& os) {
    return std::ostream_iterator<const char&>(os);
}

}  // namespace eckit
