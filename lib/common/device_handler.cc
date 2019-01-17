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

#include "device_handler.h"
#include <LMS7002M_parameters.h>

device_handler::~device_handler() { delete list; }

void device_handler::error(int device_number) {
    std::cout << "ERROR: " << LMS_GetLastErrorMessage() << std::endl;
    if (this->device_vector[device_number].address != NULL)
        close_all_devices();
}

lms_device_t* device_handler::get_device(int device_number) {
    return this->device_vector[device_number].address;
}

int device_handler::open_device(std::string& serial, int device_type) {

    int device_number;
    std::string search_name;
    std::cout << "##################" << std::endl;
    std::cout << "Connecting to device" << std::endl;

    // Read device list once
    if (list_read == false) {
        device_count = LMS_GetDeviceList(list);
        std::cout << "##################" << std::endl;
        std::cout << "Devices found: " << device_count << std::endl;
        std::cout << "##################" << std::endl;
        if (device_count < 1)
            exit(0);
        std::cout << "Device list:" << std::endl;

        for (int i = 0; i < device_count; i++) {
            std::cout << "Nr.:|" << i << "|device:|" << list[i] << std::endl;
            device_vector.push_back(device());
        }
        std::cout << "##################" << std::endl;
        list_read = true;
    }

    // Set device string search parameters
    if (device_type == LimeSDR_Mini) {
        search_name = "LimeSDR Mini";
    } else if (device_type == LimeSDR_USB) {
        search_name = "LimeSDR-USB";
    } else if (device_type == LimeNET_Micro) {
        search_name = "LimeNET-Micro";
    } else {
        std::cout << "ERROR: device_handler::open_device(): wrong device_type." << std::endl;
        close_all_devices();
    }

    if (serial.empty()) {
        std::cout
            << "INFO: device_handler::open_device(): no serial number. Using first " << search_name
            << " in the list. Use \"LimeUtil --find\" in terminal to find prefered device serial."
            << std::endl;
        std::cout << "##################" << std::endl;
    }

    // Identify device by device type and serial number
    for (int i = 0; i < device_count; i++) {
        std::string device_string(list[i]);
        unsigned first = device_string.find("serial=");
        std::string aquired_serial = device_string.substr(first + 7, std::string::npos);

        // If serial is left empty, use first device in list
        if (serial.empty() && device_string.find(search_name) != std::string::npos) {
            device_number = i;
            serial = aquired_serial;
            break;
        }
        // If device_type and serial match found acquire device number
        else if (device_string.find(search_name) != std::string::npos && aquired_serial == serial) {
            device_number = i;
            break;
        }
        // If program was unable to find device in list print error and stop program
        else if ((i == device_count - 1 && aquired_serial != serial) ||
                 (device_string.find(search_name) == std::string::npos &&
                  aquired_serial == serial)) {
            if (serial.empty())
                std::cout << "Unable to find " << search_name << std::endl;
            else
                std::cout << "Unable to find " << search_name << " with device serial " << serial
                          << "." << std::endl;
            std::cout << "##################" << std::endl;
            close_all_devices();
        }
    }

    // If device slot is empty, open and initialize device
    if (device_vector[device_number].address == NULL) {
        std::cout << "Device number " << device_number << " from the list is used." << std::endl;
        LMS_Open(&device_vector[device_number].address, list[device_number], NULL);
        LMS_Init(device_vector[device_number].address);
        ++open_devices; // Count open devices
        std::cout << "##################" << std::endl;
        std::cout << std::endl;
    }
    // If device is open do nothing
    else {
        std::cout << "Previously connected device number " << device_number
                  << " from the list is used." << std::endl;
        std::cout << "##################" << std::endl;
        std::cout << std::endl;
    }

    return device_number; // return device number to identify device_vector[device_number].address
                          // connection in other functions
}

