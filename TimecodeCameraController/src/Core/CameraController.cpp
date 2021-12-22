// [CFXS] //
#include "CameraController.hpp"
#include "Profiles/CaptureCamera.hpp"

namespace TCC {

    ///////////////////////////////////////////////////////////////
    static constexpr float POS_MUL = 0.1f;
    static constexpr float ROT_MUL = 0.1f;
    ///////////////////////////////////////////////////////////////

    CameraController::CameraController(QObject* parent) : QObject(parent) {
        printf("Create CameraController\n");
    }

    void CameraController::Update(float ax, float ay, float az, float ap, float at, bool posReset, bool rotReset) {
        m_X += ax * POS_MUL;
        m_Y += ay * POS_MUL;
        m_Z += az * POS_MUL;

        m_Pan += ap * ROT_MUL;
        m_Tilt += at * ROT_MUL;

        if (posReset) {
            m_X = 0;
            m_Y = 0;
            m_Z = 0;
        }

        if (rotReset) {
            m_Pan  = 0;
            m_Tilt = 0;
        }

        if (m_X < -50)
            m_X = -50;
        if (m_Y < -50)
            m_Y = -50;
        if (m_Z < -50)
            m_Z = -50;

        if (m_X > 50)
            m_X = 50;
        if (m_Y > 50)
            m_Y = 50;
        if (m_Z > 50)
            m_Z = 50;
    }

    void CameraController::Reset() {
        m_X    = 0;
        m_Y    = 0;
        m_Z    = 0;
        m_Pan  = 0;
        m_Tilt = 0;
    }

} // namespace TCC