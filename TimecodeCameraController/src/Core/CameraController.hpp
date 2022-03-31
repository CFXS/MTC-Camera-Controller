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

        void SetAccelerationMultipler(float v) {
            m_AccelerationMul = v;
        }

        void SetNormalPositionMultiplier(float v) {
            m_NormalPositionMul = v;
        }

        void SetFastPositionMultiplier(float v) {
            m_FastPositionMul = v;
        }

        void SetNormalRotationMultiplier(float v) {
            m_NormalRotationMul = v;
        }

        void SetFastRotationMultiplier(float v) {
            m_FastRotationMul = v;
        }

        float GetAccelerationMultipler() const {
            return m_AccelerationMul;
        }

        float GetNormalPositionMultiplier() const {
            return m_NormalPositionMul;
        }

        float GetFastPositionMultiplier() const {
            return m_FastPositionMul;
        }

        float GetNormalRotationMultiplier() const {
            return m_NormalRotationMul;
        }

        float GetFastRotationMultiplier() const {
            return m_FastRotationMul;
        }

    private:
        void ProcessAccel();
        void SendOutput();

    private:
        float m_X    = 0;
        float m_Y    = 0;
        float m_Z    = 0;
        float m_Pan  = 0;
        float m_Tilt = 0;
        float m_Fov  = 75;

        float m_TargetX    = 0;
        float m_TargetY    = 0;
        float m_TargetZ    = 0;
        float m_TargetPan  = 0;
        float m_TargetTilt = 0;
        float m_TargetFov  = 75;

        float m_ModX    = 0;
        float m_ModY    = 0;
        float m_ModZ    = 0;
        float m_ModPan  = 0;
        float m_ModTilt = 0;

        float m_NormalPositionMul = 0.05f;
        float m_FastPositionMul   = 0.5f;
        float m_NormalRotationMul = 1.0f;
        float m_FastRotationMul   = 4.0f;
        float m_AccelerationMul   = 0.2f;

        QString m_NetworkInterfaceAddress = "null";
        QHostAddress m_SendAddress;
        bool m_AddressValid    = false;
        QUdpSocket* m_Socket   = nullptr;
        bool m_SocketConnected = false;

        uint16_t m_PatchUniverse = 1;
        uint16_t m_PatchAddress  = 1;

        ArtNetDMX_Packet m_ArtPacket;
        uint8_t m_DMX_Sequence = 0;
    };

} // namespace TCC