void device_handler::close_device(int device_number, int block_type) {
    // Check if other block finished and close device
    if (device_vector[device_number].source_flag == false ||
        device_vector[device_number].sink_flag == false) {
        if (device_vector[device_number].address != NULL) {
            std::cout << std::endl;
            std::cout << "##################" << std::endl;
            if (LMS_Reset(this->device_vector[device_number].address) != LMS_SUCCESS)
                error(device_number);
            if (LMS_Close(this->device_vector[device_number].address) != LMS_SUCCESS)
                error(device_number);
            std::cout << "INFO: device_handler::close_device(): Disconnected from device number "
                      << device_number << "." << std::endl;
            device_vector[device_number].address = NULL;
            std::cout << "##################" << std::endl;
            std::cout << std::endl;
        }
    }
    // If two blocks used switch one block flag and let other block finish work
    // Switch flag when closing device
    switch (block_type) {
    case 1:
        device_vector[device_number].source_flag = false;
        break;
    case 2:
        device_vector[device_number].sink_flag = false;
        break;
    }
}

void device_handler::close_all_devices() {
    if (close_flag == false) {
        for (int i = 0; i <= open_devices; i++) {
            if (this->device_vector[i].address != NULL) {
                LMS_Reset(this->device_vector[i].address);
                LMS_Close(this->device_vector[i].address);
            }
        }
        close_flag == true;
        exit(0);
    }
}

void device_handler::check_blocks(int device_number,
                                  int block_type,
                                  int device_type,
                                  int chip_mode,
                                  double sample_rate,
                                  size_t oversample,
                                  int file_switch,
                                  const char* filename) {
    // Get each block settings
    switch (block_type) {
    case 1: // Source block
        if (device_vector[device_number].source_flag == true) {
            std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Source (RX) "
                         "block is allowed per device."
                      << std::endl;
            close_all_devices();
        } else {
            device_vector[device_number].source_flag = true;
            device_vector[device_number].source_device_type = device_type;
            device_vector[device_number].source_chip_mode = chip_mode;
            device_vector[device_number].source_file_switch = file_switch;
            if (file_switch == 0) {
                device_vector[device_number].source_sample_rate = sample_rate;
                device_vector[device_number].source_oversample = oversample;
            } else {
                device_vector[device_number].source_filename = filename;
            }
        }
        break;

    case 2: // Sink block
        if (device_vector[device_number].sink_flag == true) {
            std::cout << "ERROR: device_handler::check_blocks(): only one LimeSuite Sink (TX) "
                         "block is allowed per device."
                      << std::endl;
            close_all_devices();
        } else {
            device_vector[device_number].sink_flag = true;
            device_vector[device_number].sink_device_type = device_type;
            device_vector[device_number].sink_chip_mode = chip_mode;
            device_vector[device_number].sink_file_switch = file_switch;
            if (file_switch == 0) {
                device_vector[device_number].sink_sample_rate = sample_rate;
                device_vector[device_number].sink_oversample = oversample;
            } else {
                device_vector[device_number].sink_filename = filename;
            }
        }
        break;

    default:
        std::cout << "ERROR: device_handler::check_blocks(): incorrect block_type value."
                  << std::endl;
        close_all_devices();
    }

    // Check block settings which must match
    if (device_vector[device_number].source_flag && device_vector[device_number].sink_flag) {
        // Device_type must match in blocks with the same serial
        if (device_vector[device_number].source_device_type !=
            device_vector[device_number].sink_device_type) {
            std::cout << "ERROR: device_handler::check_blocks(): device_type mismatch in LimeSuite "
                         "Source (RX) and LimeSuite Sink (TX) blocks."
                      << std::endl;
            close_all_devices();
        }

        // Chip_mode must match in blocks with the same serial
        if (device_vector[device_number].source_chip_mode !=
            device_vector[device_number].sink_chip_mode) {
            std::cout << "Source: " << device_vector[device_number].source_chip_mode << std::endl;
            std::cout << "Sink: " << device_vector[device_number].sink_chip_mode << std::endl;
            std::cout << "ERROR: device_handler::check_blocks(): chip_mode mismatch in LimeSuite "
                         "Source (RX) and LimeSuite Sink (TX)."
                      << std::endl;
            close_all_devices();
        }

        // File_switch must match in blocks with the same serial
        if (device_vector[device_number].source_file_switch !=
            device_vector[device_number].sink_file_switch) {
            std::cout << "ERROR: device_handler::check_blocks(): file_switch must match in "
                         "LimeSuite Source (RX) and LimeSuite Sink (TX)."
                      << std::endl;
            close_all_devices();
        }

        // When file_switch is 0 check LimeSDR-Mini or LimeNET-Micro sample_rate and oversample
        // match throughout the blocks with the same serial
        if ((device_vector[device_number].source_file_switch == 0) &&
            (device_vector[device_number].sink_file_switch == 0)) {
            if ((device_vector[device_number].source_sample_rate !=
                 device_vector[device_number].sink_sample_rate)) {
                std::cout << "ERROR: device_handler::check_blocks(): sample_rate must match in "
                             "LimeSuite Source (RX) and LimeSuite Sink (TX) when using "
                          << device_string[device_type - 1] << std::endl;
                close_all_devices();
            } else if ((device_vector[device_number].source_oversample !=
                        device_vector[device_number].sink_oversample)) {
                std::cout << "ERROR: device_handler::check_blocks(): oversample must match in "
                             "LimeSuite Source (RX) and LimeSuite Sink (TX) when using "
                          << device_string[device_type - 1] << std::endl;
                close_all_devices();
            }
        }

        // When file_switch is 1 check filename match throughout the blocks with the same serial
        if (device_vector[device_number].source_filename !=
                device_vector[device_number].sink_filename &&
            (device_vector[device_number].source_file_switch == 1) &&
            (device_vector[device_number].sink_file_switch == 1)) {
            std::cout << "ERROR: device_handler::check_blocks(): file must match in LimeSuite "
                         "Source (RX) and LimeSuite Sink (TX)."
                      << std::endl;
            close_all_devices();
        }
    }
}

