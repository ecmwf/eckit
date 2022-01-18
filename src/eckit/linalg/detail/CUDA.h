/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <cusparse.h>

#define CALL_CUDA(e)                                                                                              \
    {                                                                                                             \
        cudaError_t error;                                                                                        \
        if ((error = e) != cudaSuccess)                                                                           \
            printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
    }

#define CALL_CUBLAS(e)                                                                                            \
    {                                                                                                             \
        cublasStatus_t error;                                                                                     \
        if ((error = e) != CUBLAS_STATUS_SUCCESS)                                                                 \
            printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
    }

#define CALL_CUSPARSE(e)                                                                                          \
    {                                                                                                             \
        cusparseStatus_t error;                                                                                   \
        if ((error = e) != CUSPARSE_STATUS_SUCCESS)                                                               \
            printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
    }
