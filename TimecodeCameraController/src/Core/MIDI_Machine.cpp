// [CFXS] //
#include "MIDI_Machine.hpp"
#include <QMutex>

namespace TCC {

    ///////////////////////////////////////////////////
    static constexpr auto MIDI_BUFFER_SIZE  = 1024 * 8;
    static constexpr auto MIDI_BUFFER_COUNT = 8;
    ///////////////////////////////////////////////////

    MIDI_Machine *MIDI_Machine::GetInstance() {
        static MIDI_Machine mm;
        return &mm;
    }

    MIDI_Machine::~MIDI_Machine() {
        if (m_CurrentDevice) {
            DeleteDevice();
        }
    }

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

    ////////////////////////////////////////////////////////////////

    void MIDI_Machine::DeleteDevice() {
        delete m_CurrentDevice;
        m_CurrentDevice = nullptr;
        m_Synced        = false;
    }

    QString MIDI_Machine::IndexToName(int idx) {
        if (idx == -1)
            return QStringLiteral("null");

        for (auto &dev : GetDevices()) {
            if (dev.index == idx)
                return dev.name;
        }

        return QStringLiteral("unknown");
    }

    void MIDI_Machine::SetCurrentDevice(int idx) {
        if (m_CurrentDevice) {
            printf("Close MIDI device \"%s\"\n", m_CurrentDevice->getPortName().c_str());
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

        printf("MIDI device \"%s\" open\n", IndexToName(idx).toStdString().c_str());
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
        } else if (m_Synced && data->size() == 2 && data->at(0) == 0xF1) {
            // quarter frame
            auto seqIdx = (data->at(1) >> 4) & 0b111;
            auto val    = (seqIdx & 1) ? ((data->at(1) & 0x0F) << 4) : (data->at(1) & 0x0F);

            auto writeIndex = 3 - (seqIdx / 2);

            m_ReconstructedFrame._timedata[writeIndex] |= val;

            if (seqIdx == 7) {
                UpdateTimestamp(&m_ReconstructedFrame);
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

        //printf("[MTC] %s %s (%.1lfms)\n", m_TimeString, MTC_FRAMERATE_TO_STRING[m_CurrentFramerate], m_CurrentTimestamp);
    }

    QString MIDI_Machine::GetTimeString() {
        QMutex mutex;
        mutex.lock();
        auto str = QString(m_TimeString);
        mutex.unlock();
        return str;
    }

    bool MIDI_Machine::HaveSync() const {
        return m_Synced;
    }

} // namespace TCC
