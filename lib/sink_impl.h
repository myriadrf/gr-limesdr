/* -*- c++ -*- */
/*
 * Copyright 2018 Lime Microsystems info@limemicro.com
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_LIMESDR_SINK_IMPL_H
#define INCLUDED_LIMESDR_SINK_IMPL_H

#include "common/device_handler.h"
#include <limesdr/sink.h>


static const pmt::pmt_t TIME_TAG = pmt::string_to_symbol("tx_time");

namespace gr {
namespace limesdr {
class sink_impl : public sink {
    private:
    lms_stream_t streamId[2];

    bool stream_analyzer = false;

    int sink_block = 2;

    pmt::pmt_t LENGTH_TAG;
    lms_stream_meta_t tx_meta;
    long burst_length = 0;
    int nitems_send = 0;
    int ret[2] = {0};

    std::string device_string[3] = {"LimeSDR-Mini", "LimeNET-Micro", "LimeSDR-USB"};

    struct constant_data {
        std::string serial;
        int device_number;
        int device_type;
        int channel_mode;
        int channel;
        double samp_rate = 1e6;
    } stored;

    std::chrono::high_resolution_clock::time_point t1, t2;

    void work_tags(int noutput_items);

    void print_stream_stats(int channel);

    public:
    sink_impl(std::string serial,
              int device_type,
              int channel_mode,
              int file_switch,
              const char* filename,
              const std::string& length_tag_name);
    ~sink_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    bool start(void);

    bool stop(void);

    inline gr::io_signature::sptr args_to_io_signature(int channel_number, int device_number);

    void init_stream(int device_number, int channel, float samp_rate);

    void set_rf_freq(float rf_freq);

    void set_pa_path(int pa_path, int channel);

    void set_nco(float nco_freq, int channel);

    void set_analog_filter(int analog_filter, float analog_bandw, int channel);

    void set_digital_filter(int digital_filter, float digital_bandw, int channel);

    void set_gain(int gain_dB, int channel);

    double set_sample_rate(double rate);

    void set_oversampling(int oversample);

    void calibrate(int calibrate, int channel, double bandw);
};
} // namespace limesdr
} // namespace gr

#endif
