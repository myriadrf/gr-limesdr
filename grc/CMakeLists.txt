# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# This software is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

install(FILES
    limesdr_source.block.yml
    limesdr_sink.block.yml
    DESTINATION share/gnuradio/grc/blocks
)
if(ENABLE_RFE)
    install(FILES limesdr_rfe.block.yml DESTINATION share/gnuradio/grc/blocks)
endif()