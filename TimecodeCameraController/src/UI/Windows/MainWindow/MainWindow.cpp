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