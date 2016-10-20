/**
* This file is part of Bowtracker.
* Copyright (C) Sebastian Tilsch, 2016
*
* Bowtracker is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Bowtracker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Bowtracker.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef MIDISINK_H
#define MIDISINK_H

#include "controlsink.h"
#include "QMidiOut.h"
#include "QMidiFile.h"
#include <QtCore/QMap>

class MidiSink: public ControlSink {
public:
    MidiSink();
    void bowStart(BowDirection _direction) ;
    void bowEnd(BowDirection _direction) ;
    void bow(float acceleration, float speed) ;
    const QMap<QString,QString> devices();
    bool connect(const QString& device);
private:
    QMidiOut m_midi;

};

#endif //MIDISINK_H
