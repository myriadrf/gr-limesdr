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

#include <lime/LMS7002M_parameters.h>
#include "device_handler.h"

void device_handler::error(int device_number)
{
    std::cout << "ERROR: " << LMS_GetLastErrorMessage() << std::endl;
    if (this->device_vector[device_number].address != NULL)
    close_all_devices();
}

device_handler::~device_handler()
{}

lms_device_t* device_handler::get_device(int device_number)
{
    return this->device_vector[device_number].address;
}

int device_handler::open_device(std::string serial, int device_type)
{
    std::string search_name;
    std::string start_string;
    int last;
    int device_number;
    
    std::cout << "##################" << std::endl;
    std::cout << "Connecting to device" << std::endl;
        
    // Read device list once
    if(list_read == false)
    {
	device_count = LMS_GetDeviceList(list);
        std::cout << "##################" << std::endl;
        std::cout << "Devices found: " << device_count << std::endl;
        std::cout << "##################" << std::endl;
        if (device_count < 1)
            exit(0);
        std::cout << "Device list:" << std::endl;

        for (int i = 0; i < device_count; i++)
        {
            std::cout << "Nr.:|" <<  i << "|device:|" << list[i] << std::endl;
            device_vector.push_back(device());
        }
        std::cout << "##################" << std::endl;
        list_read = true;
    }

    // Serial must not be empty
    if(serial.empty())
    {
      std::cout << "ERROR: device_handler::open_device(): no serial number." << std::endl;
      std::cout << "INFO: device_handler::open_device(): use \"LimeUtil -- find\" in terminal to find device serial." << std::endl;
      std::cout << "##################" << std::endl;
      close_all_devices();
    }
    
    // Set device string search parameters
    if (device_type == 1)
    {
	search_name = "LimeSDR Mini";
	start_string = "serial=";
	last = 14; // length of LimeSDR-Mini serial
    }
    else if (device_type == 2)
    {
      search_name = "LimeSDR-USB";
      start_string = "serial=";
      last = 16; // length of LimeSDR-USB serial
    }
    else
    {
	std::cout << "ERROR: device_handler::open_device(): wrong device_type." << std::endl;
	close_all_devices();
    }
    
    // Identify device by device type and serial number
    for (int i = 0; i < device_count; i++)
    {
      std::string device_string(list[i]);
      unsigned first = device_string.find(start_string);
      std::string aquired_serial = device_string.substr (first+7,last);

      // If device_type and serial match found acquire device number
      if (device_string.find(search_name) != std::string::npos && aquired_serial == serial)
      { 
	    device_number = i;
	    break;
      }
      // If program was unable to find serial in list print error and stop program
      else if ((i == device_count-1 && aquired_serial != serial) || (device_string.find(search_name) == std::string::npos && aquired_serial == serial))
      {
	  std::cout << "Unable to find " << search_name << " with device serial " << serial << "." << std::endl;
	  std::cout << "##################" << std::endl;
	  close_all_devices();
      }
    }
    
    // If device slot is empty, open and initialize device
    if(device_vector[device_number].address == NULL)
    {
      std::cout << "Device number " << device_number << " from the list is used." << std::endl;
      LMS_Open(&device_vector[device_number].address, list[device_number], NULL);
      LMS_Init(device_vector[device_number].address);
      ++open_devices; // Count open devices
      std::cout << "##################" << std::endl;
      std::cout << std::endl; 
    }
    // If device is open do nothing
    else
    {
      std::cout << "Previously connected device number " << device_number << " from the list is used." << std::endl;  
      std::cout << "##################" << std::endl;
      std::cout << std::endl; 
    }
    
    return device_number; // return device number to identify device_vector[device_number].address connection in other functions
}

