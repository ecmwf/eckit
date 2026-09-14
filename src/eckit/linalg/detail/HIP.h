// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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
