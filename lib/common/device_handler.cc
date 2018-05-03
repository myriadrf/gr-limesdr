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
    exit(0);
}

device_handler::~device_handler()
{
    close_all_devices();
}

lms_device_t* device_handler::get_device(int device_number)
{
    return this->device_vector[device_number].address;
}

void device_handler::open_device(int device_number, int device_type)
{
    // Check device_number value
    if(device_number < 0)
    {
        std::cout << "ERROR: device_handler::open_device(): device number cannot be lower than 0." << std::endl;
        close_all_devices();
        exit(0);
    }

    // Read device list once
    if(list_read == false)
    {
        if ((device_count = LMS_GetDeviceList(NULL)) < LMS_SUCCESS)
            error(device_number);
        std::cout << "##################" << std::endl;
        std::cout << "Devices found: " << device_count << std::endl;

        if (device_count < 1)
            std::exit(0);
        std::cout << "##################" << std::endl;
        std::cout << "Device list:" << std::endl;
        LMS_GetDeviceList(list);

        for (int i = 0; i < device_count; i++)
        {
            std::cout << "Nr.:|" <<  i << "|device:|" << list[i] << std::endl;
            device_vector.push_back(device());
        }
        std::cout << "##################" << std::endl;
        list_read = true;
    }

    // Compare maximum device list value with device_number value
    if(device_number > device_count-1)
    {
        std::cout << "ERROR: device_handler::open_device(): device number " << device_number << " is not connected to this system. Please check the device list." << std::endl;
        close_all_devices();
        std::cout << "---------------------------------------------------------------" <<  std::endl;
        exit(0);
    }

    // If device slot is empty, open and initialize device
    if(device_vector[device_number].address == NULL)
    {
        std::string search_name;
        if (device_type == 1)
            search_name = "LimeSDR Mini";
        else if (device_type == 2)
            search_name = "LimeSDR-USB";
        else
        {
            std::cout << "ERROR: device_handler::open_device(): wrong device_type." << std::endl;
	    close_all_devices();
            exit(0);
        }
        std::cout << "##################" << std::endl;
        std::cout << "New connection" << std::endl;

	std::string device_string(list[device_number]);

	if (device_string.find(search_name) != std::string::npos)
	{
	    if (LMS_Open(&device_vector[device_number].address, list[device_number], NULL))
		error(device_number);

	    std::cout << "Device number " << device_number << " from the list is used." << std::endl;

	    if (LMS_Init(device_vector[device_number].address) != LMS_SUCCESS)
		error(device_number);

	    std::cout << "##################" << std::endl;
	    std::cout << std::endl;

	    ++open_devices; // Count open devices
	}
	else if ((device_string.find(search_name) == std::string::npos))
	{
	    std::cout << "Device number " << device_number << " from the list is not " << search_name << "." << std::endl;
	    close_all_devices();
	    exit(0);
	    std::cout << "---------------------------------------------------------------" <<  std::endl;
	}
    }
}

void device_handler::close_device(int device_number)
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

void device_handler::close_all_devices()
{
    for(int i = 0; i <= open_devices; i++)
        this->close_device(i);
}

