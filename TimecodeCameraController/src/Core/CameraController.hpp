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

#include <Core/ArtNet/ArtNet.hpp>

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>

namespace TCC {

    class CameraController : public QObject {
        Q_OBJECT

    public:
        CameraController(QObject* parent = nullptr);

        void SetNetworkInterface(const QString& newAddress);

        void SetPatchUniverse(uint16_t universe);
        void SetPatchAddress(uint16_t address);

        const QString& GetNetworkInterface() {
            return m_NetworkInterfaceAddress;
        }

        void Update(float ax,
                    float ay,
                    float az,
                    float ap,
                    float at,
                    bool posReset,
                    bool rotReset,
                    bool fastPos,
                    bool fastRot,
                    float fovDirection);

        void Reset();

        inline float GetX() const {
            return m_X;
        }

        inline float GetY() const {
            return m_Y;
        }

        inline float GetZ() const {
            return m_Z;
        }

        inline float GetPan() const {
            return m_Pan;
        }

        inline float GetTilt() const {
            return m_Tilt;
        }

        inline float GetFOV() const {
            return m_Fov;
        }

        bool IsSocketConnected() const {
            return m_SocketConnected;
        }

    private:
        void SendOutput();

    private:
        float m_X;
        float m_Y;
        float m_Z;
        float m_Pan;
        float m_Tilt;
        float m_Fov;

        QString m_NetworkInterfaceAddress = "null";
        QHostAddress m_SendAddress;
        bool m_AddressValid    = false;
        QUdpSocket* m_Socket   = nullptr;
        bool m_SocketConnected = false;

        uint16_t m_PatchUniverse;
        uint16_t m_PatchAddress;

        ArtNetDMX_Packet m_ArtPacket;
        uint8_t m_DMX_Sequence = 0;
    };

} // namespace TCC