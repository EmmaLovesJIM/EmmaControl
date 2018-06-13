#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QtDebug>
#include <QModbusDataUnit>

class QModbusClient;
class QModbusReply;

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float speed WRITE setSpeed)
    Q_PROPERTY(float breakLevel WRITE setBreakLevel)
    Q_PROPERTY(bool direction WRITE setDirection)
    Q_PROPERTY(bool horn WRITE setHorn)
    Q_PROPERTY(bool compressor WRITE setCompressor)
    Q_PROPERTY(float currentSpeed READ currentSpeed NOTIFY currentValuesChanged)
    Q_PROPERTY(float currentAirPressure READ currentAirPressure NOTIFY currentValuesChanged)
    Q_PROPERTY(float currentAirPressureDG0 READ currentAirPressureDG0 NOTIFY currentValuesChanged)
    Q_PROPERTY(float currentAirPressureDG1 READ currentAirPressureDG1 NOTIFY currentValuesChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectionChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    void setSpeed(const float speed);
    void setCurrentSpeed(const float speed);
    void setCurrentAirPressure(const float pressure);
    void setCurrentAirPressureDG0(const float pressure);
    void setCurrentAirPressureDG1(const float pressure);
    void setBreakLevel(const float breakLevel);
    void setDirection(const bool dir);
    void setHorn(const bool on);
    void setCompressor(const bool on);

    float currentSpeed();
    float currentAirPressure();
    float currentAirPressureDG0();
    float currentAirPressureDG1();
    bool connected();

signals:
    void currentValuesChanged();
    void connectionChanged();

public slots:
    void on_connectButton_clicked();
private slots:
    void on_update();
    void onStateChanged(int state);
    void readReady();
private:
    float m_speed;
    float m_breakLevel;
    float m_currentSpeed;
    float m_currentAirPressure;
    float m_currentAirPressureDG0;
    float m_currentAirPressureDG1;
    bool m_direction;
    bool m_connected;
    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;

    void sendData(int address, bool data);
    void sendData(int address, quint16 data);
    void sendData(QModbusDataUnit writeUnit);
};

#endif // BACKEND_H
