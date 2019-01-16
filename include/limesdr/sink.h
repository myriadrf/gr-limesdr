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

#ifndef INCLUDED_LIMESDR_SINK_H
#define INCLUDED_LIMESDR_SINK_H

#include <gnuradio/block.h>
#include <limesdr/api.h>

namespace gr {
namespace limesdr {
class LIMESDR_API sink : virtual public gr::block {
    public:
    typedef boost::shared_ptr<sink> sptr;
    /*!
     * @brief Return a shared_ptr to a new instance of sink.
     *
     * To avoid accidental use of raw pointers, sink's
     * constructor is private.  limesdr::sink::make is the public
     * interface for creating new instances.
     *
     * @param serial Device serial number. Cannot be left blank.
     *
     * @param device_type LimeSDR-Mini(1), LimeSDR-USB(2).
     *
     * @param channel_mode Channel and mode selection A(1), B(2), (A+B)MIMO(3).
     *
     * @param file_switch Load settings from file: NO(0),YES(1).
     *
     * @param filename Path to file if file switch is turned on.
     *
     * @param samp_rate sample rate in S/s.
     *
     * @param oversample oversample value. Valid values are: 0(default),1,2,4,8,16,32.
     *
     * @param length_tag_name Name of stream burst length tag
     *
     * @return a new limesdr sink block object
     */
    static sptr make(std::string serial,
                     int device_type,
                     int channel_mode,
                     int file_switch,
                     const char* filename,
                     double samp_rate,
                     size_t oversample,
                     const std::string& length_tag_name);

    virtual void set_rf_freq(float rf_freq) = 0;

    virtual void set_pa_path(int pa_path, int channel) = 0;

    virtual void set_nco(float nco_freq, int channel) = 0;

    virtual void set_analog_filter(int analog_filter, float analog_bandw, int channel) = 0;

    virtual void set_digital_filter(int digital_filter, float digital_bandw, int channel) = 0;

    virtual void set_gain(int gain_dB, int channel) = 0;

    virtual void calibrate(int calibrate, int channel, double bandw) = 0;
};
} // namespace limesdr
} // namespace gr

#endif
