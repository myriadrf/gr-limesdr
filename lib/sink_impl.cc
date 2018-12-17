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

#include "sink_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace limesdr {
sink::sptr sink::make(std::string serial,
                      int device_type,
                      int channel_mode,
                      int file_switch,
                      const char* filename,
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
                      int gain_dB_ch1,
                      float nco_freq_ch0,
                      float nco_freq_ch1,
                      const std::string& length_tag_name) {
    return gnuradio::get_initial_sptr(new sink_impl(serial,
                                                    device_type,
                                                    channel_mode,
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
                                                    gain_dB_ch1,
                                                    nco_freq_ch0,
                                                    nco_freq_ch1,
                                                    length_tag_name));
}

sink_impl::sink_impl(std::string serial,
                     int device_type,
                     int channel_mode,
                     int file_switch,
                     const char* filename,
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
                     int gain_dB_ch1,
                     float nco_freq_ch0,
                     float nco_freq_ch1,
                     const std::string& length_tag_name)
    : gr::block("sink",
                args_to_io_signature(
                    channel_mode,
                    device_type), // Based on channel_mode SISO/MIMO use appropriate input signature
                gr::io_signature::make(0, 0, 0)) {
    std::cout << "---------------------------------------------------------------" << std::endl;
    std::cout << "LimeSuite Sink (TX) info" << std::endl;
    std::cout << std::endl;

    LENGTH_TAG = length_tag_name.empty() ? pmt::PMT_NIL : pmt::string_to_symbol(length_tag_name);
    // 1. Store private variables upon implementation to protect from changing them later
    stored.serial = serial;
    stored.device_type = device_type;
    stored.channel_mode = channel_mode;
    stored.samp_rate = samp_rate;

    if (stored.device_type == 1) // LimeSDR-Mini
    {
        stored.channel = LMS_CH_0;
        stored.channel_mode = 1;
    } else if (stored.device_type == 2) // LimeSDR-USB
    {
        if (stored.channel_mode < 3)                  // If SISO configure prefered channel
            stored.channel = stored.channel_mode - 1; // Channel is chip mode - 1
        else if (stored.channel_mode == 3)            // If MIMO begin configuring channel 0
            stored.channel = LMS_CH_0;
    }

    if (stored.channel_mode < 1 && stored.channel > 3) {
        std::cout << "ERROR: sink_impl::sink_impl(): Channel must be A(1), B(2) or (A+B) MIMO(3)"
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
                                                   sink_block,
                                                   stored.device_type,
                                                   stored.channel_mode,
                                                   0,
                                                   0,
                                                   file_switch,
                                                   filename);
    } else {
        // 4. Check how many blocks were used and check values between blocks
        device_handler::getInstance().check_blocks(stored.device_number,
                                                   sink_block,
                                                   stored.device_type,
                                                   stored.channel_mode,
                                                   samp_rate,
                                                   oversample,
                                                   file_switch,
                                                   NULL);

        // 5. Set SISO/MIMO mode
        device_handler::getInstance().set_chip_mode(stored.device_number,
                                                    stored.device_type,
                                                    stored.channel_mode,
                                                    stored.channel,
                                                    LMS_CH_TX);

        // 6. Set RF frequency
        device_handler::getInstance().set_rf_freq(
            stored.device_number, stored.device_type, LMS_CH_TX, stored.channel, rf_freq);

        // 7. Set sample rate
        if (stored.device_type == 1) // LimeSDR-Mini can only have the same rates
            device_handler::getInstance().set_samp_rate(
                stored.device_number, samp_rate, oversample);
        else if (stored.device_type == 2) // LimeSDR-USB can have separate rates for TX and RX
            device_handler::getInstance().set_samp_rate_dir(
                stored.device_number, LMS_CH_TX, samp_rate, oversample);

        // 8. Configure analog and digital filters
        device_handler::getInstance().set_analog_filter(
            stored.device_number, LMS_CH_TX, stored.channel, analog_filter_ch0, analog_bandw_ch0);
        device_handler::getInstance().set_digital_filter(
            stored.device_number, LMS_CH_TX, stored.channel, digital_filter_ch0, digital_bandw_ch0);


        // 9. Set PA path
        if (stored.device_type == 1) // LimeSDR-Mini
            device_handler::getInstance().set_antenna(
                stored.device_number, stored.channel, LMS_CH_TX, pa_path_mini);
        else if (stored.device_type == 2) // LimeSDR-USB
            device_handler::getInstance().set_antenna(
                stored.device_number, stored.channel, LMS_CH_TX, pa_path_ch0);

        // 10. Set GAIN
        device_handler::getInstance().set_gain(
            stored.device_number, LMS_CH_TX, stored.channel, gain_dB_ch0);

        // 11. Perform calibration
        device_handler::getInstance().calibrate(stored.device_number,
                                                stored.device_type,
                                                calibration_ch0,
                                                LMS_CH_TX,
                                                stored.channel,
                                                calibr_bandw_ch0,
                                                rf_freq,
                                                pa_path_ch0);

        // 12. Set NCO
        device_handler::getInstance().set_nco(
            stored.device_number, LMS_CH_TX, stored.channel, nco_freq_ch0, 0);

        // 13. Begin configuring device for channel 1 (if channel_mode is MIMO)
        if (stored.channel_mode == 3 && stored.device_type == 2) {
            device_handler::getInstance().set_antenna(
                stored.device_number, LMS_CH_1, LMS_CH_TX, pa_path_ch1);
            device_handler::getInstance().set_analog_filter(
                stored.device_number, LMS_CH_TX, LMS_CH_1, analog_filter_ch1, analog_bandw_ch1);
            device_handler::getInstance().set_digital_filter(
                stored.device_number, LMS_CH_TX, LMS_CH_1, digital_filter_ch1, digital_bandw_ch1);
            device_handler::getInstance().set_gain(
                stored.device_number, LMS_CH_TX, LMS_CH_1, gain_dB_ch1);
            device_handler::getInstance().calibrate(stored.device_number,
                                                    stored.device_type,
                                                    calibration_ch1,
                                                    LMS_CH_TX,
                                                    LMS_CH_1,
                                                    calibr_bandw_ch1,
                                                    rf_freq,
                                                    pa_path_ch1);
            device_handler::getInstance().set_nco(
                stored.device_number, LMS_CH_TX, LMS_CH_1, nco_freq_ch1, 0);
        }
    }
    // device_handler::getInstance().debug_packets(stored.device_number);
    // 14. Initialize stream for channel 0 (if channel_mode is SISO)
    if (stored.channel_mode < 3) {
        this->init_stream(stored.device_number, stored.channel, stored.samp_rate);
    }

    // 15. Initialize both channels streams (if channel_mode is MIMO)
    else if (stored.channel_mode == 3 && stored.device_type == 2) {
        this->init_stream(stored.device_number, LMS_CH_0, stored.samp_rate);
        this->init_stream(stored.device_number, LMS_CH_1, stored.samp_rate);
    }
    std::cout << "---------------------------------------------------------------" << std::endl;
}

