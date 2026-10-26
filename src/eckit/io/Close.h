// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   Close.h
///
/// close(2) and fclose(3) wrappers that tests can make fail (see SyscallFault). Same contract as the system calls:
/// the descriptor or stream is released even when the call fails, so it must never be closed again.

#pragma once

#include <cstdio>

namespace eckit {

/// Closes @p fd. Returns 0, or -1 with errno set.
int closeFd(int fd);

/// Closes @p file. Returns 0, or EOF with errno set.
int closeFile(FILE* file);

}  // namespace eckit
