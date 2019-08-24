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

#ifndef INCLUDED_LIMESDR_SOURCE_IMPL_H
#define INCLUDED_LIMESDR_SOURCE_IMPL_H

#include <limesdr/source.h>

#include "device_handler.h"

namespace gr {
  namespace limesdr {

    class source_impl : public source
    {
      private:
        lms_stream_t streamId[2];

        bool stream_analyzer = false;

        int source_block = 1;

        bool add_tag = false;
        uint32_t pktLoss = 0;

        struct constant_data {
          std::string serial;
          int device_number;
          int channel_mode;
          double samp_rate = 10e6;
          uint32_t FIFO_size = 0;
        } stored;

        std::chrono::high_resolution_clock::time_point t1, t2;

        void print_stream_stats(lms_stream_status_t status);

        void add_time_tag(int channel, lms_stream_meta_t meta);
        // Nothing to declare in this block.

     public:
      source_impl(std::string serial, int channel_mode, const std::string& filename);
      ~source_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      inline gr::io_signature::sptr args_to_io_signature(int channel_mode);

    };

  } // namespace limesdr
} // namespace gr

#endif /* INCLUDED_LIMESDR_SOURCE_IMPL_H */