void device_handler::settings_from_file(int device_number, const char* filename) {
    if (LMS_LoadConfig(device_handler::getInstance().get_device(device_number), filename))
        device_handler::getInstance().error(device_number);

    // Set LimeSDR-Mini switches based on .ini file
    int antenna_ch0_tx;
    int antenna_ch0_rx;
    antenna_ch0_tx = LMS_GetAntenna(
        device_handler::getInstance().get_device(device_number), LMS_CH_TX, LMS_CH_0);
    antenna_ch0_rx = LMS_GetAntenna(
        device_handler::getInstance().get_device(device_number), LMS_CH_RX, LMS_CH_0);

    LMS_SetAntenna(device_handler::getInstance().get_device(device_number),
                   LMS_CH_TX,
                   LMS_CH_0,
                   antenna_ch0_tx);
    LMS_SetAntenna(device_handler::getInstance().get_device(device_number),
                   LMS_CH_RX,
                   LMS_CH_0,
                   antenna_ch0_rx);
}

void device_handler::set_chip_mode(
    int device_number, int device_type, int chip_mode, int channel, bool direction) {
    if ((device_type == LimeSDR_Mini || device_type == LimeNET_Micro) && chip_mode >= 3) {
        std::cout << "ERROR: device_handler::set_chip_mode():" << device_string[device_type - 1];
        std::cout << " supports only SISO mode." << std::endl;
        close_all_devices();
        exit(0);
    } else {
        if (chip_mode < 3) {
            if (LMS_EnableChannel(device_handler::getInstance().get_device(device_number),
                                  direction,
                                  channel,
                                  true) != LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            std::cout << "INFO: device_handler::set_chip_mode(): SISO mode set for device number "
                      << device_number << "." << std::endl;
        } else if (chip_mode == 3) {
            if (LMS_EnableChannel(device_handler::getInstance().get_device(device_number),
                                  direction,
                                  LMS_CH_0,
                                  true) != LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            if (LMS_EnableChannel(device_handler::getInstance().get_device(device_number),
                                  direction,
                                  LMS_CH_1,
                                  true) != LMS_SUCCESS)
                device_handler::getInstance().error(device_number);
            std::cout << "INFO: device_handler::set_chip_mode(): MIMO mode set for device number "
                      << device_number << "." << std::endl;
        }
    }
}

void device_handler::set_samp_rate(int device_number, double& rate, size_t oversample) {
    if (oversample == 0 || oversample == 1 || oversample == 2 || oversample == 4 ||
        oversample == 8 || oversample == 16 || oversample == 32) {

        if (LMS_SetSampleRate(device_handler::getInstance().get_device(device_number),
                              rate,
                              oversample) != LMS_SUCCESS)
            device_handler::getInstance().error(device_number);

        double host_value;
        double rf_value;
        if (LMS_GetSampleRate(device_handler::getInstance().get_device(device_number),
                              LMS_CH_RX,
                              LMS_CH_0,
                              &host_value,
                              &rf_value))
            device_handler::getInstance().error(device_number);
        std::cout << "INFO: device_handler::set_samp_rate(): set sampling rate: "
                  << host_value / 1e6 << " MS/s." << std::endl;
        std::cout << "INFO: device_handler::set_samp_rate(): set oversampling: "
                  << rf_value / host_value << std::endl;
        rate = host_value; // Get the real rate back;
    } else {
        std::cout << "ERROR: device_handler::set_samp_rate(): valid oversample values are: "
                     "0,1,2,4,8,16,32."
                  << std::endl;
        close_all_devices();
    }
}

void device_handler::set_rf_freq(
    int device_number, int device_type, bool direction, int channel, float rf_freq) {
    if (rf_freq <= 0) {
        std::cout << "ERROR: device_handler::set_rf_freq(): rf_freq must be more than 0 Hz."
                  << std::endl;
        close_all_devices();
    } else {
        int status = LMS_SetLOFrequency(
            device_handler::getInstance().get_device(device_number), direction, channel, rf_freq);
        double value = 0;
        LMS_GetLOFrequency(
            device_handler::getInstance().get_device(device_number), direction, channel, &value);

        std::string s_dir[2] = {"RX", "TX"};
        std::cout << "INFO: device_handler::set_rf_freq(): RF frequency set [" << s_dir[direction]
                  << "]: " << value / 1e6 << " MHz." << std::endl;
    }
}

void device_handler::calibrate(int device_number,
                               int device_type,
                               int calibration,
                               int direction,
                               int channel,
                               double bandwidth) {
    if (calibration == 1) {
        std::cout << "INFO: device_handler::calibrate(): ";
        int path = LMS_GetAntenna(
            device_handler::getInstance().get_device(device_number), direction, channel);
        float_type rf_freq = 0;
        LMS_GetLOFrequency(
            device_handler::getInstance().get_device(device_number), direction, channel, &rf_freq);
        if (rf_freq < 31e6 ||
            (device_type == LimeSDR_USB && path == 2 && direction == LMS_CH_RX)) // Workaround
        {
            float workaround_freq = 50e6; // Lower than 31 MHz center frequency workaround

            if (device_type == LimeSDR_USB && path == 2 &&
                direction == LMS_CH_RX) // LNAL matching workaround [LimeSDR-USB]
                workaround_freq = 200e6;
            else if (device_type == LimeSDR_Mini &&
                     direction == LMS_CH_RX) // RX workaround [LimeSDR-Mini]
                workaround_freq = 100e6;

            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number),
                               LMS_CH_RX,
                               channel,
                               workaround_freq);
            LMS_Calibrate(device_handler::getInstance().get_device(device_number),
                          LMS_CH_RX,
                          channel,
                          bandwidth,
                          0);
            LMS_SetLOFrequency(device_handler::getInstance().get_device(device_number),
                               LMS_CH_RX,
                               channel,
                               rf_freq);
        } else { // Normal calibration
            LMS_Calibrate(device_handler::getInstance().get_device(device_number),
                          direction,
                          channel,
                          bandwidth,
                          0);
        }
    } else if (calibration == 0) {
        std::string s_dir[2] = {"RX", "TX"};
        std::cout << "INFO: device_handler::calibrate(): calibration channel " << channel << " ["
                  << s_dir[direction] << "]: disabled." << std::endl;
    } else {
        std::cout << "ERROR: device_handler::calibrate(): calibration must be either enabled (1) "
                     "or disabled (0). Disabling calibration due to incorrect parameters."
                  << std::endl;
    }
}