void device_handler::close_device(int device_number, int block_type)
{
  // Check if other block finished and close device
  if(device_vector[device_number].source_flag == false || device_vector[device_number].sink_flag == false)
  {
    if(this->device_vector[device_number].address != NULL)
    {
        std::cout << std::endl;
        std::cout << "##################" << std::endl;
        if (LMS_Reset(this->device_vector[device_number].address)!= LMS_SUCCESS)
            error(device_number);
        if (LMS_Close(this->device_vector[device_number].address)!= LMS_SUCCESS)
            error(device_number);
        std::cout << "INFO: device_handler::close_device(): Disconnected from device number " << device_number << "." << std::endl;
        device_vector[device_number].address = NULL;
        std::cout << "##################" << std::endl;
        std::cout << std::endl;
    } 
  }
  // If two blocks used switch one block flag and let other block finish work
  else
  {
    // Switch flag when closing device
    switch(block_type)
    {
      case 1:
	device_vector[device_number].source_flag = false;
	break;
      case 2:
	device_vector[device_number].sink_flag = false;
	break;
    }     
  }
}

void device_handler::close_all_devices()
{
  if(close_flag == false)
  {
    for(int i = 0; i <= open_devices; i++)
    {
	if(this->device_vector[i].address != NULL)
	{
	  LMS_Reset(this->device_vector[i].address);
	  LMS_Close(this->device_vector[i].address);
	}
    }
    close_flag == true;
    exit(0);
  }
}

void device_handler::check_blocks(int device_number, int block_type, int device_type, int chip_mode, double sample_rate, size_t oversample, int file_switch, const char *filename)
{
    // Get each block settings
    switch(block_type)
    {
      case 1: // Source block
	  if(device_vector[device_number].source_flag == true)
	  {
	      std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Source (RX) block is allowed per device." << std::endl;
	      close_all_devices();
	  }
	  else
	  {
	      device_vector[device_number].source_flag = true;
	      device_vector[device_number].source_device_type = device_type;
	      device_vector[device_number].source_chip_mode = chip_mode;
	      device_vector[device_number].source_file_switch = file_switch;
	      if(file_switch == 0)
	      {
		  device_vector[device_number].source_sample_rate = sample_rate;
		  device_vector[device_number].source_oversample = oversample;
	      }
	      else
	      {
		  device_vector[device_number].source_filename = filename;
	      }
	  }
	  break;

      case 2: // Sink block
	  if(device_vector[device_number].sink_flag == true)
	  {
	      std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Sink (TX) block is allowed per device." << std::endl;
	      close_all_devices();
	  }
	  else
	  {
	      device_vector[device_number].sink_flag = true;
	      device_vector[device_number].sink_device_type = device_type;
	      device_vector[device_number].sink_chip_mode = chip_mode;
	      device_vector[device_number].sink_file_switch = file_switch;
	      if(file_switch == 0)
	      {
		  device_vector[device_number].sink_sample_rate = sample_rate;
		  device_vector[device_number].sink_oversample = oversample;
	      }
	      else
	      {
		  device_vector[device_number].sink_filename = filename;
	      }
	  }
	  break;

      default:
	  std::cout << "ERROR: device_handler::check_blocks(): incorrect block_type value." << std::endl;
	  close_all_devices();
    }

    // Check block settings which must match
    if(device_vector[device_number].source_flag && device_vector[device_number].sink_flag)
    {
	// Device_type must match in blocks with the same serial
        if(device_vector[device_number].source_device_type != device_vector[device_number].sink_device_type)
        {
            std::cout << "ERROR: device_handler::check_blocks(): device_type mismatch in LimeSuite Source (RX) and LimeSuite Sink (TX) blocks." << std::endl;
            close_all_devices();
        }

        // Chip_mode must match in blocks with the same serial
	if(device_vector[device_number].source_chip_mode != device_vector[device_number].sink_chip_mode)
        {
            std::cout << "ERROR: device_handler::check_blocks(): chip_mode mismatch in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
        }

        // File_switch must match in blocks with the same serial
        if(device_vector[device_number].source_file_switch != device_vector[device_number].sink_file_switch)
        {
            std::cout << "ERROR: device_handler::check_blocks(): file_switch must match in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
        }

        // When file_switch is 0 check LimeSDR-Mini sample_rate and oversample match throughout the blocks with the same serial
        if( (device_vector[device_number].source_file_switch == 0) && (device_vector[device_number].sink_file_switch == 0))
        {
            if((device_vector[device_number].source_sample_rate != device_vector[device_number].sink_sample_rate) && (device_type == 1))
            {
                std::cout << "ERROR: device_handler::check_blocks(): sample_rate must match in LimeSuite Source (RX) and LimeSuite Sink (TX) when using LimeSDR-Mini." << std::endl;
                close_all_devices();
            }
            else if((device_vector[device_number].source_oversample != device_vector[device_number].sink_oversample) && (device_type == 1))
            {
                std::cout << "ERROR: device_handler::check_blocks(): oversample must match in LimeSuite Source (RX) and LimeSuite Sink (TX) when using LimeSDR-Mini." << std::endl;
                close_all_devices();
            }
        }

        // When file_switch is 1 check filename match throughout the blocks with the same serial
        if(device_vector[device_number].source_filename != device_vector[device_number].sink_filename && (device_vector[device_number].source_file_switch == 1) && (device_vector[device_number].sink_file_switch == 1))
        {
            std::cout << "ERROR: device_handler::check_blocks(): file must match in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
        }
    }
}

