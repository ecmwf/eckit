/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/order/Scan.h"

#include <algorithm>
#include <array>
#include <numeric>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::order {


static OrderRegisterType<Scan> ORDERING("scan");


#define I "-i"
#define J "-j"
#define POS "-positively"
#define NEG "-negatively"
#define ALT "-alternative"
#define SCAN "scan"


const Order::value_type Scan::DEFAULT = SCAN I POS J NEG;


Scan::Scan(const value_type& o, size_t nx, size_t ny) :
    Scan(o, [](size_t nx, size_t ny) {
        struct RegularScan final : Implementation {
            RegularScan(size_t _ni, size_t _nj) : ni(_ni), nj(_nj) {}

            size_t size() const override { return ni * nj; }
            Reordering reorder(const value_type& from, const value_type& to) const override {
                if (from == to) {
                    return no_reorder(ni * nj);
                }

                ASSERT(not is_scan_alternative(from));
                ASSERT(not is_scan_alternative(to));

                Reordering ren(ni * nj);

                if (from == SCAN I POS J POS) {
                    for (size_t count = 0, j = nj; j > 0; --j) {
                        for (size_t i = 0; i < ni; ++i) {
                            ren[count++] = (j - 1) * ni + i;
                        }
                    }

                    return ren;
                }

                if (from == SCAN I NEG J NEG) {
                    Reordering ren(ni * nj);
                    for (size_t count = 0, j = 0; j < nj; ++j) {
                        for (size_t i = ni; i > 0; --i) {
                            ren[count++] = j * ni + (i - 1);
                        }
                    }

                    return ren;
                }

                if (from == SCAN I NEG J POS) {
                    Reordering ren(ni * nj);
                    for (size_t count = 0, j = nj; j > 0; --j) {
                        for (size_t i = ni; i > 0; --i) {
                            ren[count++] = (j - 1) * ni + (i - 1);
                        }
                    }
                }

                throw exception::OrderError("RegularScan::reorder(" + from + ", " + to + ")", Here());
            }

            const size_t ni;
            const size_t nj;
        };
        return new RegularScan(nx, ny);
    }(nx, ny)) {}


Scan::Scan(const value_type& o, const pl_type& pl) :
    Scan(o, [](const pl_type& pl) {
        struct ReducedScan final : Implementation {
            explicit ReducedScan(const pl_type& _pl) :
                pl(_pl), n(std::accumulate(pl.begin(), pl.end(), static_cast<size_t>(0))) {}

            size_t size() const override { return n; }
            Reordering reorder(const value_type& from, const value_type& to) const override {
                if (from == to) {
                    return no_reorder(n);
                }

                // FIXME implement this

                throw exception::OrderError("ReducedScan::reorder(" + from + ", " + to + ")", Here());
            }

            const pl_type pl;
            const size_t n;
        };

        return new ReducedScan(pl);
    }(pl)) {}


Scan::Scan(const value_type& o) :
    Scan(o, [](const value_type&) {
        struct NoScan final : Implementation {
            size_t size() const override { return 1; }
            Reordering reorder(const value_type& from, const value_type& to) const override {
                if (from == to) {
                    return no_reorder(size());
                }

                throw exception::OrderError("NoScan::reorder", Here());
            }
        };

        return new NoScan();
    }(o)) {}


Scan::Scan(const value_type& o, Implementation* impl) : impl_(impl), order_(o) {
    static const std::array<value_type, 16> MODES{
        SCAN I NEG J NEG, SCAN I NEG J NEG ALT, SCAN I NEG J POS, SCAN I NEG J POS ALT,
        SCAN I POS J NEG, SCAN I POS J NEG ALT, SCAN I POS J POS, SCAN I POS J POS ALT,
        SCAN J NEG I NEG, SCAN J NEG I NEG ALT, SCAN J NEG I POS, SCAN J NEG I POS ALT,
        SCAN J POS I NEG, SCAN J POS I NEG ALT, SCAN J POS I POS, SCAN J POS I POS ALT,
    };

    static struct Register {
        Register() {
            std::for_each(MODES.begin(), MODES.end(), [](const auto& mode) { register_order(static_type(), mode); });
        }
    } const REGISTER;

    if (std::count(MODES.begin(), MODES.end(), o) != 1) {
        throw exception::OrderError("Scan: not recognized: '" + o + "'", Here());
    }
}


Scan::Scan(const Spec& spec) : Scan(Scan::make_order_from_spec(spec)) {}


bool Scan::is_scan_i_positively(const value_type& o) {
    return o.find(I POS) != value_type::npos;
}


bool Scan::is_scan_j_positively(const value_type& o) {
    return o.find(J POS) != value_type::npos;
}


bool Scan::is_scan_i_j(const value_type& o) {
    return o.find(I POS J) != value_type::npos || o.find(I NEG J) != value_type::npos;
}


bool Scan::is_scan_alternative(const value_type& o) {
    return o.find(ALT) != value_type::npos;
}


const std::string& Scan::static_type() {
    static const std::string type{"scan"};
    return type;
}


size_t Scan::size() const {
    return impl_->size();
}


Reordering Scan::reorder(const value_type& to) const {
    return impl_->reorder(order(), to);
}

const Order::value_type& Scan::order_default() {
    return DEFAULT;
}


Order::value_type Scan::make_order_from_spec(const Spec& spec) {
    if (spec.has("order")) {
        return spec.get_string("order");
    }

    auto positively = [&spec](const std::string& pos, const std::string& neg, bool dfault) {
        return spec.has(pos) && spec.has(neg)
                   ? throw exception::OrderError("Scan: cannot have both '" + pos + "' and '" + neg + "'", Here())
                   : spec.get_bool(pos, !spec.get_bool(neg, !dfault));
    };

    auto i_pos = positively(SCAN I POS, SCAN I NEG, true);
    auto j_pos = positively(SCAN J POS, SCAN J NEG, false);
    auto ij    = spec.get_bool(SCAN I J, true);
    auto alt   = spec.get_bool(SCAN ALT, false);

    static const std::string i = I;
    static const std::string j = J;

    return SCAN +
           (ij ? i + (i_pos ? POS : NEG) + j + (j_pos ? POS : NEG)
               : j + (j_pos ? POS : NEG) + i + (i_pos ? POS : NEG)) +
           (alt ? ALT : "");
}


void Scan::fill_spec(spec::Custom& custom) const {
    if (order_ != order_default()) {
        custom.set("type", type());
        custom.set("order", order_);
    }
}


#undef I
#undef J
#undef POS
#undef NEG
#undef ALT
#undef SCAN


}  // namespace eckit::geo::order
