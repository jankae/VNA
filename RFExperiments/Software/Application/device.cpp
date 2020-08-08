#include "device.h"

#include <signal.h>
#include <boost/log/trivial.hpp>

using namespace std;

Device::Device(QString serial)
{
    BOOST_LOG_TRIVIAL(debug) << "Starting device connection...";
    libusb_device **devList;
    m_context = nullptr;
    m_handle = nullptr;
    m_connected = false;
    lastInfoValid = false;
    libusb_init(&m_context);
//    libusb_set_debug(m_context, 4);
    auto ndevices = libusb_get_device_list(m_context, &devList);

    for (ssize_t idx = 0; idx < ndevices; idx++) {
        int ret;
        libusb_device *device = devList[idx];
        libusb_device_descriptor desc = {};

        ret = libusb_get_device_descriptor(device, &desc);
        if (ret) {
            /* some error occured */
            BOOST_LOG_TRIVIAL(error) << "Failed to get device descriptor: "
                    << libusb_strerror((libusb_error) ret);
            continue;
        }

        if (desc.idVendor != VID || desc.idProduct != PID) {
            /* Not an STM virtual COM port */
            continue;
        }

        /* Try to open the device */
        libusb_device_handle *handle = nullptr;
        ret = libusb_open(device, &handle);
        if (ret) {
            /* Failed to open */
            BOOST_LOG_TRIVIAL(warning) << "Failed to open device: "
                    << libusb_strerror((libusb_error) ret);
            continue;
        }

        char c_product[256];
        char c_serial[256];
        libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber,
                (unsigned char*) c_serial, sizeof(c_serial));
        ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct,
                (unsigned char*) c_product, sizeof(c_product));
        if (ret > 0) {
            /* managed to read the product string */
            std::string product(c_product);
            BOOST_LOG_TRIVIAL(debug) << "Opened device: " << product;
            if (product == "VNA") {
                // check serial number if necessary
                auto qSerial = QString(c_serial);
                if(serial.isEmpty() || qSerial == serial) {
                    m_handle = handle;
                    m_serial = qSerial;
                    break;
                }
            }
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Failed to get product descriptor: "
                    << libusb_strerror((libusb_error) ret);
        }
        libusb_close(handle);
    }
    libusb_free_device_list(devList, 1);
    if(!m_handle) {
        BOOST_LOG_TRIVIAL(error) << "No device found";
        throw std::runtime_error("No device found");
        return;
    }

    // Found the correct device, now connect
    /* claim the interfaces */
    for (int if_num = 1; if_num < 2; if_num++) {
        if (libusb_kernel_driver_active(m_handle, if_num)) {
            libusb_detach_kernel_driver(m_handle, if_num);
        }
        int ret = libusb_claim_interface(m_handle, if_num);
        if (ret < 0) {
            BOOST_LOG_TRIVIAL(error) << "Failed to claim interface: "
                    << libusb_strerror((libusb_error) ret);
            throw std::runtime_error("Failed to claim interface");
        }
    }
    BOOST_LOG_TRIVIAL(info) << "USB connection established" << flush;
    m_connected = true;
    m_receiveThread = new std::thread(ReceiveTrampoline, this);
}

Device::~Device()
{
    if(m_connected) {
        m_connected = false;
        m_receiveThread->join();
        for (int if_num = 1; if_num < 2; if_num++) {
            int ret = libusb_release_interface(m_handle, if_num);
            if (ret < 0) {
                BOOST_LOG_TRIVIAL(error) << "Error releasing interface" << libusb_error_name(ret);
            }
            // TODO this doesn't work
            if (libusb_kernel_driver_active(m_handle, if_num)) {
                BOOST_LOG_TRIVIAL(debug) << "Reattaching CDC ACM kernel driver." << endl;
                ret = libusb_attach_kernel_driver(m_handle, if_num);
                if (ret < 0) {
                    BOOST_LOG_TRIVIAL(error) << "Error reattaching CDC ACM kernel driver: "
                            << libusb_error_name(ret);
                }
            }
        }
        libusb_close(m_handle);
    }
}

bool Device::Configure(Protocol::SweepSettings settings)
{
    if(m_connected) {
        unsigned char buffer[128];
        Protocol::PacketInfo p;
        p.type = Protocol::PacketType::SweepSettings;
        p.settings = settings;
        unsigned int length = Protocol::EncodePacket(p, buffer, sizeof(buffer));
        if(!length) {
            BOOST_LOG_TRIVIAL(error) << "Failed to encode packet";
            return false;
        }
        int actual_length;
        auto ret = libusb_bulk_transfer(m_handle, EP_Out_Addr, buffer, length, &actual_length, 0);
        if(ret < 0) {
            BOOST_LOG_TRIVIAL(error) << "Error sending data: "
                                    << libusb_strerror((libusb_error) ret);
            return false;
        }
        return true;
    } else {
        return false;
    }
}

bool Device::SetManual(Protocol::ManualControl manual)
{
    if(m_connected) {
        unsigned char buffer[128];
        Protocol::PacketInfo p;
        p.type = Protocol::PacketType::ManualControl;
        p.manual = manual;
        unsigned int length = Protocol::EncodePacket(p, buffer, sizeof(buffer));
        if(!length) {
            BOOST_LOG_TRIVIAL(error) << "Failed to encode packet";
            return false;
        }
        int actual_length;
        auto ret = libusb_bulk_transfer(m_handle, EP_Out_Addr, buffer, length, &actual_length, 0);
        if(ret < 0) {
            BOOST_LOG_TRIVIAL(error) << "Error sending data: "
                                    << libusb_strerror((libusb_error) ret);
            return false;
        }
        return true;
    } else {
        return false;
    }
}

