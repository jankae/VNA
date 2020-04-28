#ifndef DEVICE_H
#define DEVICE_H

#include "../RFExperiments/Application/Communication/Protocol.hpp"
#include <functional>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <QObject>

Q_DECLARE_METATYPE(Protocol::Datapoint);

class Device : public QObject
{
    Q_OBJECT
public:
    Device();
    ~Device();
    bool Configure(Protocol::SweepSettings settings);
signals:
    void DatapointReceived(Protocol::Datapoint);
private:
    static constexpr int VID = 1155;
    static constexpr int PID = 22336;
    static constexpr int EP_Out_Addr = 0x01;
    static constexpr int EP_In_Addr = 0x81;

    void ReceiveThread();
    static void ReceiveTrampoline(Device *dev) {
        dev->ReceiveThread();
    }

    libusb_device_handle *m_handle;
    libusb_context *m_context;
    bool m_connected;
    std::thread *m_receiveThread;
};

#endif // DEVICE_H
