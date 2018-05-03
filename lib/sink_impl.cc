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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sink_impl.h"

namespace gr
{
  namespace limesdr
  {
    sink::sptr
    sink::make(int device_number,
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
	      int pa_path_mini,
	      int pa_path_ch0,
	      int pa_path_ch1,
	      int analog_filter_ch0,
	      double analog_bandw_ch0,
	      int analog_filter_ch1,
	      double analog_bandw_ch1,
	      int digital_filter_ch0,
	      double digital_bandw_ch0,
	      int digital_filter_ch1,
	      double digital_bandw_ch1,
	      int gain_dB_ch0,
	      int gain_dB_ch1)
    {
	return gnuradio::get_initial_sptr
	      (new sink_impl(device_number,
			    device_type,
			    chip_mode,
			    channel,
			    file_switch,
			    filename,
			    rf_freq,
			    samp_rate,
			    oversample,
			    calibration_ch0,
			    calibr_bandw_ch0,
			    calibration_ch1,
			    calibr_bandw_ch1,
			    pa_path_mini,
			    pa_path_ch0,
			    pa_path_ch1,
			    analog_filter_ch0,
			    analog_bandw_ch0,
			    analog_filter_ch1,
			    analog_bandw_ch1,
			    digital_filter_ch0,
			    digital_bandw_ch0,
			    digital_filter_ch1,
			    digital_bandw_ch1,
			    gain_dB_ch0,
			    gain_dB_ch1));
    }

    sink_impl::sink_impl(int device_number,
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
			int pa_path_mini,
			int pa_path_ch0,
			int pa_path_ch1,
			int analog_filter_ch0,
			double analog_bandw_ch0,
			int analog_filter_ch1,
			double analog_bandw_ch1,
			int digital_filter_ch0,
			double digital_bandw_ch0,
			int digital_filter_ch1,
			double digital_bandw_ch1,
			int gain_dB_ch0,
			int gain_dB_ch1)
	: gr::sync_block("sink",
			args_to_io_signature(chip_mode), // Based on chip_mode SISO/MIMO use appropriate input signature
			gr::io_signature::make(0, 0, 0))
    {
	std::cout << "---------------------------------------------------------------" <<  std::endl;
	std::cout << "LimeSuite Sink (TX) info" << std::endl;
	std::cout << std::endl;

	// 1. Store private variables upon implementation to protect from changing them later
	stored.device_number = device_number;
	stored.device_type = device_type;
	stored.chip_mode = chip_mode;

	if(stored.device_type == 1) // LimeSDR-Mini
	    stored.channel = LMS_CH_0;
	else if(stored.device_type == 2) // LimeSDR-USB
	{
	    if(stored.chip_mode == 1) // If SISO configure prefered channel
		stored.channel = channel;
	    else if(stored.chip_mode == 2) // If MIMO begin configuring channel 0
		stored.channel = LMS_CH_0;
	}

	// 2. Open device if not opened
	device_handler::getInstance().open_device(stored.device_number,stored.device_type);

	// 3. Check where to load settings from (file or block)
	if(file_switch == 1)
	{
	    if(stored.device_type == 1)
	    {
		device_handler::getInstance().mini_switch_workaround(stored.device_number);
		device_handler::getInstance().set_pa_path_mini(stored.device_number,pa_path_mini);
	    }
	    device_handler::getInstance().settings_from_file(stored.device_number,filename);
	    device_handler::getInstance().check_blocks(stored.device_number,sink_block,stored.device_type, stored.chip_mode, 0, 0, file_switch,filename);
	}
	else
	{
	    // 4. Check how many blocks were used and check values between blocks
	    device_handler::getInstance().check_blocks(stored.device_number,sink_block,stored.device_type, stored.chip_mode, samp_rate, oversample, file_switch, NULL);

	    // 5. Set SISO/MIMO mode
	    device_handler::getInstance().set_chip_mode(stored.device_number,stored.device_type,stored.chip_mode,stored.channel,LMS_CH_TX);

	    // 6. Set RF frequency
	    device_handler::getInstance().set_rf_freq(stored.device_number,stored.device_type,LMS_CH_TX,stored.channel,rf_freq);

	    // 7. Set sample rate 
	    if(stored.device_type == 1) // LimeSDR-Mini can only have the same rates
	      device_handler::getInstance().set_samp_rate(stored.device_number,samp_rate,oversample);
	    else if(stored.device_type == 2) // LimeSDR-USB can have separate rates for TX and RX
	      device_handler::getInstance().set_samp_rate_dir(stored.device_number,LMS_CH_TX,samp_rate,oversample);

	    // 8. Configure analog and digital filters
	    device_handler::getInstance().set_analog_filter(stored.device_number,LMS_CH_TX,stored.channel,analog_filter_ch0, analog_bandw_ch0);
	    device_handler::getInstance().set_digital_filter(stored.device_number,LMS_CH_TX,stored.channel,digital_filter_ch0, digital_bandw_ch0);


	    // 9. Set PA path switches and settings
	    if(stored.device_type == 1) // LimeSDR-Mini
	    {
		device_handler::getInstance().mini_switch_workaround(stored.device_number);
		device_handler::getInstance().set_pa_path_mini(stored.device_number,pa_path_mini);
		device_handler::getInstance().set_pa_path(stored.device_number,stored.channel,pa_path_mini);
	    }
	    else if(stored.device_type == 2) // LimeSDR-USB
	      device_handler::getInstance().set_pa_path(stored.device_number,stored.channel,pa_path_ch0);

	    // 10. Set GAIN
	    device_handler::getInstance().set_gain(stored.device_number,LMS_CH_TX,stored.channel,gain_dB_ch0);

	    // 11. Perform calibration
	    device_handler::getInstance().calibrate(stored.device_number,stored.device_type,calibration_ch0,LMS_CH_TX,stored.channel,calibr_bandw_ch0,rf_freq,pa_path_ch0);

	    // 12. Begin configuring device for channel 1 (if chip_mode is MIMO)
	    if(stored.chip_mode == 2 && stored.device_type == 2)
	    {
		device_handler::getInstance().set_pa_path(stored.device_number,LMS_CH_1,pa_path_ch1);
		device_handler::getInstance().set_analog_filter(stored.device_number,LMS_CH_TX,LMS_CH_1,analog_filter_ch1,analog_bandw_ch1);
		device_handler::getInstance().set_digital_filter(stored.device_number,LMS_CH_TX,LMS_CH_1,digital_filter_ch1, digital_bandw_ch1);
		device_handler::getInstance().set_gain(stored.device_number,LMS_CH_TX,LMS_CH_1,gain_dB_ch1);
		device_handler::getInstance().calibrate(stored.device_number,stored.device_type,calibration_ch1,LMS_CH_TX,LMS_CH_1,calibr_bandw_ch1,rf_freq,pa_path_ch1);
	    }
	}

	std::cout << "---------------------------------------------------------------" <<  std::endl;
    }

