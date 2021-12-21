// [CFXS] //
#include "MIDI_Machine.hpp"
#include "RtMidi.h"

namespace TCC {

    MIDI_Machine *MIDI_Machine::GetInstance() {
        static MIDI_Machine mm;
        return &mm;
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
                    m_DeviceNameList.append(QString::fromStdString(portName));
                } catch (RtMidiError &error) {
                    error.printMessage();
                }
            }
        } else {
            printf("No devices found\n");
        }

        delete midiin;
    }

} // namespace TCC
