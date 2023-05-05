/* -*- c++ -*- */
/*
 * Copyright 2020 Lime Microsystems.
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

#include "device_handler.h"
#include "logging.h"
#include <limesdr/rfe.h>

#include <boost/format.hpp>

#include <stdexcept>

namespace gr {
namespace limesdr {
rfe::rfe(int comm_type,
         std::string device,
         std::string config_file,
         char IDRX,
         char IDTX,
         char PortRX,
         char PortTX,
         char Mode,
         char Notch,
         char Atten)
{
    set_limesuite_logger();

    gr::configure_default_loggers(
        d_logger,
        d_debug_logger,
        str(boost::format("LimeRFE %s") % device));

    GR_LOG_INFO(d_logger, "---------------------------------------------------------------");
    GR_LOG_INFO(d_logger, "LimeSuite RFE Info");

    boardState.channelIDRX = IDRX;
    boardState.channelIDTX = IDTX;
    boardState.selPortRX = PortRX;
    boardState.selPortTX = PortTX;
    boardState.mode = Mode;
    boardState.notchOnOff = Notch;
    boardState.attValue = Atten;

    if (comm_type) // SDR GPIO communication
    {
        sdr_device_num = device_handler::getInstance().open_device(device);

        GR_LOG_INFO(d_logger, "Opening through GPIO communication.");
        rfe_dev =
            RFE_Open(nullptr, device_handler::getInstance().get_device(sdr_device_num));
        if (!rfe_dev) {
            throw std::runtime_error("LimeRFE: failed to open device");
        }

        // No need to set up this if it isn't automatic
        if (boardState.channelIDRX == RFE_CID_AUTO ||
            boardState.channelIDTX == RFE_CID_AUTO) {
            device_handler::getInstance().set_rfe_device(rfe_dev);

            // Update the channels since the SDR could already be set up and working
            device_handler::getInstance().update_rfe_channels();
        }
    } else // Direct USB
    {
        // Not using device handler so print the version
        GR_LOG_INFO(d_logger, "##################");
        GR_LOG_INFO(d_logger, boost::format("LimeSuite version: %s") % LMS_GetLibraryVersion());
        GR_LOG_INFO(d_logger, boost::format("gr-limesdr version: %s") % GR_LIMESDR_VER);
        GR_LOG_INFO(d_logger, "##################");

        GR_LOG_INFO(d_logger, boost::format("Opening %s") % device);
        rfe_dev = RFE_Open(device.c_str(), nullptr);
        if (!rfe_dev) {
            throw std::runtime_error("LimeRFE: failed to open " + device);
        }
    }

    int error = 0;
    unsigned char info[4] = { 0 };
    if ((error = RFE_GetInfo(rfe_dev, info)) != 0) {
        throw std::runtime_error("LimeRFE: failed to get device info.");
    }
    GR_LOG_INFO(d_logger, boost::format("FW: %d HW: %d") % (int)info[0] % (int)info[1]);

    if (config_file.empty()) {
        if ((error = RFE_ConfigureState(rfe_dev, boardState)) != 0) {
            throw std::runtime_error("LimeRFE: failed to configure device.");
        }
    } else {
        GR_LOG_INFO(d_logger, "Loading configuration file");
        if ((error = RFE_LoadConfig(rfe_dev, config_file.c_str())) != 0) {
            throw std::runtime_error("LimeRFE: failed to load configuration file.");
        }
    }
    GR_LOG_INFO(d_logger, "Board state:");
    get_board_state();
    GR_LOG_INFO(d_logger, "---------------------------------------------------------------");
}

rfe::~rfe()
{
    GR_LOG_INFO(d_logger, "Closing");
    if (rfe_dev) {
        RFE_Reset(rfe_dev);
        RFE_Close(rfe_dev);
    }
}

int rfe::change_mode(int mode)
{
    if (rfe_dev) {
        if (mode == RFE_MODE_TXRX) {
            if (boardState.selPortRX == boardState.selPortTX &&
                boardState.channelIDRX < RFE_CID_CELL_BAND01) {
                GR_LOG_ERROR(
                    d_logger,
                    "Mode cannot be set to RX+TX when same port is selected");
                return -1;
            }
        }
        int error = 0;
        if (mode > 3 || mode < 0) {
            GR_LOG_ERROR(d_logger, boost::format("Invalid mode %d") % mode);
            return -1;
        }
        std::string mode_str[4] = { "RX", "TX", "NONE", "RX+TX" };
        GR_LOG_INFO(d_logger, boost::format("Changing mode to %s") % mode_str);

        if ((error = RFE_Mode(rfe_dev, mode)) != 0) {
            GR_LOG_ERROR(d_logger, boost::format("Failed to change mode: %s") % this->strerror(error));
        }
        boardState.mode = mode;
        return error;
    }
    GR_LOG_ERROR(d_logger, "No RFE device opened");
    return -1;
}

int rfe::set_fan(int enable)
{
    if (rfe_dev) {
        std::string enable_str[2] = { "Disabling", "Enabling" };
        GR_LOG_INFO(d_logger, boost::format("%s fan") % enable_str[enable]);
        int error = 0;
        if ((error = RFE_Fan(rfe_dev, enable)) != 0) {
            GR_LOG_ERROR(d_logger, boost::format("Failed to change mode: %s") % this->strerror(error));
        }
        return error;
    }
    GR_LOG_ERROR(d_logger, "No RFE device opened");
    return -1;
}

int rfe::set_attenuation(int attenuation)
{
    if (rfe_dev) {
        int error = 0;
        if (attenuation > 7) {
            GR_LOG_ERROR(d_logger, "Attenuation value too high, valid range [0, 7]");
            return -1;
        }
        GR_LOG_INFO(d_logger, boost::format("Changing attenuation value to: %d") % attenuation);

        boardState.attValue = attenuation;
        if ((error = RFE_ConfigureState(rfe_dev, boardState)) != 0) {
            GR_LOG_ERROR(d_logger, boost::format("Failed to change attenuation: %s") % this->strerror(error));
        }
        return error;
    }
    GR_LOG_ERROR(d_logger, "No RFE device opened");
    return -1;
}

int rfe::set_notch(int enable)
{
    if (rfe_dev) {
        if (boardState.channelIDRX > RFE_CID_HAM_0920 ||
            boardState.channelIDRX == RFE_CID_WB_4000) {
            GR_LOG_ERROR(d_logger, "Notch filter cannot be se for this RX channel");
            return -1;
        }
        int error = 0; //! TODO: might need renaming
        boardState.notchOnOff = enable;
        std::string en_dis[2] = { "Disabling", "Enabling" };
        GR_LOG_INFO(d_logger, boost::format("%s notch filter") % en_dis[enable]);
        if ((error = RFE_ConfigureState(rfe_dev, boardState)) != 0) {
            GR_LOG_ERROR(d_logger, boost::format("Failed to change attenuation: %s") % this->strerror(error));
        }
        return error;
    }
    return -1;
}

std::string rfe::strerror(int error)
{
    switch (error) {
    case -4:
        return "error synchronizing communication";
    case -3:
        return "non-configurable GPIO pin specified. Only pins 4 and 5 are configurable.";
    case -2:
        return "couldn't read the .ini configuration file";
    case -1:
        return "communication error";
    case 1:
        return "wrong TX port - not possible to route selected TX channel";
    case 2:
        return "wrong RX port - not possible to route selected RX channel";
    case 3:
        return "TX+RX mode cannot be used when same TX and RX port is used";
    case 4:
        return "wrong mode for the cellular channel";
    case 5:
        return "cellular channels must be the same both for RX and TX";
    case 6:
        return "requested channel code is wrong";
    default:
        return "error code doesn't match";
    }
}

void rfe::get_board_state()
{
    rfe_boardState currentState = { 0 };
    if (RFE_GetState(rfe_dev, &currentState) != 0) {
        GR_LOG_ERROR(d_logger, "LimeRFE: failed to get board state");
        return;
    }

    GR_LOG_INFO(d_logger, boost::format("LimeRFE: RX channel: %d") % (int)currentState.channelIDRX);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: TX channel: %d") % (int)currentState.channelIDTX);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: PortRX: %d") % (int)currentState.selPortRX);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: PortTx: %d") % (int)currentState.selPortTX);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: Mode: %d") % (int)currentState.mode);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: Notch: %d") % (int)currentState.notchOnOff);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: Attenuation: %d") % (int)currentState.attValue);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: Enable SWR: %d") % (int)currentState.enableSWR);
    GR_LOG_INFO(d_logger, boost::format("LimeRFE: SourceSWR: %d") % (int)currentState.sourceSWR);
}

} // namespace limesdr
} // namespace gr
