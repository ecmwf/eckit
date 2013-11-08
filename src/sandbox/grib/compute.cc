/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
#if 0

# Use the euclidian distance

function distance(a,b)
	c = a-b
	return sqrt(accumulate(c*c))
end distance


# Read data

members   = read("/tmp/mab/phd/all_era.grib")

# Normalize
if 0 then
	centroids = read("clusters")
else
x = nil
loop m in members
	mx = maxvalue(m)
	mn = minvalue(m)
	dd = mx - mn
	m = (m-mn) / dd
	x = x & m
end loop
members = x


	K=12
	choices = []
	for i = 1 to K do
		x = int(random() * count(members)) + 1
		while x in choices do
			x = int(random() * count(members)) + 1
		end while
		choices = choices & [x]
	end for

	print (choices)
	centroids = nil
	loop c in choices
		centroids = centroids & members[c]
	end loop



end if

# Define the number of clusters

K = count(centroids)
print(K)

rank      = [nil,nil]
last_rank = [nil]

# Iterrate until the classification is unchanged

while not(rank = last_rank) do

	for i = 1 to count(last_rank) do
		print(count(last_rank[i]))
	end for
	print()

	last_rank = rank

	rank     = []
	clusters = []
	samples  = []
	for n=1 to K do
		clusters = clusters  & [nil]
		samples  = samples  & [nil]
		rank     = rank      & [[]]
	end for
	minimum   = nil;

	# Assign each member to a cluster based on distance to centroid

	for j = 1 to count(members) do
		min_distance = distance(members[j],centroids[1])
		min_index    = 1
		for i = 2 to K do
			x = distance(members[j],centroids[i])
			if x < min_distance then
				min_distance = x
				min_index    = i
			end if
		end for
		clusters[min_index] = clusters[min_index] & members[j]
		rank[min_index]     = rank[min_index] & [j]


	end for

	# Adjust centroids

	for n=1 to K do
		centroids[n] = mean(clusters[n])
		samples[n]   = clusters[n][1]
	end for

	print(centroids)
	write("clusters",centroids)
	write("samples",samples)

end while

# Output cluster

write("clusters",centroids)

function <>(a,b)
	return not(a=b)
end <>
#endif

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

    Log::info() << "MAX: " << maxvalue(p) << endl;
    Log::info() << "MIN: " << minvalue(p) << endl;

#if 0
    x = nil
    loop m in members
        mx = maxvalue(m)
        mn = minvalue(m)
        dd = mx - mn
        m = (m-mn) / dd
        x = x & m
    end loop
    members = x
#endif

    GribFieldSet m = compute::mean(members);

    Log::info() << m << endl;
    m.write("/tmp/mean.grib");


    GribFieldSet a = compute::plus(m, m);
    Log::info() << a << endl;
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    Compute app(argc,argv);
    app.start();
    return 0;
}

