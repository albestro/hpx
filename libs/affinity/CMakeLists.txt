# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(affinity_headers
  hpx/affinity/affinity_data.hpp
  hpx/affinity/detail/partlit.hpp
  hpx/affinity/parse_affinity_options.hpp
)

set(affinity_compat_headers
  hpx/runtime/threads/detail/partlit.hpp
  hpx/runtime/threads/policies/affinity_data.hpp
  hpx/runtime/threads/policies/parse_affinity_options.hpp
)

set(affinity_sources
  affinity_data.cpp
  parse_affinity_options.cpp
)

include(HPX_AddModule)
add_hpx_module(affinity
  COMPATIBILITY_HEADERS ON
  DEPRECATION_WARNINGS
  FORCE_LINKING_GEN
  GLOBAL_HEADER_GEN ON
  SOURCES ${affinity_sources}
  HEADERS ${affinity_headers}
  COMPAT_HEADERS ${affinity_compat_headers}
  DEPENDENCIES
    hpx_assertion
    hpx_config
    hpx_datastructures
    hpx_errors
    hpx_format
    hpx_topology
    hpx_util
  CMAKE_SUBDIRS tests
)
