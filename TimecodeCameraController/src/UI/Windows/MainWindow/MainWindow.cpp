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
#include "MainWindow.hpp"
#include <qnamespace.h>
#include "ui_MainWindow.h"
#include <QCloseEvent>
#include <QTreeView>
#include <QAction>
#include <QLayout>
#include <QSettings>
#include <QFileDialog>
#include <QNetworkInterface>
#include <QTimer>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>

#include <Core/MIDI_Machine.hpp>
#include <Core/Gamepad/GamepadServer.hpp>
#include "MTC_TextWidgetGL.hpp"

namespace TCC::UI {

    /////////////////////////////////////////////////////////////////
    static constexpr auto GAMEPAD_AXIS_THRESHOLD = 8000;
    /////////////////////////////////////////////////////////////////

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
        ui->setupUi(this);
        resize(1280, 720);

        m_MIDI             = new MIDI_Machine();
        m_CameraController = new CameraController(this);

        ui->content->setStyleSheet("border: 1px solid palette(dark);");
        ui->statusbar->setStyleSheet("font-size: 10pt;");

        auto mtcLabel = new MTC_TextWidgetGL(m_MIDI);
        ui->mtc_TimeContainer->layout()->addWidget(mtcLabel);

        // MIDI Ports

        ui->cb_MTC_Port->addItem(" - none - ", QVariant(-1));

        for (auto& dev : m_MIDI->GetDevices()) {
            ui->cb_MTC_Port->addItem(dev.name, QVariant(dev.index));
        }

        // Network Interfaces

        ui->cb_Netif->addItem(" - none - ", QVariant("null"));

        printf("Network Interfaces:\n");
        for (auto& netif : QNetworkInterface::allInterfaces()) {
            if (netif.addressEntries().isEmpty())
                continue;
            if (netif.type() != QNetworkInterface::Ethernet && netif.type() != QNetworkInterface::Wifi &&
                netif.type() != QNetworkInterface::Loopback)
                continue;
            if (!netif.flags().testFlag(QNetworkInterface::CanMulticast) || !netif.flags().testFlag(QNetworkInterface::IsUp))
                continue;

            for (auto& addr : netif.addressEntries()) {
                if (addr.ip().protocol() != QAbstractSocket::IPv4Protocol)
                    continue;
                if (!addr.ip().isGlobal())
                    continue;

                ui->cb_Netif->addItem(addr.ip().toString() + QStringLiteral(" [") + netif.humanReadableName() + QStringLiteral("]"),
                                      QVariant(addr.ip().toString()));
                printf(" - %s [%s]\n", addr.ip().toString().toStdString().c_str(), netif.humanReadableName().toStdString().c_str());
            }
        }

        ConfigureConnections();
        UpdateStatusBar();

