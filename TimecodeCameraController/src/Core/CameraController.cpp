// [CFXS] //
#include "CameraController.hpp"
#include "Profiles/CaptureCamera.hpp"

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