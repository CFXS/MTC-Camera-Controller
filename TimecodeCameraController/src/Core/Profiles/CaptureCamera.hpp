// [CFXS] //
#pragma once
#include <Map.hpp>

namespace TCC {

#pragma pack(1)
    struct CaptureCamera_Extended {
        uint16_t _pos_X;    // +-50m
        uint16_t _pos_Y;    // +-50m
        uint16_t _pos_Z;    // +-50m
        uint16_t _rot_Pan;  // +-180deg
        uint16_t _rot_Tilt; // +-180deg
        uint16_t _rot_Roll; // +-180deg
        uint16_t _fov;      // 5-90deg
        uint16_t _ambient;  // 0-100%
        uint16_t _exposure; // +-3EV
        uint8_t _filter;    // Layer Set = val/4
        uint8_t _scene;     // Scene = val/4

        void SetPosition(float x, float y, float z) {
            _pos_X = map(x, -50, 50, 0, 0xFFFF);
            _pos_Y = map(y, -50, 50, 0, 0xFFFF);
            _pos_Z = map(z, -50, 50, 0, 0xFFFF);
        }

        void SetRotation(float pan, float tilt, float roll = 0) {
            _rot_Pan  = map(pan, -180, 180, 0, 0xFFFF);
            _rot_Tilt = map(tilt, -180, 180, 0, 0xFFFF);
            _rot_Roll = map(roll, -180, 180, 0, 0xFFFF);
        }

        void SetFOV(float fov) {
            _fov = map(fov, 5, 90, 0, 0xFFFF);
        }

        void SetAmbient(float ambient) {
            _ambient = map(ambient, 0, 1, 0, 0xFFFF);
        }

        void SetExposure(float exposure) {
            _exposure = map(exposure, -3, 3, 0, 0xFFFF);
        }

        void SetFilter(uint8_t layerSet) {
            _filter = layerSet * 4;
        }

        void SetScene(uint8_t scene) {
            _filter = scene * 4;
        }

        void Reset() {
            SetPosition(0, 0, 0);
            SetRotation(0, 0, 0);
            SetFOV(75);
            SetAmbient(0.5);
            SetExposure(0);
            SetFilter(0);
            SetScene(0);
        }
    };
#pragma pack()

} // namespace TCC