void device_handler::check_blocks(int device_number, int block_type, int device_type, int chip_mode, double sample_rate, size_t oversample, int file_switch, const char *filename)
{
    // Check how many blocks of each type are used for each device
    switch(block_type)
    {
    case 1: // Source block
        if(device_vector[device_number].source_flag == true)
        {
            std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Source (RX) block is allowed per device." << std::endl;
            close_all_devices();
            exit(0);
        }
        else
        {
            device_vector[device_number].source_flag = true;
            device_vector[device_number].source_device_type = device_type;
            device_vector[device_number].source_chip_mode = chip_mode;
            if(file_switch == 0)
            {
                device_vector[device_number].source_sample_rate = sample_rate;
                device_vector[device_number].source_oversample = oversample;
            }
            else
            {
                device_vector[device_number].source_file_switch = file_switch;
                device_vector[device_number].source_filename = filename;
            }
        }
        break;

    case 2: // Sink block
        if(device_vector[device_number].sink_flag == true)
        {
            std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Sink (TX) block is allowed per device." << std::endl;
            device_handler::getInstance().close_device(device_number);
            exit(0);
        }
        else
        {
            device_vector[device_number].sink_flag = true;
            device_vector[device_number].sink_device_type = device_type;
            device_vector[device_number].sink_chip_mode = chip_mode;
            if(file_switch == 0)
            {
                device_vector[device_number].sink_sample_rate = sample_rate;
                device_vector[device_number].sink_oversample = oversample;
            }
            else
            {
                device_vector[device_number].sink_file_switch = file_switch;
                device_vector[device_number].sink_filename = filename;
            }
        }
        break;

    default:
        std::cout << "ERROR: device_handler::check_blocks(): incorrect block_type value." << std::endl;
        close_all_devices();
        exit(0);
    }

    // Check matching block settings
    if(device_vector[device_number].source_flag && device_vector[device_number].sink_flag)
    {
        if(device_vector[device_number].source_device_type != device_vector[device_number].sink_device_type)
        {
            std::cout << "ERROR: device_handler::check_blocks(): device_type mismatch in LimeSuite Source (RX) and LimeSuite Sink (TX) blocks." << std::endl;
            close_all_devices();
            exit(0);
        }
        else if(device_vector[device_number].source_chip_mode != device_vector[device_number].sink_chip_mode)
        {
            std::cout << "ERROR: device_handler::check_blocks(): chip_mode mismatch in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
            exit(0);
        }
        else if(device_vector[device_number].source_file_switch != device_vector[device_number].sink_file_switch)
        {
            std::cout << "ERROR: device_handler::check_blocks(): file_switch must match in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
            exit(0);
        }
        else if(device_vector[device_number].source_file_switch && device_vector[device_number].sink_file_switch == 0)
        {
            if((device_vector[device_number].source_sample_rate != device_vector[device_number].sink_sample_rate) && (device_type == 1))
            {
                std::cout << "ERROR: device_handler::check_blocks(): sample_rate must match in LimeSuite Source (RX) and LimeSuite Sink (TX) when using LimeSDR-Mini." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if((device_vector[device_number].source_oversample != device_vector[device_number].sink_oversample) && (device_type == 1))
            {
                std::cout << "ERROR: device_handler::check_blocks(): oversample must match in LimeSuite Source (RX) and LimeSuite Sink (TX) when using LimeSDR-Mini." << std::endl;
                close_all_devices();
                exit(0);
            }
        }
        else if(device_vector[device_number].source_filename != device_vector[device_number].sink_filename && device_vector[device_number].source_file_switch && device_vector[device_number].sink_file_switch == 1)
        {
            std::cout << "ERROR: device_handler::check_blocks(): file must match in LimeSuite Source (RX) and LimeSuite Sink (TX)." << std::endl;
            close_all_devices();
            exit(0);
        }
    }
}

void device_handler::settings_from_file(int device_number, const char *filename)
{
    if(LMS_LoadConfig(device_handler::getInstance().get_device(device_number),filename))
        device_handler::getInstance().error(device_number);
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
            exit(0);
        }
        else if(7.68e6 < rate && rate <= 15.36e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): when LimeSDR-Mini samp_rate is more than 7.68e6 S/s and less or equal 15.36e6 S/s, oversample cannot be higher than 8." << std::endl;
            close_all_devices();
            exit(0);
        }
        else if(3.84e6 < rate && rate <= 7.68e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
        {
            std::cout << "ERROR: device_handler::set_samp_rate(): when LimeSDR-Mini samp_rate is more than 3.84e6 S/s and less or equal 7.68e6 S/s, oversample cannot be higher than 16." << std::endl;
            close_all_devices();
            exit(0);
        }
        if(LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),rate,oversample)!=LMS_SUCCESS)
            device_handler::getInstance().error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_samp_rate(): valid oversample values are: 0,1,2,4,8,16,32." << std::endl;
        close_all_devices();
        exit(0);
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
                exit(0);
            }
            else if(30.72e6 < rate && rate <= 61.44e6 && oversample != 0 && oversample != 1 && oversample != 2)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 30.72e6 S/s and less or equal 61.44e6 S/s, oversample cannot be higher than 2." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(15.36e6 < rate && rate <= 30.72e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 15.36e6 S/s and less or equal 30.72e6 S/s, oversample cannot be higher than 4." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(7.68e6 < rate && rate <= 15.36e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 7.68e6 S/s and less or equal 15.36e6 S/s, oversample cannot be higher than 8." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(3.84e6 < rate && rate <= 7.68e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when RX samp_rate is more than 3.84e6 S/s and less or equal 7.68e6 S/s, oversample cannot be higher than 16." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,oversample)!=LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
        }
        else if(direction == LMS_CH_TX)
        {
            if(rate > 61.44e6)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): TX samp_rate cannot be more than 61.44e6 S/s." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(30.72e6 < rate && rate <= 61.44e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when TX samp_rate is more than 30.72e6 S/s and less or equal 61.44e6 S/s, oversample cannot be higher than 8." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(15.36e6 < rate && rate <= 30.72e6 && oversample != 0 && oversample != 1 && oversample != 2 && oversample != 4 && oversample != 8 && oversample != 16)
            {
                std::cout << "ERROR: device_handler::set_samp_rate(): when TX samp_rate is more than 15.36e6 S/s and less or equal 30.72e6 S/s, oversample cannot be higher than 16." << std::endl;
                close_all_devices();
                exit(0);
            }
            else if(LMS_SetSampleRateDir(device_handler::getInstance().get_device(device_number),direction,rate,oversample)!=LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
        }
    }
    else
    {
        std::cout << "ERROR: device_handler::set_samp_rate_dir() valid oversample values are: 0,1,2,4,8,16,32." << std::endl;
        close_all_devices();
        exit(0);
    }
}