void device_handler::settings_from_file(int device_number, const char *filename)
{
    if(LMS_LoadConfig(device_handler::getInstance().get_device(device_number),filename))
        device_handler::getInstance().error(device_number);
    
    // Set LimeSDR-Mini switches based on .ini file
    int antenna_ch0_tx;
    int antenna_ch0_rx;
    antenna_ch0_tx = LMS_GetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_TX,LMS_CH_0);
    antenna_ch0_rx = LMS_GetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_RX,LMS_CH_0);

    LMS_SetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_TX,LMS_CH_0,antenna_ch0_tx);
    LMS_SetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_RX,LMS_CH_0,antenna_ch0_rx);
}

void device_handler::set_chip_mode(int device_number, int device_type, int chip_mode, int channel, bool direction)
{
  if(device_type == 1 && chip_mode != 1)
  {
    std::cout << "ERROR: device_handler::set_chip_mode(): LimeSDR-Mini supports only SISO mode." << std::endl;
    close_all_devices();
    exit(0);
  }
  else
  {
    if(chip_mode == 1)
    {
      if(LMS_EnableChannel(device_handler::getInstance().get_device(device_number),direction,channel,true)!=LMS_SUCCESS)
	  device_handler::getInstance().error(device_number);
      std::cout << "INFO: device_handler::set_chip_mode(): SISO mode set for device number " << device_number << "." << std::endl;
    }
    else if(chip_mode == 2)
    {
      if(LMS_EnableChannel(device_handler::getInstance().get_device(device_number),direction,LMS_CH_0,true)!=LMS_SUCCESS)
	  device_handler::getInstance().error(device_number);
      if(LMS_EnableChannel(device_handler::getInstance().get_device(device_number),direction,LMS_CH_1,true)!=LMS_SUCCESS)
	  device_handler::getInstance().error(device_number);
      std::cout << "INFO: device_handler::set_chip_mode(): MIMO mode set for device number " << device_number << "." << std::endl;
    }
  }
}

