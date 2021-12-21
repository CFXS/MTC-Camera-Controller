// [CFXS] //
#pragma once
#include <QVector>
#include <QObject>
#include "RtMidi.h"

namespace TCC {

    class MIDI_Machine : public QObject {
        Q_OBJECT

    public:
        struct DeviceInfo {
            QString name;
            int index;
        };

    public:
        static MIDI_Machine* GetInstance();

        const QVector<DeviceInfo>& GetDevices() {
            return m_DeviceList;
        }

        QString IndexToName(int idx);

        void SetCurrentDevice(int idx);

    private:
        MIDI_Machine();
        ~MIDI_Machine();

        void DeleteDevice();

        void ProcessMessage(const std::vector<uint8_t>* data);

    signals:
        void DeviceOpenFailed(int idx);

    private:
        QVector<DeviceInfo> m_DeviceList;
        RtMidiIn* m_CurrentDevice = nullptr;
    };

} // namespace TCC