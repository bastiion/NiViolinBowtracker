/*
 * qglwidget.h - Widget for displaying opengl...
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

#ifndef QTGLWIDGET_H
#define QTGLWIDGET_H

#include <QtOpenGL/QGLWidget>
class Kinect;

class QtGLWidget: public QGLWidget
{
    Q_OBJECT
public:
    QtGLWidget(QWidget *_parent);

public slots:
    void startKinect();

protected:
    void paintGL();
    void paintKinectImage();

private:
    Kinect* m_kinect;
    bool m_mayCaptureImage;
    bool m_mayCaptureDepth;

};


#endif //QTGLWIDGET_H