void device_handler::set_samp_rate(int device_number, const double rate, size_t oversample)
{
    if(oversample==0 || oversample==1 || oversample==2 || oversample==4 || oversample==8 || oversample==16 || oversample==32)
    {
        if(rate > 30.72e6)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): LimeSDR-Mini samp_rate cannot be more than 30.72e6 S/s." << std::endl;
            close_all_devices();
            exit(0);
        }
        else if(15.36e6 < rate && rate <= 30.72e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): when LimeSDR-Mini samp_rate is more than 15.36e6 S/s and less or equal 30.72e6 S/s, oversample cannot be higher than 4." << std::endl;
            close_all_devices();
        }
        else if(7.68e6 < rate && rate <= 15.36e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): when LimeSDR-Mini samp_rate is more than 7.68e6 S/s and less or equal 15.36e6 S/s, oversample cannot be higher than 8." << std::endl;
            close_all_devices();
        }
        else if(3.84e6 < rate && rate <= 7.68e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): when LimeSDR-Mini samp_rate is more than 3.84e6 S/s and less or equal 7.68e6 S/s, oversample cannot be higher than 16." << std::endl;
            close_all_devices();
        }
        else if(oversample == 0)
	{
	  if(15.36e6 < rate && rate <= 30.72e6)
	  {
	      if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,4)!=LMS_SUCCESS)
		device_handler::getInstance().error(device_number); 
	  }
	  else if(7.68e6 < rate && rate <= 15.36e6)
	  {
	      if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,8)!=LMS_SUCCESS)
		device_handler::getInstance().error(device_number);
	  }
	  else if(3.84e6 < rate && rate <= 7.68e6)
	  {
	      if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,16)!=LMS_SUCCESS)
		device_handler::getInstance().error(device_number);
	  }
	  else if(rate <= 3.84e6)
	  {
	      if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,32)!=LMS_SUCCESS)
		device_handler::getInstance().error(device_number);
	  }
	}
	else 
	{
	  if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,oversample)!=LMS_SUCCESS)
	    device_handler::getInstance().error(device_number);
	}
	double host_value;
	double rf_value;
	if(LMS_GetSampleRate(device_handler::getInstance().get_device(device_number),LMS_CH_RX,LMS_CH_0,&host_value,&rf_value))
	  device_handler::getInstance().error(device_number);
	std::cout << "INFO: device_handler::set_samp_rate(): set sampling rate: " << host_value/1e6 << " MS/s." << std::endl;
    }
    else
    {
        std::cout << "ERROR: device_handler::set_samp_rate(): valid oversample values are: 0,1,2,4,8,16,32." << std::endl;
        close_all_devices();
    }
}

void device_handler::set_samp_rate_dir(int device_number, const int direction, const double rate, size_t oversample)
{
    if(oversample==0 || oversample==1 || oversample==2 || oversample==4 || oversample==8 || oversample==16 || oversample==32)
    {
        if(direction == LMS_CH_RX)
        {
            if(rate > 61.44e6)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): RX samp_rate cannot be more than 61.44e6 S/s." << std::endl;
                close_all_devices();
            }
            else if(30.72e6 < rate && rate <= 61.44e6 && oversample != 0 && oversample != 1 && oversample != 2)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 30.72e6 S/s and less or equal 61.44e6 S/s, oversample cannot be higher than 2." << std::endl;
                close_all_devices();
            }
            else if(15.36e6 < rate && rate <= 30.72e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 15.36e6 S/s and less or equal 30.72e6 S/s, oversample cannot be higher than 4." << std::endl;
                close_all_devices();
            }
            else if(7.68e6 < rate && rate <= 15.36e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 7.68e6 S/s and less or equal 15.36e6 S/s, oversample cannot be higher than 8." << std::endl;
                close_all_devices();
            }
            else if(3.84e6 < rate && rate <= 7.68e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 3.84e6 S/s and less or equal 7.68e6 S/s, oversample cannot be higher than 16." << std::endl;
                close_all_devices();
            }
            else if(oversample == 0)
	    {
	      if(30.72e6 < rate && rate <= 61.44e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,2)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	      else if(15.36e6 < rate && rate <= 30.72e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,4)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number); 
	      }
	      else if(7.68e6 < rate && rate <= 15.36e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,8)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	      else if(3.84e6 < rate && rate <= 7.68e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,16)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	      else if(rate <= 3.84e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,32)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	    }
            else 
	    {
	      if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,oversample)!=LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
	    }
	    double host_value;
	    double rf_value;
	    if(LMS_GetSampleRate(device_handler::getInstance().get_device(device_number),LMS_CH_RX,LMS_CH_0,&host_value,&rf_value))
	      device_handler::getInstance().error(device_number);
	    std::cout << "INFO: device_handler::set_samp_rate(): set [RX] sampling rate: " << host_value/1e6 << " MS/s." << std::endl;
        }
        else if(direction == LMS_CH_TX)
        {
            if(rate > 61.44e6)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): TX samp_rate cannot be more than 61.44e6 S/s." << std::endl;
                close_all_devices();
            }
            else if(30.72e6 < rate && rate <= 61.44e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when TX samp_rate is more than 30.72e6 S/s and less or equal 61.44e6 S/s, oversample cannot be higher than 8." << std::endl;
                close_all_devices();
            }
            else if(15.36e6 < rate && rate <= 30.72e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when TX samp_rate is more than 15.36e6 S/s and less or equal 30.72e6 S/s, oversample cannot be higher than 16." << std::endl;
                close_all_devices();
            }
            else if(oversample == 0)
	    {
	      if(30.72e6 < rate && rate <= 61.44e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,8)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	      else if(15.36e6 < rate && rate <= 30.72e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,16)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number); 
	      }
	      else if(rate <= 15.36e6)
	      {
		 if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,32)!=LMS_SUCCESS)
		    device_handler::getInstance().error(device_number);
	      }
	    }
            else 
	    {
	      if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,oversample)!=LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
	    }
	    double host_value;
	    double rf_value;
	    if(LMS_GetSampleRate(device_handler::getInstance().get_device(device_number),LMS_CH_TX,LMS_CH_0,&host_value,&rf_value))
	      device_handler::getInstance().error(device_number);
	    std::cout << "INFO: device_handler::set_samp_rate(): set [TX] sampling rate: " << host_value/1e6 << " MS/s." << std::endl;
        }
    }
    else
    {
        std::cout << "ERROR: device_handler::set_samp_rate_dir() valid oversample values are: 0,1,2,4,8,16,32." << std::endl;
        close_all_devices();
    }
}