void device_handler::set_antenna(int device_number, int channel, int direction, int antenna) {
    LMS_SetAntenna(
        device_handler::getInstance().get_device(device_number), direction, channel, antenna);
    int antenna_value =
        LMS_GetAntenna(device_handler::getInstance().get_device(device_number), direction, channel);

    std::string s_antenna[2][4] = {{"NONE", "LNAH", "LNAL", "LNAW"},
                                   {"NONE", "BAND1", "BAND2", "NONE"}};
    std::string s_dir[2] = {"RX", "TX"};

    std::cout << "INFO: device_handler::set_antenna(): channel " << channel << " antenna set ["
              << s_dir[direction] << "]: " << s_antenna[direction][antenna_value] << "."
              << std::endl;
}

void device_handler::set_analog_filter(
    int device_number, bool direction, int channel, int analog_filter, float analog_bandw) {
    if (analog_filter == 1) {
        if (channel == 0 || channel == 1) {
            if (direction == LMS_CH_TX || direction == LMS_CH_RX) {
                std::cout << "INFO: device_handler::set_analog_filter(): ";
                LMS_SetLPFBW(device_handler::getInstance().get_device(device_number),
                             direction,
                             channel,
                             analog_bandw);

                double analog_value;
                LMS_GetLPFBW(device_handler::getInstance().get_device(device_number),
                             direction,
                             channel,
                             &analog_value);
                std::cout << "configured bandwidth: " << analog_value / 1e6 << " MHz." << std::endl;
            } else {
                std::cout << "ERROR: device_handler::set_analog_filter(): direction must be "
                             "0(LMS_CH_RX) or 1(LMS_CH_TX)."
                          << std::endl;
                close_all_devices();
            }
        } else {
            std::cout << "ERROR: device_handler::set_analog_filter(): channel must be 0 or 1."
                      << std::endl;
            close_all_devices();
        }
    } else if (analog_filter == 0) {

        std::string s_dir[2] = {"RX", "TX"};
        std::cout << "INFO: device_handler::set_analog_filter(): analog filter channel " << channel
                  << " [" << s_dir[direction] << "]: disabled (maximum value set)." << std::endl;
        LMS_SetLPF(
            device_handler::getInstance().get_device(device_number), direction, channel, false);
    } else {
        std::cout << "ERROR: device_handler::set_analog_filter(): analog_filter must be either "
                     "enabled (1) or disabled (0). Disabling analog_filter due to incorrect "
                     "parameters."
                  << std::endl;
        LMS_SetLPF(
            device_handler::getInstance().get_device(device_number), direction, channel, false);
    }
}

