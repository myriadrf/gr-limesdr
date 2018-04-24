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

#ifndef DEVICE_HANDLER_H
#define DEVICE_HANDLER_H

#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <LimeSuite.h>
#include <lime/LMS7002M_parameters.h>

class device_handler
{
private:

  int open_devices = 0;

  bool list_read = false;
  int device_count;
  
  struct device
  {
    // Device address
    lms_device_t* address = NULL;

    // Flags and variables used to check
    // shared settings and blocks usage
    bool source_flag = false;
    bool sink_flag = false;
    bool mini_switch_workaround_flag = false;
    int source_device_type = -1;
    int sink_device_type = -1;
    int source_chip_mode = -1;
    int sink_chip_mode = -1;
    double source_sample_rate = -1;
    double sink_sample_rate = -1;
    size_t source_oversample = -1;
    size_t sink_oversample = -1;
    int source_file_switch = -1;
    int sink_file_switch = -1;
    const char *source_filename = NULL;
    const char *sink_filename = NULL;
  };
  
  //Device list
  lms_info_str_t* list = new lms_info_str_t[5];
  // Device vector. Adds devices from the list
  std::vector<device> device_vector; 
  
  device_handler() {};
  device_handler(device_handler const&);
  void operator=(device_handler const&);

  int LMS_CH_0 = 0;
  int LMS_CH_1 = 1;

public:
  static device_handler& getInstance()
  {
    static device_handler instance;
    return instance;
  }
  ~device_handler();
  
	/**
	* Print device error and close all devices.
	* 
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	*/
	void error(int device_number);
  
	/**
	* Get device connection handler in order to configure it.
	* 
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	*/
	lms_device_t* get_device(int device_number);

	/**
	* Connect to the device and create singletone.
	* 
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   device_type LimeSDR-Mini(1), LimeSDR-USB(2).
	*/
	void open_device(int device_number, int device_type);
	
	/**
	* Disconnect from the device.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	*/
	void close_device(int device_number);

	/**
	* Disconnect from all devices.
	*/
	void close_all_devices();
	
	/**
	* Check what blocks are used for single device.
	* 
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   device_type LimeSDR-Mini(1), LimeSDR-USB(2).
	*
	* @param   chip_mode SISO(1), MIMO(2).
	* 
	* @param   samp_rate  Sample rate in Hz.
	* 
	* @param   oversample  Oversampling value (0 (default),1,2,4,8,16,32). 
	* 
	* @param   file_switch  Load settings from file: NO(0),YES(1).
	* 
	* @param   filename  Path to file if file switch is turned on.
	*/
	void check_blocks(int device_number, int block_type, int device_type, int chip_mode, double sample_rate, size_t oversample, int file_switch, const char *filename);
	
	/**
	* Load settings from .ini file. 
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   filename Path to file if file switch is turned on.
	*/
	void settings_from_file(int device_number, const char* filename);
	
	/**
	* Set chip mode (single-input single-output/multiple-input multiple-output) 
	* and check if the device supports it.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   device_type LimeSDR-Mini(1), LimeSDR-USB(2).
	*
	* @param   chip_mode SISO(1), MIMO(2).
	*/
	void set_chip_mode(int device_number, int device_type, int chip_mode, int channel, bool direction);
		
	/**
	* Set the same sample rate for both channels.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   samp_rate  Sample rate in S/s.
	* 
	* @param   oversample  Oversampling value (0 (default),1,2,4,8,16,32). 
	*/
	void set_samp_rate(int device_number, const double rate, size_t oversample);
	
	/**
	* Set sample rate for both channels (RX and TX seperately).
	* 
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	*
	* @param   direction  Direction of samples RX(LMS_CH_RX), TX(LMS_CH_RX).
	*
	* @param   samp_rate  Sample rate in S/s.
	* 
	* @param   oversample  Oversampling value (0 (default),1,2,4,8,16,32). 
	*/
	void set_samp_rate_dir(int device_number, const int direction, const double rate, size_t oversample);
	
