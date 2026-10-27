# SPDX-FileCopyrightText: 2026- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

if( CMAKE_CXX_COMPILER_ID MATCHES IntelLLVM )
  # Turn off -ffinite-math-only which gets included by some optimisation levels which assumes values can never be NaN.
  # Then results in std::isnan(value) always return false.
  # -fno-finite-math-only: Standard override safe for 2021-2022 and 2025+
  # -fhonor-nans: Plugs the optimization bypass gap specific to 2023.1 - 2024.2
  set( INTEL_NAN_FLAGS "-fno-finite-math-only" "-fhonor-nans" )
  foreach( _flag ${INTEL_NAN_FLAGS} )
    # Do not use ecbuild_add_cxx_flags as this gets also added to link flags and icpx warns on unused link flag.
    # Following prevents the flag from being added to link steps.
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_flag}>)
  endforeach()
endif()

