/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


// There is a name clash because hip_runtime defines DEPRECATED, and eckit as well
#ifdef DEPRECATED
#undef DEPRECATED
#endif

#include <hip/hip_runtime.h>
#include <hip/library_types.h>
#include <hipsparse/hipsparse.h>

// There is a name clash because hip_runtime defines DEPRECATED, and eckit as well
#ifdef DEPRECATED
#undef DEPRECATED
#endif

#define CALL_HIP(e)                                                                                               \
    {                                                                                                             \
        hipError_t error;                                                                                         \
        if ((error = e) != hipSuccess)                                                                            \
            printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
    }

#define CALL_HIPSPARSE(e)                                                                                         \
    {                                                                                                             \
        hipsparseStatus_t error;                                                                                  \
        if ((error = e) != HIPSPARSE_STATUS_SUCCESS)                                                              \
            printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
    }
