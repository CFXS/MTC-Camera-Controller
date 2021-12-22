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

#include <QMainWindow>
#include <Core/Gamepad/GamepadState.hpp>
#include <Core/CameraController.hpp>
#include <Core/MIDI_Machine.hpp>

namespace Ui {
    class MainWindow;
}

namespace TCC::UI {

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        /// \param rawStateToLoad state data received from SaveState signal on window close
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() = default;

    private:
        void ConfigureConnections();
        void UpdateStatusBar();
        void LoadLastSession();
        void SaveProject(bool saveAs);
        void LoadProject(const QString &path);

        float GetAxisThreshold() const;
        bool IsGamepadActive() const;

        bool IsProjectOpen() const;
        void SetProjectOpen(bool state);
        QString GetProjectName() const;

        void UpdateTitle();

        void SetChangesMade();

        void SetMTC_Port(const QString &name);
        void SetNetworkInterface(const QString &addr);
        void SetPatchUniverse(int uni);
        void SetPatchAddress(int addr);

    signals:
        void Closed();
        void GamepadActivityChanged(bool state);
        void NetworkInterfaceChanged(const QString &newAddress);
        void MIDIInterfaceChanged(int newIndex);
        void PatchChanged(uint16_t universe, uint16_t address);

    protected slots:
        void GamepadStateChanged(const GamepadState &gps, const int &playerId);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        std::unique_ptr<Ui::MainWindow> ui;
        MIDI_Machine *m_MIDI;
        CameraController *m_CameraController;

        // Gamepad
        qint64 m_LastGamepadUpdate = 0;
        bool m_GamepadActive       = false;

        // Project
        bool m_ProjectOpen    = false;
        bool m_UnsavedChanges = false;
        QString m_ProjectPath;
    };

} // namespace TCC::UI