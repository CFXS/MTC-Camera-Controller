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
        char m_ID[8]       = "Art-Net";
        uint16_t m_OpCode  = 0x5000;
        uint16_t m_Version = 0x000E;
        uint8_t m_Sequence;
        uint8_t m_Physical = 0;
        uint16_t m_Universe;
        uint16_t m_DataLength = 512;
        uint8_t m_Data[512];
    };
#pragma pack()

} // namespace TCC