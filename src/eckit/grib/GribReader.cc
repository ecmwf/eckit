/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "grib_api.h"

#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/io/MoverHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"

#include "eckit/grib/GribReader.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

#define GRIB 0x47524942
#define SIZE (100*1024)

//------------------------------------------------------------------------------------------------------

GribReader::GribReader(const PathName& path):
    file_(path.localPath(),"r"),
    read_(0),
    skip_(0)
{
}


void GribReader::seek(const Offset& o)
{
    ::fseeko(file_,o,SEEK_SET);
    ASSERT(position() == o);
}

Offset GribReader::position() 
{
    return ::ftello(file_);
}

GribReader::~GribReader() 
{
    //cout << "read " << Bytes(read_) << endl;
    // cout << "skip " << Bytes(skip_) << endl;
}

long GribReader::readSome(Buffer& buffer)
{
    unsigned long magic = 0;
    unsigned char c = 0;

    while(fread(&c,1,1,file_) == 1)
    {
        read_++;
        magic <<= 8;
        magic |= c;

        if( (magic & 0xffffffff) ==  GRIB)
        {
            char* b = buffer;
            unsigned char *tmp = reinterpret_cast<unsigned char*>(b);
            size_t length = 0;
            long edition = 0;
            int i = 0 ,j;

            off_t here = ::ftello(file_) - 4;

            tmp[i++] = 'G';
            tmp[i++] = 'R';
            tmp[i++] = 'I';
            tmp[i++] = 'B';

            length = 0;
            for(j=0;j<3;j++)
            {
                ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                length <<= 8;
                length |= tmp[i];
                i++;
            }

            /* Edition number */
            ASSERT(fread(&tmp[i],1,1,file_)); read_++;

            edition = tmp[i++];

            switch(edition)
            {
            case 1:

                if(length & 0x800000)
                {
                    size_t sec1len = 0;
                    size_t sec4len = 0;
                    unsigned long flags;

                    ASSERT(1 == 0);

                    /* Large GRIBs */

                    /* Read section 1 length */
                    for(j=0;j<3;j++)
                    {
                        ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                        sec1len <<= 8;
                        sec1len |= tmp[i];
                        i++;
                    }

                    /* table version */
                    ASSERT(fread(&tmp[i++],1,1,file_)); read_++;
                    /* center */
                    ASSERT(fread(&tmp[i++],1,1,file_)); read_++;
                    /* process */
                    ASSERT(fread(&tmp[i++],1,1,file_)); read_++;
                    /* grid */
                    ASSERT(fread(&tmp[i++],1,1,file_)); read_++;
                    /* flags */
                    ASSERT(fread(&tmp[i],1,1,file_)); read_++;
                    flags = tmp[i++];

                    ASSERT(fread(&tmp[i],sec1len-3-5,1,file_)); read_ += sec1len-3-5;

                    i += sec1len-3-5;

                    if(flags & (1<<7)) {
                        /* Section 2 */
                        size_t sec2len = 0;
                        for(j=0;j<3;j++)
                        {
                            ASSERT(fread(&tmp[i],1,1,file_)); read_++;
                            sec2len <<= 8;
                            sec2len |= tmp[i];
                            i++;
                        }
                        ASSERT(fread(&tmp[i],sec2len-3,1,file_)); read_ += sec2len-3;
                        i += sec2len-3;
                    }

                    if(flags & (1<<6)) {

                        /* Section 3 */
                        size_t sec3len = 0;
                        for(j=0;j<3;j++)
                        {
                            ASSERT(fread(&tmp[i],1,1,file_)); read_++;
                            sec3len <<= 8;
                            sec3len |= tmp[i];
                            i++;
                        }

                        /* Read section 3 */
                        ASSERT(fread(&tmp[i],sec3len-3,1,file_)); read_ += sec3len-3;
                        i += sec3len-3;
                    }

                    for(j=0;j<3;j++)
                    {
                        ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                        sec4len <<= 8;
                        sec4len |= tmp[i];
                        i++;
                    }

                    if(sec4len < 120)
                    {
                        /* Special coding */
                        length &= 0x7fffff;
                        length *= 120;
                        length -= sec4len;
                        length += 4;
                    }
                    else
                    {
                        /* length is already set to the right value */
                    }

                }
                break;

            case 2:
                length = 0;

                if(sizeof(long) >= 8) {
                    for(j=0;j<8;j++)
                    {
                        ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                        length <<= 8;
                        length |= tmp[i];
                        i++;
                    }
                }
                else
                {
                    /* Check if the length fits in a long */
                    for(j=0;j<4;j++)
                    {
                        ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                        length <<= 8;
                        length |= tmp[i];
                        i++;
                    }

                    ASSERT(!length); /* Message too large */

                    for(j=0;j<4;j++)
                    {
                        ASSERT(fread(&tmp[i],1,1,file_)); read_++;

                        length <<= 8;
                        length |= tmp[i];
                        i++;
                    }
                }
                break;

            default:
                /* fprintf(stderr,"GRIB edition is %d len=%d\n",(int)edition,length); */
                ASSERT(1 == 0);
                break;
            }



            if(i < SIZE) {
                ASSERT(fread(&tmp[i],SIZE-i,1,file_)); read_ += SIZE-i;
                i += SIZE - i;;
            }

            ASSERT(i <= buffer.size());

#if 0
            ::fseeko(file_,here + length - 4,SEEK_SET);


            ASSERT(fread(b + length - 4, 4 , 1, file_));

            ASSERT(strncmp(b+length - 4,"7777",4) == 0);
#else
            skip_ += length - i;
            ::fseeko(file_,here + length,SEEK_SET);
            strncpy(b+length - 4,"7777",4);
#endif


            return length;

        }

    }

    return 0;
}

//------------------------------------------------------------------------------------------------------

}
}

