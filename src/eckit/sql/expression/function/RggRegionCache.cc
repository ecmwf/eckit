/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <cstring>
#include <fstream>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/sql/expression/function/RggRegionCache.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

RggRegionCache::RggRegionCache() : RegionCache() {}

RggRegionCache::~RggRegionCache() {}

double RggRegionCache::get_resol(const double & nval) { return nval; }

/// Reads F90 namelist file $ODB_RTABLE_PATH/rtablel_2<xxxx>
/// to find out how many latitude bands there are (must be xxxx+1)
/// and how many longitudes boxes per latband there are */
int * RggRegionCache::read_rtablel_2_file(const int & Txxxx, int *NRGRI_len, int *Nlons)
{
    int *NRGRI = NULL;
    int nb = 0;
    int nlons = 0;
    int nexp = Txxxx + 1; /* Expect this many latitude bands */

    std::string rtable_file;
    std::stringstream sr;
    sr.width(3);
    sr.setf(std::ios_base::right, std::ios_base::adjustfield);
    sr.fill('0');

    // I use an environment variable; changing DHSHOME does not work...
    PathName fpath = Resource<PathName>("$ODB_RTABLE_PATH","~/odb/include");
    sr << Txxxx;
    rtable_file = fpath + "/rtablel_2" + sr.str();

    Log::info() << " gaussian grid table = " << rtable_file << std::endl;
    std::ifstream input(rtable_file.c_str());
    if (input) {
        NRGRI = new int [nexp];
        char line[1024];
        //char *lb;
        //char *rb;
        //char *eq;
        while(input.getline(line,sizeof(line)-1)) {
            char *lb = strchr(line,'(');
            char *rb = lb ? strchr(line,')') : NULL;
            char *eq = rb ? strchr(line,'=') : NULL;
            if (lb && rb && rb) {
                if (lb < rb && rb < eq) {
                    char *comma = strchr(line,',');
                    int id = 0;
                    int npts = 0;
                    if (comma && eq < comma) *comma = '\0';
                    ++lb;
                    *rb = '\0';
                    ++eq;
                    id = atoi(lb) - 1;
                    npts = atoi(eq);
                    if (id >= 0 && id < nexp && npts > 0) {
                        NRGRI[id] = npts;
                        nlons += npts;
                    }
                }
            }
        }
        nb = nexp;
    } else {
        Log::info()  << "read_rtablel_2_file(): Unsupported resolution Txxxx = " << Txxxx << " or $ODB_RTABLE_PATH not defined" << std::endl;
    }

    if (NRGRI_len) *NRGRI_len = nb;
    if (Nlons) *Nlons = nlons;

    return NRGRI;
}

/// compute knum zeros, or if knum>50, knum approximate zeros of the 
/// bessel function J0.
///
///  pbes - array, imensione knum, to receive the values
///  knum - number of zeros requeste.
///
/// Method:
/// -------
///      The first 50 values are obtained from a lookup table.
///      Any additional values requested are interpolated.
///
void RggRegionCache::bsslzr(double pbes[], const int & knum)
{
    int inum;

    double zapi, zpi;

    double zbes[50];

    zbes[0]=2.4048255577e0;
    zbes[1]=5.5200781103e0;
    zbes[2]=8.6537279129e0;
    zbes[3]=11.7915344391e0;
    zbes[4]=14.9309177086e0;
    zbes[5]=18.0710639679e0;
    zbes[6]=21.2116366299e0;
    zbes[7]=24.3524715308e0;
    zbes[8]=27.4934791320e0;
    zbes[9]=30.6346064684e0;
    zbes[10]=33.7758202136e0;
    zbes[11]=36.9170983537e0;
    zbes[12]=40.0584257646e0;
    zbes[13]=43.1997917132e0;
    zbes[14]=46.3411883717e0;
    zbes[15]=49.4826098974e0;
    zbes[16]=52.6240518411e0;
    zbes[17]=55.7655107550e0;
    zbes[18]=58.9069839261e0;
    zbes[19]=62.0484691902e0;
    zbes[20]=65.1899648002e0;
    zbes[21]=68.3314693299e0;
    zbes[22]=71.4729816036e0;
    zbes[23]=74.6145006437e0;
    zbes[24]=77.7560256304e0;
    zbes[25]=80.8975558711e0;
    zbes[26]=84.0390907769e0;
    zbes[27]=87.1806298436e0;
    zbes[28]=90.3221726372e0;
    zbes[29]=93.4637187819e0;
    zbes[30]=96.6052679510e0;
    zbes[31]=99.7468198587e0;
    zbes[32]=102.8883742542e0;
    zbes[33]=106.0299309165e0;
    zbes[34]=109.1714896498e0;
    zbes[35]=112.3130502805e0;
    zbes[36]=115.4546126537e0;
    zbes[37]=118.5961766309e0;
    zbes[38]=121.7377420880e0;
    zbes[39]=124.8793089132e0;
    zbes[40]=128.0208770059e0;
    zbes[41]=131.1624462752e0;
    zbes[42]=134.3040166383e0;
    zbes[43]=137.4455880203e0;
    zbes[44]=140.5871603528e0;
    zbes[45]=143.7287335737e0;
    zbes[46]=146.8703076258e0;
    zbes[47]=150.0118824570e0;
    zbes[48]=153.1534580192e0;
    zbes[49]=156.2950342685e0;

    //
    // 1. Extract values from look up table.
    //
    zapi = 2.0e0*asin(1.0e0);
    inum=std::min(knum,50);

    for (int j=0; j<inum; j++)
        pbes[j] = zbes[j];

    //
    // 2. Interpolate remaining values
    //
    if (knum > 50) {
        zpi=zapi;
        for (int j=50; j<knum; j++)
            pbes[j] = pbes[j-1]+zpi;
    }

}