void device_handler::set_digital_filter(
    int device_number, bool direction, int channel, int digital_filter, float digital_bandw) {
    if (digital_filter != 0 && digital_filter != 1) {
        std::cout << "ERROR: device_handler::set_digital_filter(): digital_filter must be either "
                     "enabled (1) or disabled (0). Disabling digital_filter due to incorrect "
                     "parameters."
                  << std::endl;
        LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),
                       direction,
                       channel,
                       0,
                       digital_bandw);
    } else if (digital_bandw <= 0 && digital_filter != 0) {
        std::cout
            << "WARNING: device_handler::set_digital_filter(): digital_bandw must be more than 0."
            << std::endl;
        // close_all_devices();
    } else {
        if (digital_filter == 1)
            LMS_SetGFIRLPF(device_handler::getInstance().get_device(device_number),
                           direction,
                           channel,
                           digital_filter,
                           digital_bandw);

        std::string s_dir[2] = {"RX", "TX"};
        std::cout << "INFO: device_handler::set_digital_filter(): digital filter channel "
                  << channel << " [" << s_dir[direction] << "]: ";
        if (digital_filter)
            std::cout << digital_bandw / 1e6 << " MHz." << std::endl;
        else
            std::cout << "disabled" << std::endl;
    }
}

void device_handler::set_gain(int device_number,
                              bool direction,
                              int channel,
                              unsigned int gain_dB) {
    if ((direction == LMS_CH_RX && gain_dB >= 0 && gain_dB <= 70) ||
        (direction == LMS_CH_TX && gain_dB >= 0 && gain_dB <= 60)) {
        if (LMS_SetGaindB(device_handler::getInstance().get_device(device_number),
                          direction,
                          channel,
                          gain_dB) != LMS_SUCCESS)
            device_handler::error(device_number);

        std::string s_dir[2] = {"RX", "TX"};

        unsigned int gain_value;
        LMS_GetGaindB(device_handler::getInstance().get_device(device_number),
                      direction,
                      channel,
                      &gain_value);
        std::cout << "INFO: device_handler::set_gain(): set gain [" << s_dir[direction] << "] CH"
                  << channel << ": " << gain_value << " dB." << std::endl;
    } else {
        std::cout << "ERROR: device_handler::set_gain(): valid RX gain range [0, 70], TX gain "
                     "range [0, 60]."
                  << std::endl;
        close_all_devices();
    }
}

