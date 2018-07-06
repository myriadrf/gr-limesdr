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

#include "source_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace limesdr {
source::sptr source::make(std::string serial,
                          int device_type,
                          int chip_mode,
                          int channel,
                          int file_switch,
                          const char* filename,
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
                          int gain_dB_ch1,
                          float nco_freq_ch0,
                          float nco_freq_ch1,
                          int cmix_mode_ch0,
                          int cmix_mode_ch1) {
    return gnuradio::get_initial_sptr(new source_impl(serial,
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
                                                      lna_path_mini,
                                                      lna_path_ch0,
                                                      lna_path_ch1,
                                                      analog_filter_ch0,
                                                      analog_bandw_ch0,
                                                      analog_filter_ch1,
                                                      analog_bandw_ch1,
                                                      digital_filter_ch0,
                                                      digital_bandw_ch0,
                                                      digital_filter_ch1,
                                                      digital_bandw_ch1,
                                                      gain_dB_ch0,
                                                      gain_dB_ch1,
                                                      nco_freq_ch0,
                                                      nco_freq_ch1,
                                                      cmix_mode_ch0,
                                                      cmix_mode_ch1));
}

source_impl::source_impl(std::string serial,
                         int device_type,
                         int chip_mode,
                         int channel,
                         int file_switch,
                         const char* filename,
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
                         int gain_dB_ch1,
                         float nco_freq_ch0,
                         float nco_freq_ch1,
                         int cmix_mode_ch0,
                         int cmix_mode_ch1)
    : gr::block("source",
                gr::io_signature::make(
                    0, 0, 0), // Based on chip_mode SISO/MIMO use appropriate output signature
                args_to_io_signature(chip_mode)) {
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << "LimeSuite Source (RX) info" << std::endl;
    std::cout << std::endl;

    // 1. Store private variables upon implementation to protect from changing them later
    stored.serial = serial;
    stored.device_type = device_type;
    stored.chip_mode = chip_mode;
    stored.samp_rate = samp_rate;

    if (stored.device_type == 1) // LimeSDR-Mini
        stored.channel = LMS_CH_0;
    else if (stored.device_type == 2) // LimeSDR-USB
    {
        if (stored.chip_mode == 1) // If SISO configure prefered channel
            stored.channel = channel;
        else if (stored.chip_mode == 2) // If MIMO begin configuring channel 0
            stored.channel = LMS_CH_0;
    }

    if (channel != 0 && channel != 1) {
        std::cout << "ERROR: source_impl::source_impl(): CH0 must be either 0 (A) or 1 (B)."
                  << std::endl;
        exit(0);
    }

    // 2. Open device if not opened
    stored.device_number =
        device_handler::getInstance().open_device(stored.serial, stored.device_type);

    // 3. Check where to load settings from (file or block)
    if (file_switch == 1) {
        device_handler::getInstance().settings_from_file(stored.device_number, filename);
        device_handler::getInstance().check_blocks(stored.device_number,
                                                   source_block,
                                                   stored.device_type,
                                                   stored.chip_mode,
                                                   0,
                                                   0,
                                                   file_switch,
                                                   filename);
    } else {
        // 4. Check how many blocks were used and check values between blocks
        device_handler::getInstance().check_blocks(stored.device_number,
                                                   source_block,
                                                   stored.device_type,
                                                   stored.chip_mode,
                                                   samp_rate,
                                                   oversample,
                                                   file_switch,
                                                   NULL);

        // 5. Set SISO/MIMO mode
        device_handler::getInstance().set_chip_mode(
            stored.device_number, stored.device_type, stored.chip_mode, stored.channel, LMS_CH_RX);

        // 6. Set RF frequency
        device_handler::getInstance().set_rf_freq(
            stored.device_number, stored.device_type, LMS_CH_RX, stored.channel, rf_freq);


        // 7. Set sample rate
        if (stored.device_type == 1) // LimeSDR-Mini can only have the same rates
            device_handler::getInstance().set_samp_rate(
                stored.device_number, samp_rate, oversample);
        else if (stored.device_type == 2) // LimeSDR-USB can have separate rates for TX and RX
            device_handler::getInstance().set_samp_rate_dir(
                stored.device_number, LMS_CH_RX, samp_rate, oversample);

        // 8. Configure analog and digital filters
        device_handler::getInstance().set_analog_filter(
            stored.device_number, LMS_CH_RX, stored.channel, analog_filter_ch0, analog_bandw_ch0);
        device_handler::getInstance().set_digital_filter(
            stored.device_number, LMS_CH_RX, stored.channel, digital_filter_ch0, digital_bandw_ch0);


        // 9. Set LNA path
        if (stored.device_type == 1) // LimeSDR-Mini
            device_handler::getInstance().set_antenna(
                stored.device_number, stored.channel, LMS_CH_RX, lna_path_mini);
        else if (stored.device_type == 2) // LimeSDR-USB
            device_handler::getInstance().set_antenna(
                stored.device_number, stored.channel, LMS_CH_RX, lna_path_ch0);

        // 10. Set GAIN
        device_handler::getInstance().set_gain(
            stored.device_number, LMS_CH_RX, stored.channel, gain_dB_ch0);

        // 11. Perform calibration
        device_handler::getInstance().calibrate(stored.device_number,
                                                stored.device_type,
                                                calibration_ch0,
                                                LMS_CH_RX,
                                                stored.channel,
                                                calibr_bandw_ch0,
                                                rf_freq,
                                                lna_path_ch0);

        // 12. Set NCO
        device_handler::getInstance().set_nco(
            stored.device_number, LMS_CH_RX, stored.channel, nco_freq_ch0, 0, cmix_mode_ch0);

        // 13. Begin configuring device for channel 1 (if chip_mode is MIMO)
        if (stored.chip_mode == 2 && stored.device_type == 2) {
            device_handler::getInstance().set_analog_filter(
                stored.device_number, LMS_CH_RX, LMS_CH_1, analog_filter_ch1, analog_bandw_ch1);
            device_handler::getInstance().set_digital_filter(
                stored.device_number, LMS_CH_RX, LMS_CH_1, digital_filter_ch1, digital_bandw_ch1);
            device_handler::getInstance().set_antenna(
                stored.device_number, LMS_CH_1, LMS_CH_RX, lna_path_ch1);
            device_handler::getInstance().set_gain(
                stored.device_number, LMS_CH_RX, LMS_CH_1, gain_dB_ch1);
            device_handler::getInstance().calibrate(stored.device_number,
                                                    stored.device_type,
                                                    calibration_ch1,
                                                    LMS_CH_RX,
                                                    LMS_CH_1,
                                                    calibr_bandw_ch1,
                                                    rf_freq,
                                                    lna_path_ch1);
            device_handler::getInstance().set_nco(
                stored.device_number, LMS_CH_RX, LMS_CH_1, nco_freq_ch1, 0, cmix_mode_ch1);
        }
    }
    // 14. Initialize stream for channel 0 (if chip_mode is SISO)
    if (stored.chip_mode == 1) {
        this->init_stream(stored.device_number, stored.channel, stored.samp_rate);
    }

    // 15. Initialize both channels streams (if chip_mode is MIMO)
    else if (stored.chip_mode == 2 && stored.device_type == 2) {
        this->init_stream(stored.device_number, LMS_CH_0, stored.samp_rate);
        this->init_stream(stored.device_number, LMS_CH_1, stored.samp_rate);
    }
    std::cout << "---------------------------------------------------------------" << std::endl;
}

