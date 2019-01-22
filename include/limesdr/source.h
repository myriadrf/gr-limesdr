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

#ifndef INCLUDED_LIMESDR_SOURCE_H
#define INCLUDED_LIMESDR_SOURCE_H

#include <gnuradio/block.h>
#include <limesdr/api.h>

namespace gr {
namespace limesdr {
class LIMESDR_API source : virtual public gr::block {
    public:
    typedef boost::shared_ptr<source> sptr;

    /*!
     * @brief Return a shared_ptr to a new instance of source.
     *
     * To avoid accidental use of raw pointers, source's
     * constructor is private.  limesdr::source::make is the public
     * interface for creating new instances.
     *
     * @param serial Device serial number. Cannot be left blank.
     *
     * @param channel_mode Channel and mode selection A(1), B(2), (A+B)MIMO(3).
     *
     * @param filename Path to file if file switch is turned on.
     *
     * @return a new limesdr source block object
     */
    static sptr make(std::string serial, int channel_mode, const std::string& filename);

    virtual double set_center_freq(double freq, size_t chan = 0) = 0;

    virtual void set_lna_path(int lna_path, int channel) = 0;

    virtual void set_nco(float nco_freq, int channel) = 0;

    virtual void set_analog_filter(int analog_filter, float analog_bandw, int channel) = 0;

    virtual void set_digital_filter(int digital_filter, float digital_bandw, int channel) = 0;

    virtual uint32_t set_gain(uint32_t gain_dB, int channel = 0) = 0;

    virtual double set_sample_rate(double rate) = 0;

    virtual void set_oversampling(int oversample) = 0;

    virtual void calibrate(int calibrate, int channel, double bandw) = 0;
};
} // namespace limesdr
} // namespace gr

#endif