    sink_impl::~sink_impl()
    {}

		bool sink_impl::start(void)
		{
			// Initialize and start stream for channel 0 (if chip_mode is SISO)
			if(stored.chip_mode == 1) // If SISO configure prefered channel
			{
					this->init_stream(stored.device_number,stored.channel);
					if(LMS_StartStream(&streamId[stored.channel]) != LMS_SUCCESS)
				device_handler::getInstance().error(stored.device_number);
			}

			// Initialize and start stream for channels 0 & 1 (if chip_mode is MIMO)
			else if(stored.chip_mode == 2 && stored.device_type == 2)
			{
					this->init_stream(stored.device_number,LMS_CH_0);
					this->init_stream(stored.device_number,LMS_CH_1);
					if(LMS_StartStream(&streamId[LMS_CH_0]) != LMS_SUCCESS)
				device_handler::getInstance().error(stored.device_number);
					if(LMS_StartStream(&streamId[LMS_CH_1]) != LMS_SUCCESS)
				device_handler::getInstance().error(stored.device_number);
			}
			return true;
		}

    bool sink_impl::stop(void)
    {
			// Stop and destroy stream for channel 0 (if chip_mode is SISO)
			if(stored.chip_mode == 1)
			{
					LMS_StopStream(&streamId[stored.channel]);
					LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number), &streamId[stored.channel]);
			}
			// Stop and destroy stream for channels 0 & 1 (if chip_mode is MIMO)
			else if(stored.chip_mode == 2)
			{
					LMS_StopStream(&streamId[LMS_CH_0]);
					LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number), &streamId[LMS_CH_0]);
					LMS_StopStream(&streamId[LMS_CH_1]);
					LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number), &streamId[LMS_CH_1]);
			}
			device_handler::getInstance().close_device(stored.device_number);

      return true;
    }
    
    int
    sink_impl::work(int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
    {
	// Send stream for channel 0 (if chip_mode is SISO)
	if(stored.chip_mode == 1)
	{
	    int ret = LMS_SendStream(&streamId[stored.channel],
				    input_items[0],
				    noutput_items,
				    NULL, 0);
	    if (ret < 0)
	    {
		return 0; //call again
	    }
	    consume(0,ret);
	}
	// Send stream for channels 0 & 1 (if chip_mode is MIMO)
	else if(stored.chip_mode == 2)
	{
	    int ret0 = LMS_SendStream(&streamId[LMS_CH_0],
				      input_items[0],
				      noutput_items,
				      NULL, 0);
	    int ret1 = LMS_SendStream(&streamId[LMS_CH_1],
				      input_items[1],
				      noutput_items,
				      NULL, 0);
	    if(ret0 < 0 || ret1 < 0)
	    {
		return 0; //call again
	    }
	    consume(0,ret0);
	    consume(1,ret1);
	}
    }

    // Setup stream
    void sink_impl::init_stream(int device_number, int channel)
    {
	streamId[channel].channel = channel;
	streamId[channel].fifoSize = fifosize;
	streamId[channel].throughputVsLatency = 0.5;
	streamId[channel].isTx = LMS_CH_TX;
	streamId[channel].dataFmt = lms_stream_t::LMS_FMT_F32;

	if (LMS_SetupStream(device_handler::getInstance().get_device(device_number),&streamId[channel])!=LMS_SUCCESS)
	    device_handler::getInstance().error(device_number);

	std::cout << "INFO: sink_impl::init_stream(): sink channel " << channel << " (device nr. " << device_number << ") stream setup done." << std::endl;
    }

    // Return io_signature to manage module input count
    // based on SISO (one input) and MIMO (two inputs) modes
    inline gr::io_signature::sptr sink_impl::args_to_io_signature(int channel_number)
    {
	if (channel_number == 1)
	{
	    return gr::io_signature::make(1, 1, sizeof(gr_complex));
	}
	else if (channel_number == 2)
	{
	    return gr::io_signature::make(2, 2, sizeof(gr_complex));
	}
	else
	{
	    std::cout << "ERROR: sink_impl::args_to_io_signature(): channel_number must be 0 or 1." << std::endl;
	    exit(0);
	}
    }
  }
}

