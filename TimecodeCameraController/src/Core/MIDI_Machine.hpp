// ---------------------------------------------------------------------
// CFXS TImecodeCameraController <https://github.com/CFXS/TimecodeCameraController>
// Copyright (C) 2021 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
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
        MIDI_Machine();
        ~MIDI_Machine();

        const QVector<DeviceInfo>& GetDevices() const {
            return m_DeviceList;
        }

        void SetCurrentDevice(int idx);

        QString IndexToName(int idx) const;

        QString GetTimeString() const;

        bool HaveSync() const;

        QString GetCurrentDeviceName() const {
            return IndexToName(m_CurrentDeviceIndex);
        }

        qint64 GetLastUpdateTimestamp() const {
            return m_LastUpdateTimestamp;
        }

        qint64 GetTimeMilliseconds() const {
            return m_CurrentTimestamp; // cast to int - double precision probably not needed
        }

        bool IsTimecodeActive() const;

    private:
        void DeleteDevice();

        void ProcessMessage(const std::vector<uint8_t>* data);
        void UpdateTimestamp(const MTC_FullFrame* frame);

    signals:
        void DeviceOpenFailed(int idx);

    private:
        QVector<DeviceInfo> m_DeviceList;
        RtMidiIn* m_CurrentDevice = nullptr;
        int m_CurrentDeviceIndex  = -1;

        double m_CurrentTimestamp = 0;
        bool m_Synced             = false;
        MTC_FrameRate m_CurrentFramerate;
        MTC_FullFrame m_ReconstructedFrame;
        char m_TimeString[32];
        qint64 m_LastUpdateTimestamp = 0;
    };

} // namespace TCC