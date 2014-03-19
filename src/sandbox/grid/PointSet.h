#ifndef eckit_PointSet_h
#define eckit_PointSet_h

#include <cassert>
#include <fstream>
#include <vector>
#include <map>
#include <limits>
#include <memory>

#include "atlas/Mesh.hpp"

#include "PointIndex3.h"
#include "FloatCompare.h"

//-----------------------------------------------------------------------------

namespace eckit {

class PointSet {

public: // types

    typedef PointIndex3::iterator iterator;
    typedef std::map< size_t, size_t > DupStore_t;

public: // methods

    PointSet( const std::vector< KPoint3 >& ipts ) : npts_(ipts.size())
    {
        build(ipts);
    }

    ~PointSet(){ delete tree_; }

    DupStore_t& duplicates() { return duplicates_; }

    template < typename POINT_T >
    void list_unique_points( std::vector< POINT_T >& opts, std::vector< size_t >& idxs )
    {
        ASSERT( opts.empty() );
        ASSERT( idxs.empty() );

        opts.reserve(npts_);
        idxs.reserve(npts_);

        // std::cout << "computing duplicates ... " << std::endl;

        for( PointIndex3::iterator i = tree_->begin(); i != tree_->end(); ++i )
        {
            KPoint3 p (  i->point() );
            size_t  ip = i->payload();
//            std::cout << "point " << ip << " " << p << std::endl;
            size_t uidx = unique(p,ip);
            if( ip == uidx )
            {
                opts.push_back( POINT_T( p.data() ) );
                idxs.push_back( ip );
//                std::cout << "----> UNIQ " << ip << std::endl;
            }
            else
            {
//                std::cout << "----> DUP " << ip << " -> " << uidx << std::endl;
            }
        }
    }

    size_t unique( const eckit::KPoint3& p, size_t idx = std::numeric_limits<size_t>::max() )
    {
        DupStore_t::iterator dit = duplicates_.find(idx);
        if( dit != duplicates_.end() )
        {
//                std::cout << "      !! DUPLICATE !!" << std::endl;
                return dit->second;
        }
        else
            return this->search_unique(p,idx,0);
    }

    iterator begin() { return tree_->begin(); }
    iterator end() { return tree_->end(); }

    size_t size() const { return npts_; }

protected: // methods

    template < typename V >
    void build( const V& ipts )
    {
        std::vector< PointIndex3::Value > pidx;
        pidx.reserve(npts_);

        for( size_t ip = 0; ip < npts_; ++ip )
            pidx.push_back( PointIndex3::Value( PointIndex3::Point( ipts[ip] ), ip ) );

        PathName path (std::string("~/tmp/cache/grid/") + "test" + ".kdtree");
        PathName tmp  (std::string("~/tmp/cache/grid/") + "test" + ".tmp");
        tmp.unlink();

        tree_ = new PointIndex3(tmp, npts_, 0);

        tree_->build(pidx.begin(), pidx.end());

        PathName::rename(tmp, path);
    }

    size_t search_unique( const eckit::KPoint3& p, size_t idx, u_int32_t n  )
    {
        PointIndex3::NodeList nearest = tree_->kNearestNeighbours( p, Kn(n) );

        std::vector<size_t> equals;
        equals.reserve( nearest.size() );

        for( size_t i = 0; i < nearest.size(); ++i )
        {
            KPoint3 np  = nearest[i].value().point();
            size_t nidx = nearest[i].value().payload();

//            std::cout << "      - " << nidx << " " << np << std::endl;

            if( points_equal(p,np) )
            {
//                std::cout << "      EQUAL !!" << std::endl;
                equals.push_back(nidx);
          }
            else
                break;
        }

        if( equals.size() == nearest.size() ) /* need to increase the search to find all duplicates of this point */
        {
            return this->search_unique(p,idx,++n);
        }
        else /* stop recursion */
        {
            size_t ret = idx; /* if nothing found return same idx */

            if( equals.size() >= 1 ) /* if an equal point was found return the first one */
                ret = equals[0];

            for( size_t n = 1; n < equals.size(); ++n )
                duplicates_[ equals[n] ] = ret;

            return ret;
        }
    }

protected:

    size_t Kn( u_int32_t n )
    {
        if( !n )
            return 2;
        else
            return 2 + std::pow(2,n) * 180;
    }

    bool duplicate( size_t idx ) { return duplicates_.find(idx) != duplicates_.end(); }

private:

    size_t npts_;
    PointIndex3* tree_;
    DupStore_t duplicates_; ///< map from duplicate idx to idx representing group of points

};

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

