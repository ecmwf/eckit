
#include <cassert>
#include <iostream>
#include <string>  // for std::stoi

#include "HEALPix.h"


int main(int argc, char* argv[]) {
    for (int a = 1; a < argc; ++a) {
        int nside = std::stoi(argv[a]);

        HEALPix h(nside);

        std::cout << "Nside=" << h.nside() << "\n"
                  << "r->n\t| n->r" << std::endl;

        for (int i = 0; i < h.size(); ++i) {
            auto n = h.ring_to_nest(i);
            auto r = h.nest_to_ring(i);
            assert(i == h.ring_to_nest(r) && i == h.nest_to_ring(n));

            std::cout << i << " -> " << n << "\t| " << i << " -> " << r << std::endl;
        }
    }

    return 0;
}