source_impl::~source_impl() {
    // Stop and destroy stream for channel 0 (if chip_mode is SISO)
    if (stored.chip_mode == 1) {
        LMS_StopStream(&streamId[stored.channel]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[stored.channel]);
    }
    // Stop and destroy stream for channels 0 & 1 (if chip_mode is MIMO)
    else if (stored.chip_mode == 2) {
        LMS_StopStream(&streamId[LMS_CH_0]);
        LMS_StopStream(&streamId[LMS_CH_1]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[LMS_CH_0]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[LMS_CH_1]);
    }
    device_handler::getInstance().close_device(stored.device_number, source_block);
}

bool source_impl::start(void) {
    std::unique_lock<std::recursive_mutex> lock(device_handler::getInstance().block_mutex);
    // Initialize and start stream for channel 0 (if chip_mode is SISO)
    if (stored.chip_mode == 1) // If SISO configure prefered channel
    {
        if (LMS_StartStream(&streamId[stored.channel]) != LMS_SUCCESS)
            device_handler::getInstance().error(stored.device_number);
    }

    // Initialize and start stream for channels 0 & 1 (if chip_mode is MIMO)
    else if (stored.chip_mode == 2 && stored.device_type == 2) {
        if (LMS_StartStream(&streamId[LMS_CH_0]) != LMS_SUCCESS)
            device_handler::getInstance().error(stored.device_number);
        if (LMS_StartStream(&streamId[LMS_CH_1]) != LMS_SUCCESS)
            device_handler::getInstance().error(stored.device_number);
    }
    std::unique_lock<std::recursive_mutex> unlock(device_handler::getInstance().block_mutex);
    return true;
}

bool source_impl::stop(void) {
    std::unique_lock<std::recursive_mutex> lock(device_handler::getInstance().block_mutex);
    // Stop stream for channel 0 (if chip_mode is SISO)
    if (stored.chip_mode == 1) {
        LMS_StopStream(&streamId[stored.channel]);
    }
    // Stop streams for channels 0 & 1 (if chip_mode is MIMO)
    else if (stored.chip_mode == 2) {
        LMS_StopStream(&streamId[LMS_CH_0]);
        LMS_StopStream(&streamId[LMS_CH_1]);
    }
    std::unique_lock<std::recursive_mutex> unlock(device_handler::getInstance().block_mutex);
    return true;
}

