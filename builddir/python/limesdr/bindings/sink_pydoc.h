/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "pydoc_macros.h"
#define D(...) DOC(gr, limesdr, __VA_ARGS__)
/*
  This file contains placeholders for docstrings for the Python bindings.
  Do not edit! These were automatically extracted during the binding process
  and will be overwritten during the build process
 */


static const char* __doc_gr_limesdr_sink = R"doc(

Constructor Specific Documentation:

Return a shared_ptr to a new instance of sink.

To avoid accidental use of raw pointers, sink's constructor is private. limesdr::sink::make is the public interface for creating new instances.

Args:
    serial : Device serial number. Cannot be left blank.
    channel_mode : Channel and mode selection A(1), B(2), (A+B)MIMO(3).
    filename : Path to file if file switch is turned on.
    length_tag_name : Name of stream burst length tag)doc";


static const char* __doc_gr_limesdr_sink_sink_0 = R"doc()doc";


static const char* __doc_gr_limesdr_sink_sink_1 = R"doc()doc";


static const char* __doc_gr_limesdr_sink_make = R"doc(

Constructor Specific Documentation:

Return a shared_ptr to a new instance of sink.

To avoid accidental use of raw pointers, sink's constructor is private. limesdr::sink::make is the public interface for creating new instances.

Args:
    serial : Device serial number. Cannot be left blank.
    channel_mode : Channel and mode selection A(1), B(2), (A+B)MIMO(3).
    filename : Path to file if file switch is turned on.
    length_tag_name : Name of stream burst length tag)doc";


static const char* __doc_gr_limesdr_sink_set_center_freq = R"doc(Set center frequency)doc";


static const char* __doc_gr_limesdr_sink_set_antenna = R"doc(Set which antenna is used)doc";


static const char* __doc_gr_limesdr_sink_set_nco = R"doc(Set NCO (numerically controlled oscillator). By selecting NCO frequency configure NCO. When NCO frequency is 0, NCO is off.)doc";


static const char* __doc_gr_limesdr_sink_set_bandwidth = R"doc(Set analog filters.)doc";


static const char* __doc_gr_limesdr_sink_set_digital_filter = R"doc(Set digital filters (GFIR).)doc";


static const char* __doc_gr_limesdr_sink_set_gain = R"doc(Set the combined gain value in dB)doc";


static const char* __doc_gr_limesdr_sink_set_sample_rate = R"doc(Set the same sample rate for both channels.)doc";


static const char* __doc_gr_limesdr_sink_set_oversampling = R"doc(Set oversampling for both channels.)doc";


static const char* __doc_gr_limesdr_sink_calibrate = R"doc(Perform device calibration.)doc";


static const char* __doc_gr_limesdr_sink_set_buffer_size = R"doc(Set stream buffer size)doc";


static const char* __doc_gr_limesdr_sink_set_tcxo_dac = R"doc(Set TCXO DAC.)doc";
