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
#include "TCC_Application.hpp"
#include "MIDI_Machine.hpp"

#include <QString>
#include <QStringLiteral>
#include <QStandardPaths>
#include <QFontDatabase>
#include <QFile>

namespace TCC {

    TCC_Application::TCC_Application(int argc, char **argv) : Application(argc, argv, CFXS_PROGRAM_NAME) {
        qApp->setWindowIcon(QPixmap(":/HWD_Icon.png"));
    }

    void TCC_Application::OnCreate() {
        QFontDatabase::addApplicationFont(":/Font/Code");

        MIDI_Machine::GetInstance();
    }

    void TCC_Application::OnDestroy() {
    }

} // namespace TCC