        // local process and update loop
        auto localLoop_30 = new QTimer(this);
        connect(localLoop_30, &QTimer::timeout, [=]() {
            mtcLabel->repaint();

            if (m_GamepadActive != IsGamepadActive()) {
                m_GamepadActive = IsGamepadActive();
                emit GamepadActivityChanged(m_GamepadActive);
            }

            static bool lastProjectLoaded = false;
            if (!lastProjectLoaded) {
                lastProjectLoaded = true;
                LoadLastSession();
            }
        });
        localLoop_30->start(1000 / 40);
    }

    void MainWindow::LoadLastSession() {
        QString lastSessionCfgPath =
            QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/LastSession.tcc";

        if (QFileInfo(lastSessionCfgPath).exists()) {
            QSettings lastSession(lastSessionCfgPath, QSettings::IniFormat);
            LoadProject(lastSession.value("ProjectPath").toString());
        } else {
            UpdateTitle();
        }
    }

    void MainWindow::SaveProject(bool saveAs) {
        if (saveAs || !IsProjectOpen()) {
            QString newPath = QFileDialog::getSaveFileName(this, "Save Project As...", "./", "CFXS TCC Project (*.tcc)");

            if (newPath.isEmpty())
                return;

            m_ProjectPath = newPath;
            m_ProjectOpen = true;
        }

        QSettings cfg(m_ProjectPath, QSettings::IniFormat);
        cfg.setValue("WindowState", saveState());
        cfg.setValue("WindowGeometry", saveGeometry());
        cfg.setValue("MTC_Port", m_MIDI->GetCurrentDeviceName());
        cfg.setValue("NetworkInterface", m_CameraController->GetNetworkInterface());
        cfg.setValue("PatchUniverse", ui->sb_Universe->value());
        cfg.setValue("PatchAddress", ui->sb_Address->value());
        cfg.sync();

        QSettings lastSession(
            QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/LastSession.tcc",
            QSettings::IniFormat);
        lastSession.setValue("ProjectPath", m_ProjectPath);
        lastSession.sync();

        m_UnsavedChanges = false;
        UpdateTitle();
    }

    void MainWindow::LoadProject(const QString& path) {
        if (m_UnsavedChanges) {
        } else {
            if (!path.isEmpty() && QFileInfo::exists(path)) {
                m_ProjectPath = path;
                m_ProjectOpen = true;

                QSettings cfg(m_ProjectPath, QSettings::IniFormat);
                restoreState(cfg.value("WindowState").toByteArray());
                restoreGeometry(cfg.value("WindowGeometry").toByteArray());
                SetMTC_Port(cfg.value("MTC_Port").toString());
                SetNetworkInterface(cfg.value("NetworkInterface").toString());
                SetPatchUniverse(cfg.value("PatchUniverse").toInt());
                SetPatchAddress(cfg.value("PatchAddress").toInt());

                m_UnsavedChanges = false;
            } else {
                if (!path.isEmpty()) {
                    QSettings lastSession(
                        QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/LastSession.tcc",
                        QSettings::IniFormat);
                    lastSession.setValue("ProjectPath", "");
                    QMessageBox::critical(nullptr, CFXS_PROGRAM_NAME, "Project file from last session not found");
                }
            }
        }

        UpdateTitle();
    }

    void MainWindow::SetMTC_Port(const QString& name) {
        int cbIndex = 1;
        for (auto& entry : m_MIDI->GetDevices()) {
            if (entry.name == name) {
                m_MIDI->SetCurrentDevice(entry.index);
                ui->cb_MTC_Port->setCurrentIndex(cbIndex);
                return;
            }
            cbIndex++;
        }

        m_MIDI->SetCurrentDevice(-1);
        ui->cb_MTC_Port->setCurrentIndex(0);
    }

    void MainWindow::SetNetworkInterface(const QString& addr) {
        for (int i = 1; i < ui->cb_Netif->count(); i++) {
            if (ui->cb_Netif->itemData(i, Qt::UserRole).toString() == addr) {
                m_CameraController->SetNetworkInterface(addr);
                ui->cb_Netif->setCurrentIndex(i);
                return;
            }
        }

        m_CameraController->SetNetworkInterface("null");
        ui->cb_Netif->setCurrentIndex(0);
    }

    void MainWindow::SetPatchUniverse(int uni) {
        ui->sb_Universe->setValue(uni);
        m_CameraController->SetPatchUniverse(uni);
    }

    void MainWindow::SetPatchAddress(int addr) {
        ui->sb_Address->setValue(addr);
        m_CameraController->SetPatchAddress(addr);
    }

    void MainWindow::UpdateTitle() {
        if (m_UnsavedChanges) {
            setWindowTitle(QStringLiteral(CFXS_PROGRAM_NAME) + " " + QStringLiteral(CFXS_VERSION_STRING) + " - " + GetProjectName() +
                           " [unsaved changes]");
        } else {
            setWindowTitle(QStringLiteral(CFXS_PROGRAM_NAME) + " " + QStringLiteral(CFXS_VERSION_STRING) + " - " + GetProjectName());
        }

        UpdateStatusBar();
    }

    void MainWindow::ConfigureConnections() {
        connect(&GamepadServer::instance(), SIGNAL(stateUpdate(GamepadState, int)), this, SLOT(GamepadStateChanged(GamepadState, int)));

        connect(this, &MainWindow::GamepadActivityChanged, [=](bool state) {
            UpdateStatusBar();
        });

        connect(ui->sb_Universe, &QSpinBox::editingFinished, [=]() {
            ui->sb_Universe->findChild<QLineEdit*>()->clearFocus();
        });

        connect(ui->sb_Address, &QSpinBox::editingFinished, [=]() {
            ui->sb_Address->findChild<QLineEdit*>()->clearFocus();
        });

        connect(ui->sb_Universe, &QSpinBox::valueChanged, [=](int val) {
            emit PatchChanged(static_cast<uint16_t>(val), ui->sb_Address->value());
            m_CameraController->SetPatchUniverse(val);
            SetChangesMade();
        });

        connect(ui->sb_Address, &QSpinBox::valueChanged, [=](int val) {
            emit PatchChanged(ui->sb_Address->value(), static_cast<uint16_t>(val));
            m_CameraController->SetPatchAddress(val);
            SetChangesMade();
        });

        connect(ui->cb_MTC_Port, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            emit MIDIInterfaceChanged(ui->cb_MTC_Port->itemData(index, Qt::UserRole).toInt());
            SetChangesMade();
        });

        connect(ui->cb_Netif, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            emit NetworkInterfaceChanged(ui->cb_Netif->itemData(index, Qt::UserRole).toString());
            SetChangesMade();
        });

        connect(this, &MainWindow::MIDIInterfaceChanged, [=](int idx) {
            m_MIDI->SetCurrentDevice(idx);
        });

        connect(this, &MainWindow::NetworkInterfaceChanged, [=](const QString& addr) {
            m_CameraController->SetNetworkInterface(addr);
        });

        connect(ui->actionSave, &QAction::triggered, this, [=]() {
            SaveProject(false);
        });

        connect(ui->actionSave_As, &QAction::triggered, this, [=]() {
            SaveProject(true);
        });
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        if (m_UnsavedChanges) {
            auto res = QMessageBox::question(
                nullptr,
                CFXS_PROGRAM_NAME,
                "Unsaved changes in current project\n\nSave before exiting?",
                QMessageBox::StandardButton::Save | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel,
                QMessageBox::StandardButton::Save);

            if (res == QMessageBox::StandardButton::Save) {
                SaveProject(false);
            } else if (res == QMessageBox::StandardButton::Cancel) {
                event->ignore();
                return;
            }
        }

        printf("Close MainWindow\n");
        emit Closed();
        event->accept();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MainWindow::IsProjectOpen() const {
        return m_ProjectOpen;
    }

    void MainWindow::SetProjectOpen(bool state) {
        m_ProjectOpen = state;
    }

    QString MainWindow::GetProjectName() const {
        if (IsProjectOpen()) {
            return m_ProjectPath.mid(m_ProjectPath.lastIndexOf("/") + 1, m_ProjectPath.lastIndexOf('.') - 1);
        } else {
            return "No project open";
        }
    }

    float MainWindow::GetAxisThreshold() const {
        return GAMEPAD_AXIS_THRESHOLD;
    }

    bool MainWindow::IsGamepadActive() const {
        return m_LastGamepadUpdate && (QDateTime::currentMSecsSinceEpoch() - m_LastGamepadUpdate < 500);
    }

    void MainWindow::UpdateStatusBar() {
        if (IsProjectOpen()) {
            ui->statusbar->showMessage("Project Path: " + m_ProjectPath + " | " +
                                       QString(IsGamepadActive() ? "Gamepad Active" : "Gamepad Not Detected"));
        } else {
            ui->statusbar->showMessage(IsGamepadActive() ? "Gamepad input active" : "Gamepad not detected");
        }
    }

    void MainWindow::SetChangesMade() {
        if (!m_UnsavedChanges) {
            m_UnsavedChanges = true;
            UpdateTitle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Gamepad input

    static float map(float value, float from1, float to1, float from2, float to2) {
        return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
    }

    void MainWindow::GamepadStateChanged(const GamepadState& state, const int& playerId) {
        m_LastGamepadUpdate = QDateTime::currentMSecsSinceEpoch();

        float accTarget_Pos_X = 0;
        if (fabs(state.m_lThumb.xAxis) > GetAxisThreshold())
            accTarget_Pos_X = map(state.m_lThumb.xAxis, -32768.0f, 32767.0f, 0, 2) - 1;

        float accTarget_Pos_Y = 0;
        if (fabs(state.m_lThumb.yAxis) > GetAxisThreshold())
            accTarget_Pos_Y = map(state.m_lThumb.yAxis, -32768.0f, 32767.0f, 0, 2) - 1;

        float accTarget_Pos_Z = 0;
        accTarget_Pos_Z       = -map(state.m_lTrigger, 0.0f, 255.0f, 0, 1) + map(state.m_rTrigger, 0, 255, 0, 1);

        float accTarget_Pan = 0;
        if (fabs(state.m_rThumb.xAxis) > GetAxisThreshold())
            accTarget_Pan = map(state.m_rThumb.xAxis, -32768.0f, 32767.0f, 0, 2) - 1;

        float accTarget_Tilt = 0;
        if (fabs(state.m_rThumb.yAxis) > GetAxisThreshold())
            accTarget_Tilt = map(state.m_rThumb.yAxis, -32768.0f, 32767.0f, 0, 2) - 1;

        float fovDir = 0;

        if (state.m_pad_y)
            fovDir = -0.1f;
        if (state.m_pad_x)
            fovDir = 0.1f;

        if (state.m_rShoulder && state.m_pad_y)
            fovDir = -100; // reset

        m_CameraController->Update(accTarget_Pos_X,        // x
                                   -accTarget_Pos_Y,       // y
                                   accTarget_Pos_Z,        // z
                                   accTarget_Pan,          // pan
                                   accTarget_Tilt,         // tilt
                                   state.m_lThumb.pressed, // reset pos
                                   state.m_rThumb.pressed, // reset rot
                                   state.m_lShoulder,      // fast pos
                                   state.m_rShoulder,      // fast rot
                                   fovDir                  // fov in/out
        );
    }

} // namespace TCC::UI