	/**
	* Set RF frequency of both channels (RX and TX seperately).
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   device_type LimeSDR-Mini(1), LimeSDR-USB(2).
	* 
	* @param   direction  Direction of samples RX(LMS_CH_RX), TX(LMS_CH_TX).
	*
	* @param   rf_freq  RF frequency in Hz.
	*/
	void set_rf_freq(int device_number, int device_type, bool direction, int channel, float rf_freq);
	
	/**
	* Perform device calibration.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   device_type LimeSDR-Mini(1), LimeSDR-USB(2).
	* 
	* @param   calibration Turn calibration: OFF(0),ON(1).
	* 
	* @param   direction  Direction of samples: RX(LMS_CH_RX),TX(LMS_CH_RX).
	* 
	* @param   channel  Channel selection: A(LMS_CH_0),B(LMS_CH_1).
	* 
	* @param   bandwidth Set calibration bandwidth in Hz.
	* 
	* @param   rf_freq Pass RF frequency in Hz for calibration.
	* 
	* @param   path Pass path for calibration.
	* 
	* @note    rf_freq and path are necessary in order to perform calibration for LNAL (matching)
	* 	   and center frequencies below 30 MHz (NCO).
	*/
	void calibrate(int device_number, int device_type, int calibration, int direction, int channel, double bandwidth, float rf_freq, int path);
	
	/**
	* Workaround for LimeSDR-Mini switch bug. Call upon implementation.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	*/
	void mini_switch_workaround(int device_number);
	
	/**
	* LNA switch for LimeSDR-Mini.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   lna_path_mini  LNA switch: LNAH(1),LNAW(3).
	*/
	void set_lna_path_mini(int device_number, int lna_path_mini);
	
	/**
	* Set LNA path.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   channel  Channel selection: A(LMS_CH_0),B(LMS_CH_1).
	* 
	* @param   lna_path  LNA path: no path(0),LNAH(1),LNAL(2),LNAW(3).
	*/
	void set_lna_path(int device_number, int channel, int lna_path);
	
	/**
	* PA switch for LimeSDR-Mini.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   pa_path_mini  PA path: BAND1(1),BAND2(2).
	*/
	void set_pa_path_mini(int device_number, int pa_path_mini);
	
	/**
	* Set PA path.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   channel  Channel selection: A(LMS_CH_0),B(LMS_CH_1).
	* 
	* @param   pa_path  PA path: BAND1(1),BAND2(2).
	*/
	void set_pa_path(int device_number, int channel, int pa_path);
	
	/**
	* Set analog filters.
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   direction  Direction of samples: RX(LMS_CH_RX),TX(LMS_CH_TX).
	*
	* @param   channel  Channel selection: A(LMS_CH_0),B(LMS_CH_1).
	* 
	* @param   analog_filter  Turn analog filter: OFF(0),ON(1).
	*
	* @param   analog_bandw  Channel filter bandwidth in Hz.
	*/
	void set_analog_filter(int device_number, bool direction, int channel, int analog_filter, float analog_bandw);
	
	/**
	* Set digital filters (GFIR).
	*
	* @param   device_number Device number from the list of LMS_GetDeviceList.
	* 
	* @param   direction  Direction of samples: RX(LMS_CH_RX),TX(LMS_CH_TX).
	*
	* @param   channel  Channel selection: A(LMS_CH_0),B(LMS_CH_1).
	* 
	* @param   digital_filter  Turn digital filter: OFF(0),ON(1).
	*
	* @param   digital_bandw  Channel filter bandwidth in Hz.
	*/
	void set_digital_filter(int device_number, bool direction, int channel, int digital_filter, float digital_bandw);
	
	/**
	* Set the combined gain value in dB
	* This function computes and sets the optimal gain values of various amplifiers
	* that are present in the device based on desired  gain value in dB.
	*
	* @note actual gain depends on LO frequency and analog LPF configuration and
	* resulting output signal levle may be different when those values are changed
	*
	* @param   device_number  Device number from the list of LMS_GetDeviceList.
	* 
	* @param   direction      Select RX or TX.
	* 
	* @param   channel        Channel index.
	* 
	* @param   gain_dB        Desired gain: [0,70] RX, [0,60] TX.
	*/
	void set_gain(int device_number, bool direction, int channel, unsigned int gain_dB);
};


#endif
