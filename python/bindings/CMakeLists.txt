# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT limesdr_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT limesdr_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND limesdr_python_files
    sink_python.cc
    source_python.cc
    python_bindings.cc)

if(ENABLE_RFE)
    list(APPEND limesdr_python_files rfe_python.cc)
    add_compile_definitions(ENABLE_RFE)
endif()

GR_PYBIND_MAKE_OOT(limesdr
   ../..
   gr::limesdr
   "${limesdr_python_files}")

install(TARGETS limesdr_python DESTINATION ${GR_PYTHON_DIR}/limesdr COMPONENT pythonapi)
