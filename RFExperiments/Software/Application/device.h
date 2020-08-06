#ifndef DEVICE_H
#define DEVICE_H

#include "../RFExperiments/Application/Communication/Protocol.hpp"
#include <functional>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <QObject>

Q_DECLARE_METATYPE(Protocol::Datapoint);
Q_DECLARE_METATYPE(Protocol::Status);

class Device : public QObject
{
    Q_OBJECT
public:
    // connect to a VNA device. If serial is specified only connecting to this device, otherwise to the first one found
    Device(QString serial = QString());
    ~Device();
    bool Configure(Protocol::SweepSettings settings);
    bool SetManual(Protocol::ManualControl manual);
    // Returns serial numbers of all connected devices
    static std::vector<QString> GetDevices();
    QString serial() const;

signals:
    void DatapointReceived(Protocol::Datapoint);
    void StatusReceived(Protocol::Status);
    void ConnectionLost();
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
    QString m_serial;
    bool m_connected;
    std::thread *m_receiveThread;
};

#endif // DEVICE_H
