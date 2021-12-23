// ---------------------------------------------------------------------
// CFXS MTC Camera Controller <https://github.com/CFXS/MTC-Camera-Controller>
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

// Art-Net4 spec

namespace TCC {

    static constexpr uint16_t ARTNET_PORT = 6454;

#pragma pack(1)
    class ArtNetDMX_Packet {
    public:
        ArtNetDMX_Packet() = default;

        void SetUniverse(uint16_t uni) {
            m_Universe = uni;
        }

        uint8_t* GetRawData() {
            return reinterpret_cast<uint8_t*>(this);
        }

        void SetSequence(uint8_t seq) {
            m_Sequence = seq;
        }

        template<typename T>
        T* GetDataField() {
            return reinterpret_cast<T*>(m_Data);
        }

    private:
        char m_ID[8]       = "Art-Net";
        uint16_t m_OpCode  = 0x5000;
        uint16_t m_Version = 0x0E00;
        uint8_t m_Sequence;
        uint8_t m_Physical = 0;
        uint16_t m_Universe;
        uint16_t m_DataLength = 0x0002;
        uint8_t m_Data[512];
    };
#pragma pack()

} // namespace TCC