/* -*- c++ -*- */
/*
 * Copyright 2019 Lime Microsystems.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "source_impl.h"

namespace gr {
  namespace limesdr {

    source::sptr
    source::make(std::string serial, int channel_mode, const std::string& filename)
    {
      return gnuradio::get_initial_sptr
        (new source_impl(serial, channel_mode, filename));
    }


    /*
     * The private constructor
     */
    source_impl::source_impl(std::string serial, int channel_mode, const std::string& filename)
      : gr::sync_block("source",
              gr::io_signature::make(0, 0, 0),
               args_to_io_signature(channel_mode))
    {
      std::cout << "---------------------------------------------------------------" << std::endl;
      std::cout << "LimeSuite Source (RX) info" << std::endl;
      std::cout << std::endl;

      // 1. Store private variables upon implementation to protect from changing them later
      stored.serial = serial;
      stored.channel_mode = channel_mode;

      if (stored.channel_mode < 0 && stored.channel_mode > 2) {
        std::cout
          << "ERROR: source_impl::source_impl(): Channel must be A(0), B(1) or (A+B) MIMO(2)"
          << std::endl;
        exit(0);
      }

      // 2. Open device if not opened
      stored.device_number = device_handler::getInstance().open_device(stored.serial);
      // 3. Check where to load settings from (file or block)
      if (!filename.empty()) {
        device_handler::getInstance().settings_from_file(stored.device_number, filename, nullptr);
        device_handler::getInstance().check_blocks(
            stored.device_number, source_block, stored.channel_mode, filename);
      } else {
        // 4. Check how many blocks were used and check values between blocks
        device_handler::getInstance().check_blocks(
            stored.device_number, source_block, stored.channel_mode, "");

        // 5. Enable required channel/s
        device_handler::getInstance().enable_channels(
            stored.device_number, stored.channel_mode, LMS_CH_RX);
      }
    }

    /*
     * Our virtual destructor.
     */
    source_impl::~source_impl()
    {
    }

    int
    source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    // Return io_signature to manage module output count
    // based on SISO (one output) and MIMO (two outputs) modes
    inline gr::io_signature::sptr source_impl::args_to_io_signature(int channel_number) {
      if (channel_number < 2) {
        return gr::io_signature::make(1, 1, sizeof(gr_complex));
      } else if (channel_number == 2) {
        return gr::io_signature::make(2, 2, sizeof(gr_complex));
      } else {
        std::cout << "ERROR: source_impl::args_to_io_signature(): channel_number must be 0,1 or 2."
          << std::endl;
        exit(0);
      }
    }

  } /* namespace limesdr */
} /* namespace gr */

