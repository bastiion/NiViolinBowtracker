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

#ifndef CONTROLSINK_H
#define CONTROLSINK_H

class ControlSink  {
public:
    enum BowDirection {
        UpBow,
        DownBow
    };
    ControlSink() {};
    void bowStart(BowDirection _direction) {};
    void bowEnd(BowDirection _direction) {};
    void bow(float acceleration, float speed) {};



};

#endif