void device_handler::set_rf_freq(int device_number, int device_type, bool direction, int channel, float rf_freq)
{
    if(rf_freq <= 0)
    {
        std::cout << "ERROR: device_handler::set_rf_freq(): rf_freq must be more than 0 Hz." << std::endl;
        close_all_devices();
        exit(0);
    }
    else
    {
        if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), direction, channel, rf_freq) != LMS_SUCCESS)
            device_handler::error(device_number);
    }
}

void device_handler::calibrate(int device_number, int device_type, int calibration, int direction, int channel, double bandwidth, float rf_freq, int path)
{
    if(calibration == 1 && direction == LMS_CH_RX)
    {
      std::cout << "INFO: device_handler::calibrate(): ";
        if(device_type == 2 && path == 2) // LNAL matching workaround [LimeSDR-USB]
        {
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 200e6) != LMS_SUCCESS)
                device_handler::error(device_number);
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq) != LMS_SUCCESS)
                device_handler::error(device_number);
        }
        else if(rf_freq >= 31e6) // Normal calibration
        {
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
        }
        else if(device_type == 1)// Lower than 31 MHz center frequency workaround [LimeSDR-Mini]
        {
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 100e6) != LMS_SUCCESS)
                device_handler::error(device_number);
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq) != LMS_SUCCESS)
                device_handler::error(device_number);
        }
        else if(device_type == 2)// Lower than 31 MHz center frequency workaround [LimeSDR-USB]
        {
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, 50e6) != LMS_SUCCESS)
                device_handler::error(device_number);
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_RX, channel, rf_freq) != LMS_SUCCESS)
                device_handler::error(device_number);
        }
    }
    else if(calibration == 1 && direction == LMS_CH_TX)
    {
      std::cout << "INFO: device_handler::calibrate(): ";
        if(rf_freq >= 31e6)
        {
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
        }
        else
        {
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, 50e6) != LMS_SUCCESS)
                device_handler::error(device_number);
            if(LMS_Calibrate(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, bandwidth, 0)!= LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            if (LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number), LMS_CH_TX, channel, rf_freq) != LMS_SUCCESS)
                device_handler::error(device_number);
        }
    }
}