int source_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items) {
    // Receive stream for channel 0 (if chip_mode is SISO)
    if (stored.chip_mode == 1) {
        // Print stream stats to debug
        if (stream_analyzer == true && std::time(0) > t) {
            lms_stream_status_t status;
            LMS_GetStreamStatus(&streamId[stored.channel], &status);
            std::cout << std::endl;
            std::cout << "RX rate: " << status.linkRate / 1e6 << " MB/s ";
            std::cout << "dropped packets: " << status.droppedPackets << " ";
            std::cout << "FIFO: " << 100 * status.fifoFilledCount / status.fifoSize << "% ";
            std::cout << "underrun: " << status.underrun << " overrun: " << status.overrun;
            std::cout << " FIFO size: " << status.fifoSize << std::endl;
            t = std::time(0);
        }

        int ret0 =
            LMS_RecvStream(&streamId[stored.channel], output_items[0], noutput_items, NULL, 100);
        if (ret0 < 0) {
            return 0;
        }
        produce(0, ret0);
        return WORK_CALLED_PRODUCE;
    }
    // Receive stream for channels 0 & 1 (if chip_mode is MIMO)
    else if (stored.chip_mode == 2) {
        // Print stream stats to debug
        if (stream_analyzer == true && std::time(0) > t) {
            lms_stream_status_t status;
            LMS_GetStreamStatus(&streamId[LMS_CH_0], &status);
            std::cout << std::endl;
            std::cout << "RX rate: " << status.linkRate / 1e6 << " MB/s ";
            std::cout << "dropped packets: " << status.droppedPackets << " ";
            std::cout << "FIFO: " << 100 * status.fifoFilledCount / status.fifoSize << "% ";
            std::cout << "underrun: " << status.underrun << " overrun: " << status.overrun
                      << std::endl;
            t = std::time(0);
        }

        int ret0 = LMS_RecvStream(&streamId[LMS_CH_0], output_items[0], noutput_items, NULL, 100);
        int ret1 = LMS_RecvStream(&streamId[LMS_CH_1], output_items[1], noutput_items, NULL, 100);
        if (ret0 <= 0 || ret1 <= 0) {
            return 0;
        }
        this->produce(0, ret0);
        this->produce(1, ret1);
        return WORK_CALLED_PRODUCE;
    }
    return 0;
}

// Setup stream
void source_impl::init_stream(int device_number, int channel, float samp_rate) {
    streamId[channel].channel = channel;
    streamId[channel].fifoSize = samp_rate / 1e4;
    streamId[channel].throughputVsLatency = 0.5;
    streamId[channel].isTx = LMS_CH_RX;
    streamId[channel].dataFmt = lms_stream_t::LMS_FMT_F32;

    if (LMS_SetupStream(device_handler::getInstance().get_device(stored.device_number),
                        &streamId[channel]) != LMS_SUCCESS)
        device_handler::getInstance().error(stored.device_number);

    std::cout << "INFO: source_impl::init_stream(): source channel " << channel << " (device nr. "
              << device_number << ") stream setup done." << std::endl;
}

// Return io_signature to manage module output count
// based on SISO (one output) and MIMO (two outputs) modes
inline gr::io_signature::sptr source_impl::args_to_io_signature(int channel_number) {
    if (channel_number == 1) {
        return gr::io_signature::make(1, 1, sizeof(gr_complex));
    } else if (channel_number == 2) {
        return gr::io_signature::make(2, 2, sizeof(gr_complex));
    } else {
        std::cout << "ERROR: source_impl::args_to_io_signature(): channel_number must be 0 or 1."
                  << std::endl;
        exit(0);
    }
}
void source_impl::set_rf_freq(float rf_freq) {
    device_handler::getInstance().set_rf_freq(
        stored.device_number, stored.device_type, LMS_CH_RX, LMS_CH_0, rf_freq);
}

void source_impl::set_nco(float nco_freq, int cmix_mode, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "source_impl::set_nco(): INFO: Setting bypassed. LimeSDR-Mini does not "
                     "support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_nco(
            stored.device_number, LMS_CH_RX, channel, nco_freq, 0, cmix_mode);
    }
}

void source_impl::set_lna_path(int lna_path, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "source_impl::set_lna_path(): INFO: Setting bypassed. LimeSDR-Mini does not "
                     "support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_antenna(
            stored.device_number, channel, LMS_CH_RX, lna_path);
    }
}

void source_impl::set_analog_filter(int analog_filter, float analog_bandw, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "source_impl::set_analog_filter(): INFO: Setting bypassed. LimeSDR-Mini does "
                     "not support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_analog_filter(
            stored.device_number, LMS_CH_RX, channel, analog_filter, analog_bandw);
    }
}

void source_impl::set_digital_filter(int digital_filter, float digital_bandw, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "source_impl::set_digital_filter(): INFO: Setting bypassed. LimeSDR-Mini does "
                     "not support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_digital_filter(
            stored.device_number, LMS_CH_RX, channel, digital_filter, digital_bandw);
    }
}

void source_impl::set_gain(int gain_dB, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "source_impl::set_gain(): INFO: Setting bypassed. LimeSDR-Mini does not "
                     "support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_gain(stored.device_number, LMS_CH_RX, channel, gain_dB);
    }
}
} // namespace limesdr
} // namespace gr
