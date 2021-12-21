// [CFXS] //
#pragma once

namespace TCC {

    ///////////////////////////////////////////////////////////////////////
    static const uint8_t MTC_FULL_MATCH[] = {0xF0, 0x7F, 0x7F, 0x01, 0x01}; // first 5 bytes of full timecode message
    ///////////////////////////////////////////////////////////////////////

    namespace _MTC_FrameRate {
        enum MTC_FrameRate : int { FPS_24, FPS_25, FPS_29_97, FPS_30, _COUNT };
    }
    using MTC_FrameRate = _MTC_FrameRate::MTC_FrameRate;

    static constexpr int MTC_FRAMERATE_TO_INT[MTC_FrameRate::_COUNT]            = {24, 25, 30, 30};
    static constexpr const char* MTC_FRAMERATE_TO_STRING[MTC_FrameRate::_COUNT] = {"24 FPS", "25 FPS", "29.97 FPS DF", "30 FPS"};

    struct MTC_Full {
        uint8_t header[5];
        uint8_t hh;
        uint8_t mm;
        uint8_t ss;
        uint8_t ff;
        uint8_t end;

        inline bool IsValid() const {
            return (memcmp(header, MTC_FULL_MATCH, sizeof(header)) == 0) && end == 0xF7;
        }

        inline int GetHours() const {
            return (hh & 0b11111) % 24;
        }

        inline int GetMinutes() const {
            return (mm & 0b111111) % 60;
        }

        inline int GetSeconds() const {
            return (mm & 0b111111) % 60;
        }

        inline int GetFrames() const {
            return (mm & 0b11111) % MTC_FRAMERATE_TO_INT[GetRate()];
        }

        inline int GetRate() const {
            switch ((hh >> 5) & 0b11) {
                case 0b00: return MTC_FrameRate::FPS_24;
                case 0b01: return MTC_FrameRate::FPS_25;
                case 0b10: return MTC_FrameRate::FPS_29_97;
                case 0b11: return MTC_FrameRate::FPS_30;
            }

            return MTC_FrameRate::FPS_30;
        }
    };
    ////////////////////////////////////////////////////////////////////////

} // namespace TCC