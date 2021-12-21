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
#pragma once

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace TCC::UI {

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        /// \param rawStateToLoad state data received from SaveState signal on window close
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        /// connect actions to functions
        void RegisterActions();

    signals:
        void Closed();

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        std::unique_ptr<Ui::MainWindow> ui;
    };

} // namespace TCC::UI