sink_impl::~sink_impl() {
    // Stop and destroy stream for channel 0 (if channel_mode is SISO)
    if (stored.channel_mode < 3) {
        LMS_StopStream(&streamId[stored.channel]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[stored.channel]);
    }
    // Stop and destroy stream for channels 0 & 1 (if channel_mode is MIMO)
    else if (stored.channel_mode == 3) {
        LMS_StopStream(&streamId[LMS_CH_0]);
        LMS_StopStream(&streamId[LMS_CH_1]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[LMS_CH_0]);
        LMS_DestroyStream(device_handler::getInstance().get_device(stored.device_number),
                          &streamId[LMS_CH_1]);
    }
    device_handler::getInstance().close_device(stored.device_number, sink_block);
}

bool sink_impl::start(void) {
    std::unique_lock<std::recursive_mutex> lock(device_handler::getInstance().block_mutex);
    // Init timestamp
    tx_meta.timestamp = 0;

    if (stream_analyzer) {
        t1 = std::chrono::high_resolution_clock::now();
        t2 = t1;
    }
    // Initialize and start stream for channel 0 (if channel_mode is SISO)
    if (stored.channel_mode < 3) // If SISO configure prefered channel
    {
        LMS_StartStream(&streamId[stored.channel]);
    }
    // Initialize and start stream for channels 0 & 1 (if channel_mode is MIMO)
    else if (stored.channel_mode == 3 && stored.device_type == 2) {
        LMS_StartStream(&streamId[LMS_CH_0]);
        LMS_StartStream(&streamId[LMS_CH_1]);
    }
    std::unique_lock<std::recursive_mutex> unlock(device_handler::getInstance().block_mutex);
    return true;
}