std::vector<QString> Device::GetDevices()
{
    std::vector<QString> serials;

    // TODO remove duplicate code (almost identical to constructor)
    libusb_device **devList;
    libusb_context *context = nullptr;
    libusb_init(&context);
    auto ndevices = libusb_get_device_list(context, &devList);

    for (ssize_t idx = 0; idx < ndevices; idx++) {
        int ret;
        libusb_device *device = devList[idx];
        libusb_device_descriptor desc = {};

        ret = libusb_get_device_descriptor(device, &desc);
        if (ret) {
            /* some error occured */
            BOOST_LOG_TRIVIAL(error) << "Failed to get device descriptor: "
                    << libusb_strerror((libusb_error) ret);
            continue;
        }

        if (desc.idVendor != VID || desc.idProduct != PID) {
            /* Not an STM virtual COM port */
            continue;
        }

        /* Try to open the device */
        libusb_device_handle *handle = nullptr;
        ret = libusb_open(device, &handle);
        if (ret) {
            /* Failed to open */
            BOOST_LOG_TRIVIAL(warning) << "Failed to open device: "
                    << libusb_strerror((libusb_error) ret);
            continue;
        }

        char c_product[256];
        char c_serial[256];
        libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber,
                (unsigned char*) c_serial, sizeof(c_serial));
        ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct,
                (unsigned char*) c_product, sizeof(c_product));
        if (ret > 0) {
            /* managed to read the product string */
            std::string product(c_product);
            BOOST_LOG_TRIVIAL(debug) << "Opened device: " << product;
            if (product == "VNA") {
                serials.push_back(QString(c_serial));
            }
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Failed to get product descriptor: "
                    << libusb_strerror((libusb_error) ret);
        }
        libusb_close(handle);
    }
    libusb_free_device_list(devList, 1);

    return serials;
}

void Device::ReceiveThread()
{
    BOOST_LOG_TRIVIAL(info) << "Receive thread started" << flush;
    constexpr int timeout = 100;
    unsigned char recbuf[2048];
    unsigned int inputCnt = 0;
    while (m_connected) {
        int actual_length;
        unsigned char data[64];
        auto ret = libusb_bulk_transfer(m_handle, EP_In_Addr, data,
                sizeof(data), &actual_length, timeout);
        if (ret == LIBUSB_ERROR_TIMEOUT || ret == LIBUSB_SUCCESS) {
            if (actual_length > 0) {
                if (inputCnt + actual_length < sizeof(recbuf)) {
                    // add received data to input buffer
                    memcpy(&recbuf[inputCnt], data, actual_length);
                    inputCnt += actual_length;
                }
                Protocol::PacketInfo packet;
                uint16_t handled_len;
                do {
                    handled_len = Protocol::DecodeBuffer(recbuf, inputCnt, &packet);
                    if (handled_len == inputCnt) {
                        // complete input buffer used up, reset counter
                        inputCnt = 0;
                    } else {
                        // only used part of the buffer, move up remaining bytes
                        uint16_t remaining = inputCnt - handled_len;
                        memmove(recbuf, &recbuf[handled_len], remaining);
                        inputCnt = remaining;
                    }
                    if(packet.type == Protocol::PacketType::Datapoint) {
                        //BOOST_LOG_TRIVIAL(debug) << "Got new datapoint: " << packet.datapoint.pointNum << std::flush;
                        emit DatapointReceived(packet.datapoint);
                    } else if(packet.type == Protocol::PacketType::Status) {
                        BOOST_LOG_TRIVIAL(debug) << "Got status" << std::flush;
                        emit ManualStatusReceived(packet.status);
                    } else if(packet.type == Protocol::PacketType::DeviceInfo) {
                        lastInfo = packet.info;
                        lastInfoValid = true;
                        emit DeviceInfoUpdated();
                    }
                } while (handled_len > 0);
            }
        } else if (ret < 0) {
            BOOST_LOG_TRIVIAL(error) << "Error receiving data: "
                    << libusb_strerror((libusb_error) ret);
            emit ConnectionLost();
            return;
        }
    }
    BOOST_LOG_TRIVIAL(debug) << "Disconnected, receive thread exiting";
}

Protocol::DeviceInfo Device::getLastInfo() const
{
    return lastInfo;
}

QString Device::getLastDeviceInfoString()
{
    QString ret;
    if(!lastInfoValid) {
        ret.append("No device information available yet");
    } else {
        ret.append("HW Rev.");
        ret.append(lastInfo.HW_Revision);
        ret.append(" FW "+QString::number(lastInfo.FW_major)+"."+QString::number(lastInfo.FW_minor).rightJustified(2, '0'));
        ret.append(" Temps: "+QString::number(lastInfo.temperatures.source)+"°C/"+QString::number(lastInfo.temperatures.LO1)+"°C/"+QString::number(lastInfo.temperatures.MCU)+"°C");
        ret.append(" Reference:");
        if(lastInfo.extRefInUse) {
            ret.append("External");
        } else {
            ret.append("Internal");
            if(lastInfo.extRefAvailable) {
                ret.append(" (External available)");
            }
        }
    }
    return ret;
}

QString Device::serial() const
{
    return m_serial;
}
