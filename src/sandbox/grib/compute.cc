/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <random>

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/runtime/Context.h"

#include "eckit/grib/GribFieldSet.h"
#include "eckit/grib/GribField.h"
#include "eckit/grib/GribCompute.h"


using namespace eckit;
using namespace eckit::compute;

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

class Compute : public Tool {
public:

    Compute(int argc,char **argv): Tool(argc,argv) {
    }

    ~Compute() {
    }

    virtual void run();
};

//-----------------------------------------------------------------------------


double distance(const GribFieldSet& a, const GribFieldSet& b)
{
    GribFieldSet c = a-b;
    return std::sqrt(compute::accumulate(c*c));
}

void Compute::run()
{
    GribFieldSet members("/tmp/data.grib");
    Log::info() << members << endl;
    Log::info() << "MAX: " << maxvalue(members) << endl;
    Log::info() << "MIN: " << minvalue(members) << endl;

    GribFieldSet p;
    for(GribFieldSet::iterator j = members.begin(); j != members.end() ; ++j) {
        GribFieldSet m(*j);
        double mx = maxvalue(m);
        double mn = minvalue(m);
        double dd = mx - mn;
        m = (m-mn)/dd;
        p = merge(p, m);
    }

    members = p;
    Log::info() << members << endl;
    Log::info() << "MAX: " << maxvalue(members) << endl;
    Log::info() << "MIN: " << minvalue(members) << endl;


    int K = 12;


    vector<vector<size_t> > last;
    vector<GribFieldSet> centroids(K);
    std::default_random_engine generator;
    std::uniform_int_distribution<size_t> distribution(0, members.count()-1);
    std::set<size_t> seen;

    for(size_t i = 0; i < K; i++) {
        size_t choice = distribution(generator);
        while(seen.find(choice) != seen.end()) {
            choice = distribution(generator);
        }
        seen.insert(choice);
        centroids[i] = members[choice];
    }

    Timer timer("k-mean");

    for(;;)
    {
        vector<vector<size_t> > ranks(K);

        for(size_t j = 0 ; j < members.count(); ++j)
        {
            GribFieldSet m = members[j];

            double min_distance = distance(m, centroids[0]);
            int    min_index    = 0;
            for(size_t i = 1; i < K; ++i)
            {
                double x = distance(m, centroids[i]);
                if(x < min_distance) {
                    min_distance = x;
                    min_index    = i;
                }
            }

            ranks[min_index].push_back(j);

        }

        for(size_t i = 0; i < K; i++) {
            cout << ranks[i].size() << ' ';
        }
        cout << endl;

        for(size_t i = 0; i < K; i++) {
            centroids[i] = mean(members.slice(ranks[i]));
        }

        if (ranks == last)
            break;

        last = ranks;
    }

}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    Compute app(argc,argv);
    app.start();
    return 0;
}