/// gauaw - compute abscissas and weights for gaussian integration
int RggRegionCache::gauaw(double pa[], double pw[], const int &k)
{
    double zeps = 1e-14;
    double zpi, zc, zxz, zkm2, zkm1, zfn, zpk, zkmrk, zsp, zvsp;
    int ifk, ikk, /*js,*/ iter, jn, il;
    int iret;

    // ------------------------------------------------------------------
    // 1. set constants and find zeros of bessel function.

    iret=0;
    zpi=2.0e0*asin(1.0e0);
    zc=(1.0e0-pow((2.0e0/zpi),2))*0.25e0;
    ifk= k;
    ikk= k/2;

    bsslzr(pa, ikk);

    bool cont=true;
    for (int js=0; js<ikk; js++) {
        zxz = cos(pa[js]/sqrt(pow((ifk+0.5e0),2)+zc));
        iter=0;
        cont=true;
        do {
            // ------------------------------------------------------------------
            // 2. Compute abscissas an weights.

            // 2.1 set values for next iteration.

            zkm2=1.0e0;
            zkm1=zxz;
            ++iter;
            if (iter > 10) {
                cont=false;
                iret=10;
            } else {
                // 2.2 Computation of the legendre polynomial.
                for (jn=2; jn<= k; jn++) {
                    zfn = jn;
                    zpk = ((2.0e0*zfn-1.0e0)*zxz*zkm1-(zfn-1.0e0)*zkm2)/zfn;
                    zkm2=zkm1;
                    zkm1=zpk;
                }

                zkm1=zkm2;
                zkmrk=(ifk*(zkm1-zxz*zpk))/(1.0e0-zxz*zxz);
                zsp=zpk/zkmrk;
                zxz=zxz-zsp;
                zvsp=fabs(zsp);
                if (zvsp<=zeps) cont=false;
            }

        } while (cont);

        // 2.3 Abscissas and weights.
        if (iter <= 10) {
            pa[js] = zxz;
            pw[js] = (2.0e0*(1.0e0-zxz*zxz))/pow((ifk*zkm1),2);

            // 2.4 odd k computation of weight at the equator
            if (k != ikk*2) {
                pa[ikk]=0.0e0;
                zpk=2.0e0/(ifk*ifk);
                for (jn=2; jn<=k; jn+=2) {
                    zfn=jn;
                    zpk=zpk*zfn*zfn/pow((zfn-1.0e0),2);
                }
                pw[ikk] = zpk;
            } else {
                for (jn=0; jn<ikk; jn++) {
                    il = k - jn -1;
                    pa[il] = -pa[jn];
                    pw[il] = pw[jn];
                }
            }
        }
    }
    return iret;
}

void RggRegionCache::create_cache(const double & resol, const int & n) 
{
    // nothing done for this resolution
    int iret = 0;
    int ndgl = n + 1; // Number of latitude bands i.e. "nb"
    int nlons = 0;
    int *loncnt = read_rtablel_2_file(n, &ndgl, &nlons);
    double *latband = NULL;
    double *midlat = NULL;
    double *stlon = NULL;
    double *deltalon = NULL;
    double *zlmu = NULL;
    double *zw = NULL;
    double *zlatedge = NULL;
    const double zfact = piconst::recip_pi_over_180;
    double zfact2 = ((double)45)/atan((double) 1);
    int ii, iequator = ndgl/2;

    zlmu = new double [ndgl];
    zw = new double [ndgl];

    iret=gauaw(zlmu,zw,ndgl);

    midlat = zlmu;
    for (ii=0; ii<ndgl; ii++)
        midlat[ii] = zfact2*asin(zlmu[ii]);

    zlatedge = new double [ndgl + 1];
    zlatedge[0] = piconst::half_pi;
    for (ii=0; ii<ndgl/2; ii++)
        zlatedge[ii+1] = asin( std::max(-1.0, std::min(1.0,sin(zlatedge[ii])-zw[ii]) ) );

    delete [] zw;

    zlatedge[iequator] = 0;

    for (ii=iequator+1; ii<ndgl+1; ii++)
        zlatedge[ii] = -zlatedge[ndgl-ii];

    latband = new double [ndgl + 1];

    for (ii=0; ii<ndgl+1; ii++)
        latband[ii] = zfact*zlatedge[ii];

    delete [] zlatedge;

    // Starting longitudes, deltalon's

    stlon = new double [ndgl];
    deltalon = new double [ndgl];

    {
        int cnt;
        const double three_sixty = 360;
        for (ii=0; ii<ndgl; ii++) {
            cnt = loncnt[ii];
            double delta = three_sixty/cnt;
            deltalon[ii] = delta;
            stlon[ii] = -delta/2;
        }
    }

    // Store in cache
    //double rgg_resol = (deltalon[iequator] + deltalon[iequator+1])/2;
    RegionCacheKind kind = rgg_cache_kind;
    RegionCache::put_cache(kind, resol, ndgl,
                           latband, midlat,
                           stlon, deltalon,
                           loncnt);
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