bool sink_impl::stop(void) {
    std::unique_lock<std::recursive_mutex> lock(device_handler::getInstance().block_mutex);
    // Stop stream for channel 0 (if channel_mode is SISO)
    if (stored.channel_mode < 3) {
        LMS_StopStream(&streamId[stored.channel]);
    }
    // Stop streams for channels 0 & 1 (if channel_mode is MIMO)
    else if (stored.channel_mode == 3) {
        LMS_StopStream(&streamId[LMS_CH_0]);
        LMS_StopStream(&streamId[LMS_CH_1]);
    }
    std::unique_lock<std::recursive_mutex> unlock(device_handler::getInstance().block_mutex);
    return true;
}

int sink_impl::general_work(int noutput_items,
                            gr_vector_int& ninput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items) {
    // std::cout << "Got to work" << std::endl;
    // Init number of items to be sent and timestamps
    nitems_send = noutput_items;
    int current_sample = nitems_read(0);
    tx_meta.waitForTimestamp = false;
    tx_meta.flushPartialPacket = false;

    // Check if channel 0 has any tags
    this->work_tags(noutput_items);

    // If length tag has been found burst_length should be higher than 0
    if (burst_length > 0) {
        nitems_send = std::min<long>(burst_length, nitems_send);
        // Check if it is the end of the burst
        if (burst_length - (long)nitems_send == 0) {
            tx_meta.flushPartialPacket = true;
        }
    }

    // Send stream for channel 0 (if channel_mode is SISO)
    if (stored.channel_mode < 3) {
        // Print stream stats to debug
        if (stream_analyzer == true) {
            this->print_stream_stats(stored.channel);
        }
        // std::cout << "Timestamp " << tx_meta[0].timestamp  << std::endl;
        // std::cout << "Nitems: " << nitems_send << std::endl;
        ret[0] =
            LMS_SendStream(&streamId[stored.channel], input_items[0], nitems_send, &tx_meta, 100);
        if (ret[0] < 0) {
            return 0;
        } // else
          // std::cout << "Sent: " << ret[0] << std::endl;
        burst_length -= ret[0];
        tx_meta.timestamp += ret[0];
        consume(0, ret[0]);
    }
    // Send stream for channels 0 & 1 (if channel_mode is MIMO)
    else if (stored.channel_mode == 3) {
        // Print stream stats to debug
        if (stream_analyzer == true) {
            this->print_stream_stats(LMS_CH_0);
        }
        // Send data
        ret[0] = LMS_SendStream(&streamId[LMS_CH_0], input_items[0], nitems_send, &tx_meta, 100);
        ret[1] = LMS_SendStream(&streamId[LMS_CH_1], input_items[1], nitems_send, &tx_meta, 100);
        if (ret[0] < 0 || ret[1] < 0) {
            return 0;
        }

        burst_length -= ret[0];
        tx_meta.timestamp += ret[0];
        consume(0, ret[0]);
        consume(1, ret[1]);
    }
    return 0;
}
void sink_impl::work_tags(int noutput_items) {

    static uint64_t last_timestamp = 0;
    std::vector<tag_t> tags;
    int current_sample = nitems_read(0);
    get_tags_in_range(tags, 0, current_sample, current_sample + noutput_items);

    if (!tags.empty()) {
        std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
        // Go through the tags
        for (tag_t cTag : tags) {
            // Found tx_time tag
            if (pmt::eq(cTag.key, TIME_TAG)) {
                // Convert time to sample timestamp
                uint64_t secs = pmt::to_uint64(pmt::tuple_ref(cTag.value, 0));
                double fracs = pmt::to_double(pmt::tuple_ref(cTag.value, 1));
                uint64_t u_rate = (uint64_t)stored.samp_rate;
                double f_rate = stored.samp_rate - u_rate;
                uint64_t timestamp =
                    u_rate * secs + llround(secs * f_rate + fracs * stored.samp_rate);

                if (cTag.offset == current_sample) {
                    if (timestamp - last_timestamp < 1020 && timestamp - last_timestamp != 0) {
                        std::cout << "ERROR: Timestamps must be at least 1020 samples apart"
                                  << std::endl;
                        continue;
                    }
                    tx_meta.waitForTimestamp = true;
                    tx_meta.timestamp = timestamp;
                    last_timestamp = timestamp;
                } else {
                    nitems_send = cTag.offset - current_sample;
                    break;
                }
            }
            // Found length tag
            else if (!pmt::is_null(LENGTH_TAG) && pmt::eq(cTag.key, LENGTH_TAG)) {
                if (cTag.offset == current_sample) {
                    // Found length tag in the middle of the burst
                    if (burst_length > 0 && ret[0] > 0)
                        std::cout << "INFO: Length tag has been preemted" << std::endl;
                    burst_length = pmt::to_long(cTag.value);
                } else {
                    nitems_send = cTag.offset - current_sample;
                    break;
                }
            }
        }
    }
}
// Print stream status
void sink_impl::print_stream_stats(int channel) {
    t2 = std::chrono::high_resolution_clock::now();
    auto timePeriod = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    if (timePeriod >= 1000) {
        lms_stream_status_t status;
        LMS_GetStreamStatus(&streamId[channel], &status);
        std::cout << std::endl;
        std::cout << "TX";
        std::cout << "|rate: " << status.linkRate / 1e6 << " MB/s ";
        std::cout << "|dropped packets: " << status.droppedPackets << " ";
        std::cout << "|FIFO: " << 100 * status.fifoFilledCount / status.fifoSize << "%"
                  << std::endl;
        t1 = t2;
    }
}
// Setup stream
void sink_impl::init_stream(int device_number, int channel, float samp_rate) {
    streamId[channel].channel = channel;
    streamId[channel].fifoSize = int(samp_rate) / 1e4;
    streamId[channel].throughputVsLatency = 0.5;
    streamId[channel].isTx = LMS_CH_TX;
    streamId[channel].dataFmt = lms_stream_t::LMS_FMT_F32;

    if (LMS_SetupStream(device_handler::getInstance().get_device(device_number),
                        &streamId[channel]) != LMS_SUCCESS)
        device_handler::getInstance().error(device_number);

    std::cout << "INFO: sink_impl::init_stream(): sink channel " << channel << " (device nr. "
              << device_number << ") stream setup done." << std::endl;
}

