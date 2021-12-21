// ---------------------------------------------------------------------
// CFXS TImecodeCameraController <https://github.com/CFXS/TImecodeCameraController>
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

namespace TCC {

    ///////////////////////////////////////////////////////////////////////
    static const uint8_t MTC_FULL_MATCH[] = {0xF0, 0x7F, 0x7F, 0x01, 0x01}; // first 5 bytes of full timecode message
    ///////////////////////////////////////////////////////////////////////

    namespace _MTC_FrameRate {
        enum MTC_FrameRate : int { FPS_24, FPS_25, FPS_29_97, FPS_30, _COUNT };
    }
    using MTC_FrameRate = _MTC_FrameRate::MTC_FrameRate;

    static constexpr int MTC_FRAMERATE_TO_INT[MTC_FrameRate::_COUNT]            = {24, 25, 30, 30};
    static constexpr double MTC_FRAMERATE_TO_DOUBLE[MTC_FrameRate::_COUNT]      = {24, 25, 29.97, 30};
    static constexpr const char* MTC_FRAMERATE_TO_STRING[MTC_FrameRate::_COUNT] = {"24 FPS", "25 FPS", "29.97 FPS DF", "30 FPS"};

#pragma pack(1)
    struct MTC_FullFrame {
        uint8_t header[5];
        union {
            struct {
                uint8_t hh;
                uint8_t mm;
                uint8_t ss;
                uint8_t ff;
            };
            uint8_t _timedata[4];
            uint32_t _timedata32;
        };
        uint8_t end;

        inline void ClearTime() {
            _timedata32 = 0;
        }

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
            return (ss & 0b111111) % 60;
        }

        inline int GetFrames() const {
            return (ff & 0b11111) % MTC_FRAMERATE_TO_INT[GetRate()];
        }

        inline MTC_FrameRate GetRate() const {
            switch ((hh >> 5) & 0b11) {
                case 0b00: return MTC_FrameRate::FPS_24;
                case 0b01: return MTC_FrameRate::FPS_25;
                case 0b10: return MTC_FrameRate::FPS_29_97;
                case 0b11: return MTC_FrameRate::FPS_30;
            }

            return MTC_FrameRate::FPS_30;
        }

        inline double ToMilliseconds() const {
            return GetSeconds() * 1000 + GetMinutes() * 60000 + GetHours() * 3600000 +
                   (1000.0 / MTC_FRAMERATE_TO_DOUBLE[GetRate()] * GetFrames());
        }
    };
#pragma pack()
    ////////////////////////////////////////////////////////////////////////

} // namespace TCC