void device_handler::mini_switch_workaround(int device_number)
{
    if(device_vector[device_number].mini_switch_workaround_flag == false)
    {
        device_vector[device_number].mini_switch_workaround_flag = true;

        uint16_t addr = 0x0017;
        uint16_t default_value = 0;
        uint16_t value = 0x2204;

        if(LMS_ReadFPGAReg(device_handler::getInstance().get_device(device_number),addr, &default_value))
            device_handler::error(device_number);

        if(LMS_WriteFPGAReg(device_handler::getInstance().get_device(device_number),addr, value))
            device_handler::error(device_number);

        if(LMS_WriteFPGAReg(device_handler::getInstance().get_device(device_number),addr, default_value))
            device_handler::error(device_number);
    }
}

void device_handler::set_lna_path_mini(int device_number, int lna_path_mini)
{

    if(lna_path_mini == 1 || lna_path_mini == 3)
    {
        uint16_t addr = 0x0017;
        uint16_t value = 0;
        int bit = 0;
        if(LMS_ReadFPGAReg(device_handler::getInstance().get_device(device_number),addr, &value))
            device_handler::error(device_number);

        if(lna_path_mini==1)
        {
            bit = (value >> 8) & 1UL;
            if(bit == 0)
            {
                value |= 1UL << 8;
                value &= ~(1UL << 9);
            }
            else if(bit == 1)
            {

            }
            else
            {
                std::cout << "ERROR: device_handler::set_lna_path_mini(): failed to read 8th bit in 0x0017 registry." << std::endl;
                close_all_devices();
                exit(0);
            }
        }
        else if(lna_path_mini==3)
        {
            bit = (value >> 9) & 1UL;
            if(bit == 0)
            {
                value |= 1UL << 9;
                value &= ~(1UL << 8);
            }
            else if(bit == 1)
            {

            }
            else
            {
                std::cout << "ERROR: device_handler::set_lna_path_mini(): failed to read 9th bit in 0x0017 registry." << std::endl;
                close_all_devices();
                exit(0);
            }
        }

        if(LMS_WriteFPGAReg(device_handler::getInstance().get_device(device_number),addr, value))
            device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_lna_path_mini(): lna_path_mini must be 1 or 3." << std::endl;
        close_all_devices();
        exit(0);
    }
}

void device_handler::set_lna_path(int device_number, int channel, int lna_path)
{
    if(lna_path == 1 || lna_path == 2 || lna_path == 3)
    {
      if(LMS_SetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_RX,channel,lna_path))
	device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_lna_path(): lna_path value must be 1,2,3." << std::endl;
        close_all_devices();
        exit(0);
    }
}

void device_handler::set_pa_path_mini(int device_number, int pa_path_mini)
{
    if(pa_path_mini == 1 || pa_path_mini == 2)
    {
        uint16_t addr = 0x0017;
        uint16_t value = 0;
        int bit = 0;

        if(LMS_ReadFPGAReg(device_handler::getInstance().get_device(device_number),addr, &value))
            device_handler::error(device_number);
        if(pa_path_mini==1)
        {
            bit = (value >> 12) & 1UL;
            if(bit == 0)
            {
                value |= 1UL << 12;
                value &= ~(1UL << 13);
            }
            else if(bit == 1)
            {

            }
            else
            {
                std::cout << "ERROR: device_handler::set_pa_path_mini(): failed to read 12th bit in 0x0017 registry." << std::endl;
                close_all_devices();
                exit(0);
            }
        }
        else if(pa_path_mini==2)
        {
            bit = (value >> 13) & 1UL;
            if(bit == 0)
            {
                value |= 1UL << 13;
                value &= ~(1UL << 12);
            }
            else if(bit == 1)
            {

            }
            else
            {
                std::cout << "ERROR: device_handler::set_pa_path_mini(): failed to read 13th bit in 0x0017 registry." << std::endl;
                close_all_devices();
                exit(0);
            }
        }

        if(LMS_WriteFPGAReg(device_handler::getInstance().get_device(device_number),addr, value))
            device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_pa_path_mini(): pa_path_mini must be 1 or 2." << std::endl;
        close_all_devices();
        exit(0);
    }
}

