// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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
