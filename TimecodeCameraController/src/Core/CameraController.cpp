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
#include "CameraController.hpp"
#include "Profiles/CaptureCamera.hpp"
#include <QMutex>

namespace TCC {

    ///////////////////////////////////////////////////////////////
    static constexpr float POS_MUL_NORMAL = 0.025f;
    static constexpr float POS_MUL_FAST   = 0.15f;
    static constexpr float ROT_MUL_NORMAL = 0.1f;
    static constexpr float ROT_MUL_FAST   = 0.75f;

    static constexpr float DEFAULT_FOV = 75.0f;
    ///////////////////////////////////////////////////////////////

    CameraController::CameraController(QObject* parent) : QObject(parent) {
        printf("Create CameraController\n");
        Reset();
    }

    void CameraController::SetNetworkInterface(const QString& newAddress) {
        m_NetworkInterfaceAddress = newAddress;
        printf("Set interface to %s\n", newAddress.toStdString().c_str());
    }

    void CameraController::SetPatchUniverse(uint16_t universe) {
        m_PatchUniverse = universe;
    }

    void CameraController::SetPatchAddress(uint16_t address) {
        m_PatchUniverse = address;
    }

    void CameraController::Update(float ax,
                                  float ay,
                                  float az,
                                  float ap,
                                  float at,
                                  bool posReset,
                                  bool rotReset,
                                  bool fastPos,
                                  bool fastRot,
                                  float fovDirection) {
        QMutex mutex;
        mutex.lock();
        m_X += ax * (fastPos ? POS_MUL_FAST : POS_MUL_NORMAL);
        m_Y += ay * (fastPos ? POS_MUL_FAST : POS_MUL_NORMAL);
        m_Z += az * (fastPos ? POS_MUL_FAST : POS_MUL_NORMAL);

        m_Pan += ap * (fastRot ? ROT_MUL_FAST : ROT_MUL_NORMAL);
        m_Tilt += at * (fastRot ? ROT_MUL_FAST : ROT_MUL_NORMAL);

        if (posReset) {
            m_X = 0;
            m_Y = 0;
            m_Z = 0;
        }

        if (rotReset) {
            m_Pan  = 0;
            m_Tilt = 0;
            m_Fov  = DEFAULT_FOV;
        }

        if (fovDirection > -90)
            m_Fov += fovDirection;
        else
            m_Fov = DEFAULT_FOV;

        if (m_X < -50)
            m_X = -50;
        if (m_Y < -50)
            m_Y = -50;
        if (m_Z < -50)
            m_Z = -50;
        if (m_Pan < -180)
            m_Pan = 180;
        if (m_Tilt < -180)
            m_Tilt = 180;
        if (m_Fov < 5)
            m_Fov = 5;

        if (m_X > 50)
            m_X = 50;
        if (m_Y > 50)
            m_Y = 50;
        if (m_Z > 50)
            m_Z = 50;
        if (m_Pan > 180)
            m_Pan = -180;
        if (m_Tilt > 180)
            m_Tilt = -180;
        if (m_Fov > 90)
            m_Fov = 90;
        mutex.unlock();
    }

    void CameraController::Reset() {
        m_X    = 0;
        m_Y    = 0;
        m_Z    = 0;
        m_Pan  = 0;
        m_Tilt = 0;
        m_Fov  = DEFAULT_FOV;
    }

} // namespace TCC