void device_handler::set_pa_path(int device_number, int channel, int pa_path)
{
    if(pa_path == 1 || pa_path == 2)
    {
      if(LMS_SetAntenna(device_handler::getInstance().get_device(device_number),LMS_CH_TX,channel,pa_path))
	device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_pa_path(): pa_path value must be 1,2." << std::endl;
        close_all_devices();
        exit(0);
    }
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
                    exit(0);
                }
                else
                {
                    std::cout << "INFO: device_handler::set_analog_filter(): ";
                    if(LMS_SetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,analog_bandw)!=LMS_SUCCESS)
                        device_handler::error(device_number);
                }
            }
            else if(direction == LMS_CH_RX)
            {
                if(analog_bandw < 1.5e6 || analog_bandw > 130e6)
                {
                    std::cout << "ERROR: device_handler::set_analog_filter(): analog_bandw RX value must be [1.5e6,130e6]." << std::endl;
                    close_all_devices();
                    exit(0);
                }
                else
                {
                    std::cout << "INFO: device_handler::set_analog_filter(): ";
                    if(LMS_SetLPFBW(device_handler::getInstance().get_device(device_number),direction,channel,analog_bandw)!=LMS_SUCCESS)
                        device_handler::error(device_number);
                }
            }
            else
            {
                std::cout << "ERROR: device_handler::set_analog_filter(): direction must be 0(LMS_CH_RX) or 1(LMS_CH_TX)." << std::endl;
                close_all_devices();
                exit(0);
            }
        }
        else
        {
            std::cout << "ERROR: device_handler::set_analog_filter(): channel must be 0 or 1." << std::endl;
            close_all_devices();
            exit(0);
        }
    }
    else if(analog_filter == 0)
    {
        if(LMS_SetLPF(device_handler::getInstance().get_device(device_number), direction, channel, false)!=LMS_SUCCESS)
            device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_analog_filter(): analog_filter must be 0(OFF) or 1(ON)." << std::endl;
        close_all_devices();
        exit(0);
    }
}

void device_handler::set_digital_filter(int device_number, bool direction, int channel, int digital_filter, float digital_bandw)
{
  if(digital_filter != 0 && digital_filter != 1)
  {
    std::cout << "ERROR: device_handler::set_digital_filter(): digital_filter must be 0(OFF) or 1(ON)." << std::endl;
    close_all_devices();
    exit(0);
  }
  else if(digital_bandw < 0)
  {
    std::cout << "ERROR: device_handler::set_digital_filter(): digital_bandw must be more than 0." << std::endl;
    close_all_devices();
    exit(0);
  }
  else
  {

    if(LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),direction,channel,digital_filter,digital_bandw)!=LMS_SUCCESS)
        device_handler::error(device_number);
  }
}

void device_handler::set_gain(int device_number, bool direction, int channel, unsigned gain_dB)
{
    if((direction==LMS_CH_RX && gain_dB >= 0 && gain_dB <= 70)||(direction==LMS_CH_TX && gain_dB >= 0 && gain_dB <= 60))
    {
        if(LMS_SetGaindB(device_handler::getInstance().get_device(device_number),direction,channel,gain_dB)!=LMS_SUCCESS)
            device_handler::error(device_number);
    }
    else
    {
        std::cout << "ERROR: device_handler::set_gain(): valid RX gain range [0, 70], TX gain range [0, 60]." << std::endl;
        close_all_devices();
        exit(0);
    }
}
