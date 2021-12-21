// ---------------------------------------------------------------------
// CFXS TImecodeCameraController <https://github.com/CFXS/TImecodeCameraController>
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
#include <QDesktopServices>
#include <QFileDialog>
#include <QNetworkInterface>

#include <Core/MIDI_Machine.hpp>

namespace TCC::UI {

    //////////////////////////////////////////////////////////////////////////////////////////////////

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
        ui->setupUi(this);
        resize(1280, 720); // default size
        setWindowTitle(QStringLiteral(CFXS_PROGRAM_NAME) + " " + QStringLiteral(CFXS_VERSION_STRING));

        auto menuFont = ui->menubar->font();
        menuFont.setPointSize(menuFont.pointSize() + 1);
        ui->menubar->setFont(menuFont);

        for (auto action : ui->menubar->actions()) {
            auto actionFont = action->font();
            actionFont.setPointSize(actionFont.pointSize() + 1);
            action->setFont(actionFont);
        }

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

        RegisterActions();
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        printf("Close MainWindow\n");
        emit Closed();
        event->accept();
    }

    ///////////////////////////////////////////////////////////////////

    void MainWindow::RegisterActions() {
        // Exit
        connect(ui->actionExit, &QAction::triggered, this, [=]() {
            this->close();
        });
    }

} // namespace TCC::UI