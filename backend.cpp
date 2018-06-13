#include "backend.h"

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QUrl>
#include <QThread>

BackEnd::BackEnd(QObject *parent) :
    QObject(parent)
  , lastRequest(nullptr)
  , modbusDevice(nullptr)
{
    modbusDevice = new QModbusTcpClient(this);

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << modbusDevice->errorString();
    });

    if (!modbusDevice) {
            qDebug() << "Could not create Modbus client.";
    } else {
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &BackEnd::onStateChanged);
    }

    m_currentSpeed = 0;
    m_currentAirPressure = 0;
    m_currentAirPressureDG0 = 0;
    m_currentAirPressureDG1 = 0;
    m_connected = false;
}

BackEnd::~BackEnd()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
}

void BackEnd::setSpeed(const float speed)
{
    if (speed == m_speed)
        return;

    qDebug() << "New Speed: " << speed;

    sendData(1,(quint16) speed);

    m_speed = speed;
}

void BackEnd::setBreakLevel(const float breakLevel)
{
    if (breakLevel == m_breakLevel)
        return;

    qDebug() << "New breakLevel: " << breakLevel;

    sendData(0,(quint16) breakLevel);

    m_breakLevel = breakLevel;
}

void BackEnd::setDirection(const bool dir)
{
    if (dir == m_direction)
        return;

    qDebug() << "New Direction: " << dir;

    sendData(1,dir);

    m_direction = dir;
}

void BackEnd::setHorn(const bool on)
{
    qDebug() << "setHorn: " << on;
    sendData(6,on);
}

void BackEnd::setCompressor(const bool on)
{
    qDebug() << "setCompressor: " << on;
    sendData(8,on);
}

void BackEnd::setCurrentSpeed(const float speed)
{
    m_currentSpeed = speed;
}

float BackEnd::currentSpeed()
{
    return m_currentSpeed;
}

void BackEnd::setCurrentAirPressure(const float pressure)
{
    m_currentAirPressure = pressure;
}

float BackEnd::currentAirPressure()
{
    return m_currentAirPressure;
}

void BackEnd::setCurrentAirPressureDG0(const float pressure)
{
    m_currentAirPressureDG0 = pressure;
}

float BackEnd::currentAirPressureDG0()
{
    return m_currentAirPressureDG0;
}


void BackEnd::setCurrentAirPressureDG1(const float pressure)
{
    m_currentAirPressureDG1 = pressure;
}

float BackEnd::currentAirPressureDG1()
{
    return m_currentAirPressureDG1;
}

bool BackEnd::connected()
{
    return this->m_connected;
}

void BackEnd::on_connectButton_clicked()
{
    qDebug() << "Click";
    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
#ifdef LOCAL
        const QUrl url = QUrl::fromUserInput("127.0.0.1:5020");
#else
        const QUrl url = QUrl::fromUserInput("192.168.1.17:5020");
#endif
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());

        qDebug() << "Try to connect to " << url.host() << ":" << url.port();
        modbusDevice->setTimeout(1000);
        modbusDevice->setNumberOfRetries(5);
        if (!modbusDevice->connectDevice()) {
            qDebug() << "Connect failed: " << modbusDevice->errorString();
            this->m_connected = false;
        }
    } else {
        //modbusDevice->disconnectDevice();
    }
}
void BackEnd::on_update()
{
    if (!modbusDevice)
        return;

    QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::InputRegisters, 0, 10);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &BackEnd::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        qDebug() << "Read error: " << modbusDevice->errorString();
    }
}

void BackEnd::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            //qDebug() << entry;
        }
        this->setCurrentSpeed(unit.value(3));
        this->setCurrentAirPressure(unit.value(4));
        this->setCurrentAirPressureDG0(unit.value(5));
        this->setCurrentAirPressureDG1(unit.value(6));
        emit BackEnd::currentValuesChanged();
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() << "Read response error: "
                 << reply->errorString()
                 << " (Mobus exception: "
                 << reply->rawResult().exceptionCode()
                 << ")";
    } else {
        qDebug() << "Read response error: "
                 << reply->errorString()
                 << " (code: "
                 << reply->error()
                 <<")";
    }

    reply->deleteLater();
    QThread::msleep(100);
    emit on_update();
}


void BackEnd::onStateChanged(int state)
{
    this->m_connected = (state == QModbusDevice::ConnectedState);

    if (state == QModbusDevice::UnconnectedState)
        qDebug() << "Disonnect";
    else if (state == QModbusDevice::ConnectedState)
    {
        emit on_update();
        qDebug() << "Connect";
    }
    emit connectionChanged();
}


void BackEnd::sendData(int address, bool data)
{
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::Coils, address, 1);
    writeUnit.setValue(0, data);
    sendData(writeUnit);
}

void BackEnd::sendData(int address, quint16 data)
{
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, address, 1);
    writeUnit.setValue(0, data);
    sendData(writeUnit);
}

void BackEnd::sendData(QModbusDataUnit writeUnit)
{
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                } else if (reply->error() != QModbusDevice::NoError) {
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        qDebug() << "Write error: " << modbusDevice->errorString();
    }
}
