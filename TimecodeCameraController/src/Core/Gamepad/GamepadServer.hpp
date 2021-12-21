// https://github.com/wboler05
#pragma once

#include <QObject>

#include "GamepadState.hpp"

class GamepadServer : public QObject {
    Q_OBJECT

public:
    static GamepadServer& instance() {
        if (s_instance == nullptr) {
            s_instance = new GamepadServer();
        }
        return *s_instance;
    }

    // Gamepad update period
    const int UPDATE_PERIOD_MS = 15;

signals:
    void stateUpdate(const GamepadState& gps, const int& player);

protected slots:
    void readState();

private:
    GamepadServer();

    static GamepadServer* s_instance;
};