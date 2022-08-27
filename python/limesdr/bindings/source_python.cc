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
/* BINDTOOL_HEADER_FILE(source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(1a2857d751a5db4102ab764707167b12)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/limesdr/source.h>
// pydoc.h is automatically generated in the build directory
#include <source_pydoc.h>

void bind_source(py::module& m)
{

    using source = ::gr::limesdr::source;


    py::class_<source, gr::block, gr::basic_block, std::shared_ptr<source>>(
        m, "source", D(source))

        .def(py::init(&source::make),
             py::arg("serial"),
             py::arg("channel_mode"),
             py::arg("filename"),
             D(source, make))


        .def("set_center_freq",
             &source::set_center_freq,
             py::arg("freq"),
             py::arg("chan") = 0,
             D(source, set_center_freq))


        .def("set_antenna",
             &source::set_antenna,
             py::arg("antenna"),
             py::arg("channel") = 0,
             D(source, set_antenna))


        .def("set_nco",
             &source::set_nco,
             py::arg("nco_freq"),
             py::arg("channel"),
             D(source, set_nco))


        .def("set_bandwidth",
             &source::set_bandwidth,
             py::arg("analog_bandw"),
             py::arg("channel") = 0,
             D(source, set_bandwidth))


        .def("set_digital_filter",
             &source::set_digital_filter,
             py::arg("digital_bandw"),
             py::arg("channel"),
             D(source, set_digital_filter))


        .def("set_gain",
             &source::set_gain,
             py::arg("gain_dB"),
             py::arg("channel") = 0,
             D(source, set_gain))


        .def("set_sample_rate",
             &source::set_sample_rate,
             py::arg("rate"),
             D(source, set_sample_rate))


        .def("set_oversampling",
             &source::set_oversampling,
             py::arg("oversample"),
             D(source, set_oversampling))


        .def("calibrate",
             &source::calibrate,
             py::arg("bandw"),
             py::arg("channel") = 0,
             D(source, calibrate))


        .def("set_buffer_size",
             &source::set_buffer_size,
             py::arg("size"),
             D(source, set_buffer_size))


        .def("set_tcxo_dac",
             &source::set_tcxo_dac,
             py::arg("dacVal") = 125,
             D(source, set_tcxo_dac))

        ;
}
