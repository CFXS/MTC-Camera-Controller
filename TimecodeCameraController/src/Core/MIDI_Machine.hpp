// [CFXS] //
#pragma once
#include <QVector>
#include <QObject>
#include "RtMidi.h"
#include "MTC.hpp"

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

        QString GetTimeString();

        bool HaveSync() const;

    private:
        MIDI_Machine();
        ~MIDI_Machine();

        void DeleteDevice();

        void ProcessMessage(const std::vector<uint8_t>* data);
        void UpdateTimestamp(const MTC_FullFrame* frame);

    signals:
        void DeviceOpenFailed(int idx);

    private:
        QVector<DeviceInfo> m_DeviceList;
        RtMidiIn* m_CurrentDevice = nullptr;

        double m_CurrentTimestamp = 0;
        bool m_Synced             = false;
        MTC_FrameRate m_CurrentFramerate;
        MTC_FullFrame m_ReconstructedFrame;
        char m_TimeString[32];
    };

} // namespace TCC