void device_handler::set_rf_freq(int device_number, int device_type, bool direction, int channel, float rf_freq)
{
    if(rf_freq <= 0)
    {
        std::cout << "ERROR: device_handler::set_rf_freq(): rf_freq must be more than 0 Hz." << std::endl;
        close_all_devices();
    }
    else
    {
        int status = LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), direction, channel, rf_freq);
	double value = 0;
        LMS_GetLOFrequency(device_handler::getInstance().get_device(device_number), direction, channel, &value);
	std::string direction_string;
	if(direction == LMS_CH_RX)
	  direction_string = "RX";
	else if(direction == LMS_CH_TX)
	  direction_string = "TX";
	
	std::cout << "INFO: device_handler::set_rf_freq(): RF frequency set [" << direction_string << "]: " <<  value/1e6 << " MHz." << std::endl;
    }
}

void device_handler::calibrate(int device_number, int device_type, int calibration, int direction, int channel, double bandwidth, float rf_freq, int path)
{
    if(calibration == 1 && direction == LMS_CH_RX)
    {
      std::cout << "INFO: device_handler::calibrate(): ";
        if(device_type == 2 && path == 2) // LNAL matching workaround [LimeSDR-USB]
        {
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 200e6);
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0);
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq);
        }
        else if(rf_freq >= 31e6) // Normal calibration
        {
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0);
        }
        else if(device_type == 1)// Lower than 31 MHz center frequency workaround [LimeSDR-Mini]
        {
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 100e6);
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0);
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq);
        }
        else if(device_type == 2)// Lower than 31 MHz center frequency workaround [LimeSDR-USB]
        {
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 50e6);
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0);
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq);
        }
    }
    else if(calibration == 1 && direction == LMS_CH_TX)
    {
      std::cout << "INFO: device_handler::calibrate(): ";
        if(rf_freq >= 31e6)
        {
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, bandwidth, 0);
        }
        else
        {
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, 50e6);
            LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, bandwidth, 0);
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, rf_freq);
        }
    }
    else if(calibration == 0)
    {
      	std::string direction_string;
	if(direction == LMS_CH_RX)
	  direction_string = "RX";
	else if(direction == LMS_CH_TX)
	  direction_string = "TX";
	std::cout << "INFO: device_handler::calibrate(): calibration channel " << channel << " [" << direction_string << "]: disabled." << std::endl;
    }
    else
    {
      std::cout << "ERROR: device_handler::calibrate(): calibration must be either enabled (1) or disabled (0). Disabling calibration due to incorrect parameters." << std::endl;
    }
}

