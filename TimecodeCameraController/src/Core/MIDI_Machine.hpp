// [CFXS] //
#pragma once
#include <QStringList>

namespace TCC {

    class MIDI_Machine {
    public:
        static MIDI_Machine* GetInstance();

        const QStringList& GetDeviceNameList() {
            return m_DeviceNameList;
        }

    private:
        MIDI_Machine();

    private:
        QStringList m_DeviceNameList;
    };

} // namespace TCC