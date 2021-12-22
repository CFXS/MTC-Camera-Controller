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
#include "MIDI_Machine.hpp"
#include <QMutex>
#include <QDateTime>

namespace TCC {

    ///////////////////////////////////////////////////
    static constexpr auto MIDI_BUFFER_SIZE  = 1024 * 8;
    static constexpr auto MIDI_BUFFER_COUNT = 8;
    ///////////////////////////////////////////////////

    MIDI_Machine::MIDI_Machine() {
        printf("Create MIDI_Machine\n");

        printf("MIDI Devices:\n");
        RtMidiIn *midiin = 0;

        try {
            midiin = new RtMidiIn();
        } catch (RtMidiError &error) {
            error.printMessage();
            exit(EXIT_FAILURE);
        }

        if (int portCount = midiin->getPortCount()) {
            for (int i = 0; i < portCount; i++) {
                try {
                    auto portName = midiin->getPortName(i);
                    printf(" - %s\n", portName.c_str());
                    m_DeviceList.append({QString::fromStdString(portName), i});
                } catch (RtMidiError &error) {
                    error.printMessage();
                }
            }
        } else {
            printf("No devices found\n");
        }

        delete midiin;
    }

    MIDI_Machine::~MIDI_Machine() {
        if (m_CurrentDevice) {
            DeleteDevice();
        }
    }

    ////////////////////////////////////////////////////////////////

    void MIDI_Machine::DeleteDevice() {
        delete m_CurrentDevice;
        m_CurrentDevice      = nullptr;
        m_Synced             = false;
        m_CurrentDeviceIndex = -1;
    }

    QString MIDI_Machine::IndexToName(int idx) const {
        if (idx == -1)
            return QStringLiteral("null");

        for (auto &dev : GetDevices()) {
            if (dev.index == idx)
                return dev.name;
        }

        return QStringLiteral("null");
    }

    void MIDI_Machine::SetCurrentDevice(int idx) {
        if (idx == m_CurrentDeviceIndex)
            return;

        if (m_CurrentDevice) {
            printf("Close MIDI device \"%s\"\n", GetCurrentDeviceName().toStdString().c_str());
            DeleteDevice();
        }

        if (idx < 0)
            return;

        m_CurrentDevice = new RtMidiIn();

        if (!m_CurrentDevice)
            return;

        m_Synced = false;

        m_CurrentDevice->setBufferSize(MIDI_BUFFER_SIZE, MIDI_BUFFER_COUNT);
        m_CurrentDevice->setCallback(
            [](double timeStamp, std::vector<unsigned char> *message, void *dis) {
                static_cast<MIDI_Machine *>(dis)->ProcessMessage(message);
            },
            this);

        m_CurrentDevice->openPort(idx);
        if (!m_CurrentDevice->isPortOpen()) {
            printf("Failed to open MIDI device \"%s\"\n", IndexToName(idx).toStdString().c_str());
            emit DeviceOpenFailed(idx);
            DeleteDevice();
            return;
        }

        m_CurrentDevice->ignoreTypes(false, false, true);

        m_CurrentDeviceIndex = idx;
        printf("Open MIDI device \"%s\"\n", IndexToName(idx).toStdString().c_str());
    }

    void MIDI_Machine::ProcessMessage(const std::vector<uint8_t> *data) {
        if (!m_CurrentDevice)
            return;

        if (data->size() == 10) {
            // check full timecode
            auto pack = reinterpret_cast<const MTC_FullFrame *>(data->data());
            if (pack->IsValid()) {
                static MTC_FullFrame lastReference;

                if (memcmp(pack, &lastReference, sizeof(MTC_FullFrame))) {
                    memcpy(&lastReference, pack, sizeof(MTC_FullFrame));

                    UpdateTimestamp(pack);

                    m_Synced = true;
                    m_ReconstructedFrame.ClearTime();
                }
            } else {
                m_Synced = false;
            }
        } else if (data->size() == 2 && data->at(0) == 0xF1) {
            // quarter frame
            auto seqIdx = (data->at(1) >> 4) & 0b111;
            auto val    = (seqIdx & 1) ? ((data->at(1) & 0x0F) << 4) : (data->at(1) & 0x0F);

            auto writeIndex = 3 - (seqIdx / 2);

            m_ReconstructedFrame._timedata[writeIndex] |= val;

            if (seqIdx == 7) {
                if (m_Synced) {
                    UpdateTimestamp(&m_ReconstructedFrame);
                } else {
                    m_Synced = true;
                }
                m_ReconstructedFrame.ClearTime();
            }
        }
    }

    void MIDI_Machine::UpdateTimestamp(const MTC_FullFrame *frame) {
        QMutex mutex;
        mutex.lock();

        snprintf(m_TimeString, 32, "%02d:%02d:%02d.%02d", frame->GetHours(), frame->GetMinutes(), frame->GetSeconds(), frame->GetFrames());

        m_CurrentFramerate = frame->GetRate();
        m_CurrentTimestamp = frame->ToMilliseconds();

        mutex.unlock();

        m_LastUpdateTimestamp = QDateTime::currentMSecsSinceEpoch();
    }

    QString MIDI_Machine::GetTimeString() const {
        QMutex mutex;
        mutex.lock();
        auto str = QString(m_TimeString);
        mutex.unlock();
        return str;
    }

    bool MIDI_Machine::HaveSync() const {
        return m_Synced;
    }

    bool MIDI_Machine::IsTimecodeActive() const {
        if (!HaveSync())
            return false;

        if (QDateTime::currentMSecsSinceEpoch() - GetLastUpdateTimestamp() > 100) {
            return false;
        }

        return true;
    }

} // namespace TCC