// Return io_signature to manage module input count
// based on SISO (one input) and MIMO (two inputs) modes
inline gr::io_signature::sptr sink_impl::args_to_io_signature(int channel_number, int device_type) {
    if (channel_number < 3 || device_type == 1) {
        return gr::io_signature::make(1, 1, sizeof(gr_complex));
    } else if (channel_number == 3 && device_type == 2) {
        return gr::io_signature::make(2, 2, sizeof(gr_complex));
    } else {
        std::cout << "ERROR: sink_impl::args_to_io_signature(): channel_number must be 1,2 or 3."
                  << std::endl;
        exit(0);
    }
}

void sink_impl::set_rf_freq(float rf_freq) {
    device_handler::getInstance().set_rf_freq(
        stored.device_number, stored.device_type, LMS_CH_TX, LMS_CH_0, rf_freq);
}

void sink_impl::set_pa_path(int pa_path, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "sink_impl::set_pa_path(): INFO: Setting bypassed. LimeSDR-Mini does not "
                     "support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_antenna(
            stored.device_number, channel, LMS_CH_TX, pa_path);
    }
}

void sink_impl::set_nco(float nco_freq, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "sink_impl::set_nco(): INFO: Setting bypassed. LimeSDR-Mini does not support "
                     "channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_nco(
            stored.device_number, LMS_CH_TX, channel, nco_freq, 0);
    }
}

void sink_impl::set_analog_filter(int analog_filter, float analog_bandw, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "sink_impl::set_analog_filter(): INFO: Setting bypassed. LimeSDR-Mini does "
                     "not support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_analog_filter(
            stored.device_number, LMS_CH_TX, channel, analog_filter, analog_bandw);
    }
}

void sink_impl::set_digital_filter(int digital_filter, float digital_bandw, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "sink_impl::set_digital_filter(): INFO: Setting bypassed. LimeSDR-Mini does "
                     "not support channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_digital_filter(
            stored.device_number, LMS_CH_TX, channel, digital_filter, digital_bandw);
    }
}

void sink_impl::set_gain(int gain_dB, int channel) {
    if (stored.device_type == 1 && channel == 1) {
        // IGNORE CHANNEL 1 FOR LIMESDR-MINI
        std::cout << "sink_impl::set_gain(): INFO: Setting bypassed. LimeSDR-Mini does not support "
                     "channel 1 configuration."
                  << std::endl;
    } else {
        device_handler::getInstance().set_gain(stored.device_number, LMS_CH_TX, channel, gain_dB);
    }
}
} // namespace limesdr
} // namespace gr
