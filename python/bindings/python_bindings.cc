/*
 * Copyright 2023 Lime Microsystem info@limemicro.com
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
    void bind_sink(py::module& m);
    void bind_source(py::module& m);
#ifdef ENABLE_RFE
    void bind_rfe(py::module& m);
#endif
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(limesdr_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
        bind_sink(m);
        bind_source(m);
#ifdef ENABLE_RFE
        bind_rfe(m);
#endif
    // ) END BINDING_FUNCTION_CALLS
}