void device_handler::set_antenna(int device_number, int channel, int direction, int antenna)
{
    LMS_SetAntenna(device_handler::getInstance().get_device(device_number),direction,channel,antenna);
    int antenna_value = LMS_GetAntenna(device_handler::getInstance().get_device(device_number),direction,channel);
    lms_range_t range;
    LMS_GetAntennaBW(device_handler::getInstance().get_device(device_number),direction,channel,antenna,&range);
    std::string antenna_string;
    std::string direction_string;
    if(direction == LMS_CH_RX)
    {
      direction_string = "RX";
      switch(antenna_value)
      {
	case 1:
	  antenna_string = "LNAH";
	  break;
	case 2:
	  antenna_string = "LNAL";
	  break;
	case 3:
	  antenna_string = "LNAW";
	  break;
	default:
	  antenna_string = "NONE";
	  break;
      }
    }
    else if(direction == LMS_CH_TX)
    {
      direction_string = "TX";
      switch(antenna_value)
      {
	case 1:
	  antenna_string = "BAND1";
	  break;
	case 2:
	  antenna_string = "BAND2";
	  break;
	default:
	  antenna_string = "NONE";
	  break;
      }
    }
   std::cout << "INFO: device_handler::set_antenna(): channel " << channel << " antenna set [" << direction_string << "]: " << antenna_string << "." << std::endl;
}

void device_handler::set_analog_filter(int device_number, bool direction, int channel, int analog_filter, float analog_bandw)
{
    if(analog_filter == 1)
    {
        if(channel == 0 || channel == 1)
        {
            if(direction == LMS_CH_TX)
            {
                if(analog_bandw < 5e6 || analog_bandw > 130e6)
                {
                    std::cout << "ERROR: device_handler::set_analog_filter(): analog_bandw TX value must be [5e6,130e6]." << std::endl;
                    close_all_devices();
                }
                else
                {
                    std::cout << "INFO: device_handler::set_analog_filter(): ";
                    LMS_SetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,analog_bandw);
		    
		    double analog_value;
		    LMS_GetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,&analog_value);
		    std::cout << "Configured bandwidth: " << analog_value/1e6 << " MHz." << std::endl;
                }
            }
            else if(direction == LMS_CH_RX)
            {
                if(analog_bandw < 1.5e6 || analog_bandw > 130e6)
                {
                    std::cout << "ERROR: device_handler::set_analog_filter(): analog_bandw RX value must be [1.5e6,130e6]." << std::endl;
                    close_all_devices();
                }
                else
                {
                    std::cout << "INFO: device_handler::set_analog_filter(): ";
                    LMS_SetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,analog_bandw);
		    
		    double analog_value;
		    LMS_GetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,&analog_value);
		    std::cout << "Configured bandwidth: " << analog_value/1e6 << " MHz." << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR: device_handler::set_analog_filter(): direction must be 0(LMS_CH_RX) or 1(LMS_CH_TX)." << std::endl;
                close_all_devices();
            }
        }
        else
        {
            std::cout << "ERROR: device_handler::set_analog_filter(): channel must be 0 or 1." << std::endl;
            close_all_devices();
        }
    }
    else if(analog_filter == 0)
    {
	std::string direction_string;
	if(direction == LMS_CH_RX)
	  direction_string = "RX";
	else if(direction == LMS_CH_TX)
	  direction_string = "TX";
	
	std::cout << "INFO: device_handler::set_analog_filter(): analog filter channel " << channel << " [" << direction_string << "]: disabled (maximum value set)." << std::endl; 
	LMS_SetLPF(device_handler::getInstance().get_device(device_number),direction,channel,false);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_analog_filter(): analog_filter must be either enabled (1) or disabled (0). Disabling analog_filter due to incorrect parameters." << std::endl;
	LMS_SetLPF(device_handler::getInstance().get_device(device_number),direction,channel,false);
    }
}

