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

#ifndef INCLUDED_LIMESDR_SOURCE_IMPL_H
#define INCLUDED_LIMESDR_SOURCE_IMPL_H

#include <limesdr/source.h>
#include "common/device_handler.h"

namespace gr
{
  namespace limesdr
  {
    class source_impl : public source
    {
    private:
	lms_stream_t streamId[2];
	uint32_t fifosize =  4096*10;

	struct constant_data
	{
	  int device_number;
	  int device_type;
	  int chip_mode;
	  int channel;
	} stored;

	int LMS_CH_0 = 0;
	int LMS_CH_1 = 1;
	int source_block = 1;

    public:
	source_impl(int device_number,
		    int device_type,
		    int chip_mode,
		    int channel,
		    int file_switch,
		    const char *filename,
		    double rf_freq,
		    double samp_rate,
		    size_t oversample,
		    int calibration_ch0,
		    double calibr_bandw_ch0,
		    int calibration_ch1,
		    double calibr_bandw_ch1,
		    int lna_path_mini,
		    int lna_path_ch0,
		    int lna_path_ch1,
		    int analog_filter_ch0,
		    double analog_bandw_ch0,
		    int analog_filter_ch1,
		    double analog_bandw_ch1,
		    int digital_filter_ch0,
		    double digital_bandw_ch0,
		    int digital_filter_ch1,
		    double digital_bandw_ch1,
		    int gain_dB_ch0,
		    int gain_dB_ch1);
	~source_impl();

	int work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);

	void init_stream(int device_number, int channel);

	inline gr::io_signature::sptr args_to_io_signature(int channel_number);
    };
  }
}

#endif

