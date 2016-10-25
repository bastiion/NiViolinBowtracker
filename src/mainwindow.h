/*
 * mainwindow.h - Mainwindow to embed the OpenGL Window...
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>
#include "qtglwidget.h"
#include "midisink.h"

class QListWidget;
class QListWidgetItem;
class QSpinBox;

class MainWindow: public QWidget
{
    Q_OBJECT
public:
        MainWindow();

private:
        QtGLWidget *glw;
        MidiSink * m_midiSink;

        QListWidget * midiConnectionListW;
        QSpinBox * speedControlerNumSpin;
        QSpinBox * speedControlerValueSpin;
public slots:
        void refreshMidiConnections();
        void connectMidi(QListWidgetItem* item);

        void upBowStart(float _velo, float _acc);
        void downBowStart(float _velo, float _acc);
        void bowStop();
        void changeNoteKey(int _angle);
        void changeNoteParameter(float _velo, float _asc);
        void sendTestControlParameter();


};


#endif //MAINWINDOW_H