void device_handler::set_nco(
    int device_number, bool direction, int channel, float nco_freq, float nco_pho) {
    if (nco_freq == 0) {
        LMS_SetNCOIndex(
            device_handler::getInstance().get_device(device_number), direction, channel, -1, 0);
    } else {
        double freq_value_in[16] = {nco_freq};

        int cmix_mode;

        if (nco_freq > 0)
            cmix_mode = 0;
        else if (nco_freq < 0)
            cmix_mode = 1;

        LMS_SetNCOFrequency(device_handler::getInstance().get_device(device_number),
                            direction,
                            channel,
                            freq_value_in,
                            nco_pho);
        LMS_SetNCOIndex(device_handler::getInstance().get_device(device_number),
                        direction,
                        channel,
                        0,
                        cmix_mode);
        std::string s_dir[2] = {"RX", "TX"};
        std::string s_cmix[2] = {"UPCONVERT", "DOWNCONVERT"};
        std::string cmix_mode_string;

        double freq_value_out[16];
        double pho_value_out[16];
        LMS_GetNCOFrequency(device_handler::getInstance().get_device(device_number),
                            direction,
                            channel,
                            freq_value_out,
                            pho_value_out);
        std::cout << "INFO: device_handler::set_nco(): set channel " << channel << " NCO ["
                  << s_dir[direction] << "] CH" << channel << ": " << freq_value_out[0] / 1e6
                  << " MHz (" << pho_value_out[0] << " deg.)(" << s_cmix[cmix_mode] << ")."
                  << std::endl;
    }
}

void device_handler::disable_DC_corrections(int device_number) {
    LMS_WriteParam(device_handler::getInstance().get_device(device_number), LMS7_DC_BYP_RXTSP, 1);
    LMS_WriteParam(device_handler::getInstance().get_device(device_number), LMS7_DCLOOP_STOP, 1);
}
