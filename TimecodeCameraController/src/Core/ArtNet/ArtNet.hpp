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

// Art-Net4 spec

namespace TCC {

#pragma pack(1)
    class ArtNetDMX_Packet {
    public:
        ArtNetDMX_Packet() = default;

        void CopyData(uint8_t* data, uint8_t sequence) {
            memcpy(m_Data, data, 512);
            m_Sequence = sequence;
        }

        void SetUniverse(uint16_t uni) {
            m_Universe = uni;
        }

        uint8_t* GetRawData() {
            return reinterpret_cast<uint8_t*>(this);
        }

    private:
        __unused char m_ID[8]       = "Art-Net";
        __unused uint16_t m_OpCode  = 0x5000;
        __unused uint16_t m_Version = 0x000E;
        uint8_t m_Sequence;
        __unused uint8_t m_Physical = 0;
        uint16_t m_Universe;
        __unused uint16_t m_DataLength = 512;
        uint8_t m_Data[512];
    };
#pragma pack()

} // namespace TCC