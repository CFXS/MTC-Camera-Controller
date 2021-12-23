// ---------------------------------------------------------------------
// CFXS MTC Camera Controller <https://github.com/CFXS/MTC-Camera-Controller>
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

#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <Core/MIDI_Machine.hpp>
#include <QDateTime>

namespace TCC {

    class MTC_TextWidgetGL : public QOpenGLWidget {
    public:
        MTC_TextWidgetGL(const MIDI_Machine* midi) : m_MIDI(midi) {
        }

        void SetString(QString str) {
            m_String = str;
        }

        void paintEvent(QPaintEvent* e) override {
            QPainter painter(this);

            auto font = QFont("Consolas", 20);
            font.setStyleStrategy(QFont::PreferQuality);
            painter.setFont(font);
            painter.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

            painter.fillRect(0, 0, width(), height(), QBrush(qApp->palette().color(QPalette::Window)));

            QPainterPath path;
            path.addRoundedRect(QRect(0, 0, width(), height()), 2, 2);
            painter.fillPath(path, QBrush(QColor(16, 16, 16)));

            if (m_MIDI->HaveSync()) {
                m_String = m_MIDI->GetTimeString();
                if (m_MIDI->IsTimecodeActive()) {
                    painter.setPen(QColor{0, 255, 0});
                } else {
                    painter.setPen(QColor{160, 160, 160});
                }
            } else {
                painter.setPen(QColor{255, 0, 0});
                m_String = "No MTC";
            }

            auto txt_w = painter.fontMetrics().horizontalAdvance(m_String);
            auto txt_h = painter.fontMetrics().height();
            painter.drawText(width() / 2 - txt_w / 2, height() / 2 + txt_h / 3, m_String);

            painter.end();
        }

    private:
        QString m_String;
        const MIDI_Machine* m_MIDI;
    };

} // namespace TCC