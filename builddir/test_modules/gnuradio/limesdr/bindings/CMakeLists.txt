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
    set(GR_PYBIND_FLAGS -DENABLE_RFE)
endif()

GR_PYBIND_MAKE_OOT(limesdr
   ../../..
   gr::limesdr
   "${limesdr_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET limesdr_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:limesdr_python>
    ${CMAKE_BINARY_DIR}/test_modules/gnuradio/limesdr/
)

install(TARGETS limesdr_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/limesdr COMPONENT pythonapi)
