// [CFXS] //
#pragma once

#include <QObject>

namespace TCC {

    class CameraController : public QObject {
        Q_OBJECT

    public:
        CameraController(QObject* parent = nullptr);

        void Update(float ax, float ay, float az, float ap, float at, bool posReset, bool rotReset);

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

    private:
        float m_X;
        float m_Y;
        float m_Z;
        float m_Pan;
        float m_Tilt;
    };

} // namespace TCC