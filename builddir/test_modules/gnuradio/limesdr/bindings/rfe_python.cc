/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(rfe.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(93d97c526883a8117cacae2827edcef8)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/limesdr/rfe.h>
// pydoc.h is automatically generated in the build directory
#include <rfe_pydoc.h>

void bind_rfe(py::module& m)
{

    using rfe = ::gr::limesdr::rfe;


    py::class_<rfe, std::shared_ptr<rfe>>(m, "rfe", D(rfe))

        .def(py::init<int,
                      std::string,
                      std::string,
                      char,
                      char,
                      char,
                      char,
                      char,
                      char,
                      char>(),
             py::arg("comm_type"),
             py::arg("device"),
             py::arg("config_file"),
             py::arg("IDRX"),
             py::arg("IDTX"),
             py::arg("PortRX"),
             py::arg("PortTX"),
             py::arg("Mode"),
             py::arg("Notch"),
             py::arg("Atten"),
             D(rfe, rfe, 0))
        .def(py::init<gr::limesdr::rfe const&>(), py::arg("arg0"), D(rfe, rfe, 1))


        .def("change_mode", &rfe::change_mode, py::arg("mode"), D(rfe, change_mode))


        .def("set_fan", &rfe::set_fan, py::arg("enable"), D(rfe, set_fan))


        .def("set_attenuation",
             &rfe::set_attenuation,
             py::arg("attenuation"),
             D(rfe, set_attenuation))


        .def("set_notch", &rfe::set_notch, py::arg("enable"), D(rfe, set_notch))

        ;
}