void device_handler::set_digital_filter(int device_number, bool direction, int channel, int digital_filter, float digital_bandw)
{
  if(digital_filter != 0 && digital_filter != 1)
  {
    std::cout << "ERROR: device_handler::set_digital_filter(): digital_filter must be either enabled (1) or disabled (0). Disabling digital_filter due to incorrect parameters." << std::endl;
    LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),direction,channel,0,digital_bandw);
  }
  else if(digital_bandw < 0)
  {
    std::cout << "ERROR: device_handler::set_digital_filter(): digital_bandw must be more than 0." << std::endl;
    close_all_devices();
  }
  else if(digital_filter == 1)
  {

    LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),direction,channel,digital_filter,digital_bandw);

    std::string direction_string;
    if(direction == LMS_CH_RX)
      direction_string = "RX";
    else if(direction == LMS_CH_TX)
      direction_string = "TX";
    
    std::cout << "INFO: device_handler::set_digital_filter(): digital filter channel " << channel << " [" << direction_string << "]: " << digital_bandw/1e6 << " MHz." << std::endl;  
  }
  else if(digital_filter == 0)
  {

    LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),direction,channel,digital_filter,digital_bandw);

    std::string direction_string;
    if(direction == LMS_CH_RX)
      direction_string = "RX";
    else if(direction == LMS_CH_TX)
      direction_string = "TX";
    
    std::cout << "INFO: device_handler::set_digital_filter(): digital filter channel " << channel << " set [" << direction_string << "]: disabled." << std::endl;  
  }  
}

void device_handler::set_gain(int device_number, bool direction, int channel, unsigned int gain_dB)
{
    if((direction==LMS_CH_RX && gain_dB >= 0 && gain_dB <= 70)||(direction==LMS_CH_TX && gain_dB >= 0 && gain_dB <= 60))
    {
        if(LMS_SetGaindB(device_handler::getInstance().get_device(device_number),direction,channel,gain_dB)!=LMS_SUCCESS)
          device_handler::error(device_number);

	std::string direction_string;
	if(direction == LMS_CH_RX)
	  direction_string = "RX";
	else if(direction == LMS_CH_TX)
	  direction_string = "TX";

	unsigned int gain_value;
	LMS_GetGaindB(device_handler::getInstance().get_device(device_number),direction,channel,&gain_value);
	std::cout << "INFO: device_handler::set_gain(): set gain [" << direction_string << "]: " << gain_value << " dB." << std::endl;
    }
    else
    {
        std::cout << "ERROR: device_handler::set_gain(): valid RX gain range [0, 70], TX gain range [0, 60]." << std::endl;
        close_all_devices();
    }
}

void device_handler::set_nco(int device_number, bool direction, int channel, float nco_freq, float nco_pho, int cmix_mode)
{
  if(nco_freq == 0)
  {
    LMS_SetNCOIndex(device_handler::getInstance().get_device(device_number),direction,channel,-1,cmix_mode);
  }
  else
  {
    double freq_value_in[16];
    for (int i = 0; i < 16; ++i)
    {
      if(i==0)
	freq_value_in[i]=nco_freq;
      else
	freq_value_in[i]=0;
    }
    LMS_SetNCOFrequency(device_handler::getInstance().get_device(device_number),direction,channel,freq_value_in,nco_pho);
    LMS_SetNCOIndex(device_handler::getInstance().get_device(device_number),direction,channel,0,cmix_mode);
    std::string direction_string;
    if(direction == LMS_CH_RX)
      direction_string = "RX";
    else if(direction == LMS_CH_TX);
      direction_string = "TX";
    
    double freq_value_out[16];
    double pho_value_out[16];
    LMS_GetNCOFrequency(device_handler::getInstance().get_device(device_number),direction,channel,freq_value_out,pho_value_out);
    std::cout << "INFO: device_handler::set_nco(): set channel " << channel << " NCO [" << direction_string << "]: " << freq_value_out[0]/1e6 << " MHz (" << pho_value_out[0] << " deg.)." << std::endl;
  }
}
