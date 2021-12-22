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
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <Map.hpp>

#include <Core/MIDI_Machine.hpp>
#include <Core/Gamepad/GamepadServer.hpp>

namespace TCC::UI {

    //////////////////////////////////////////////////////////////////////////////////////////////////

    class MTC_TextWidgetGL : public QOpenGLWidget {
    public:
        MTC_TextWidgetGL() = default;

        void SetString(QString str) {
            m_String = str;
        }

        void paintEvent(QPaintEvent* e) override {
            QPainter painter(this);

            painter.fillRect(0, 0, width(), height(), QBrush(qApp->palette().color(QPalette::Window)));

            QPainterPath path;
            path.addRoundedRect(QRectF(0, 0, width(), height()), 4, 4);
            //QPen pen(Qt::black, 10);
            //painter.setPen(pen);
            painter.fillPath(path, QBrush(QColor(16, 16, 16)));
            //painter.drawPath(path);

            auto font = QFont("Consolas", 20);
            font.setStyleStrategy(QFont::PreferQuality);
            painter.setFont(font);

            if (MIDI_Machine::GetInstance()->HaveSync()) {
                m_String = MIDI_Machine::GetInstance()->GetTimeString();
                if (MIDI_Machine::GetInstance()->IsTimecodeActive()) {
                    painter.setPen(QColor{0, 255, 0});
                } else {
                    painter.setPen(QColor{160, 160, 160});
                }
            } else {
                m_String = "- No MTC -";
                painter.setPen(QColor{255, 0, 0});
            }

            auto txt_w = painter.fontMetrics().horizontalAdvance(m_String);
            auto txt_h = painter.fontMetrics().height();
            painter.drawText(width() / 2 - txt_w / 2, height() / 2 + txt_h / 3, m_String);

            painter.end();
        }

    private:
        QString m_String;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
        ui->setupUi(this);
        resize(1280, 720); // default size
        setWindowTitle(QStringLiteral(CFXS_PROGRAM_NAME) + " " + QStringLiteral(CFXS_VERSION_STRING));

        m_CameraController = new CameraController(this);

        ui->content->setStyleSheet("border: 1px solid palette(dark);");

        // MIDI Ports

        ui->cb_MTC_Port->addItem(" - none - ", QVariant(-1));

        for (auto& dev : MIDI_Machine::GetInstance()->GetDevices()) {
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

            for (auto& addr : netif.addressEntries()) {
                if (addr.ip().protocol() != QAbstractSocket::IPv4Protocol)
                    continue;

                ui->cb_Netif->addItem(addr.ip().toString() + QStringLiteral(" [") + netif.humanReadableName() + QStringLiteral("]"),
                                      QVariant(netif.name()));
                printf(" - %s [%s]\n", addr.ip().toString().toStdString().c_str(), netif.humanReadableName().toStdString().c_str());
            }
        }

        connect(ui->cb_MTC_Port, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            MIDI_Machine::GetInstance()->SetCurrentDevice(ui->cb_MTC_Port->itemData(index, Qt::UserRole).toInt());
        });

        auto mtcLabel = new MTC_TextWidgetGL;
        ui->mtc_TimeContainer->layout()->addWidget(mtcLabel);

        auto mtcUpdate = new QTimer(this);
        connect(mtcUpdate, &QTimer::timeout, [=]() {
            mtcLabel->repaint();
        });
        mtcUpdate->start(1000 / 30);

        connect(&GamepadServer::instance(), SIGNAL(stateUpdate(GamepadState, int)), this, SLOT(GamepadStateChanged(GamepadState, int)));
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        printf("Close MainWindow\n");
        emit Closed();
        event->accept();
    }

    void MainWindow::GamepadStateChanged(const GamepadState& state, const int& playerId) {
        float accTarget_Pos_Z = map(state.m_lTrigger, 0, 255, 0, -1) + map(state.m_rTrigger, 0, 255, 0, 1);
        float accTarget_Pos_X = map(fabs(state.m_lThumb.xAxis) > GetAxisThreshold() ? state.m_lThumb.xAxis : 0, -32768, 32767, -1, 1);
        float accTarget_Pos_Y = map(fabs(state.m_lThumb.yAxis) > GetAxisThreshold() ? state.m_lThumb.yAxis : 0, -32768, 32767, -1, 1);

        float accTarget_Pan  = map(fabs(state.m_rThumb.xAxis) > GetAxisThreshold() ? state.m_rThumb.xAxis : 0, -32768, 32767, -1, 1);
        float accTarget_Tilt = map(fabs(state.m_rThumb.yAxis) > GetAxisThreshold() ? state.m_rThumb.yAxis : 0, -32768, 32767, -1, 1);

        m_CameraController->Update(accTarget_Pos_X,
                                   accTarget_Pos_Y,
                                   accTarget_Pos_Z,
                                   accTarget_Pan,
                                   accTarget_Tilt,
                                   state.m_lThumb.pressed,
                                   state.m_rThumb.pressed);
    }

} // namespace TCC::UI