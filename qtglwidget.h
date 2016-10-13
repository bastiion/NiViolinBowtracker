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
class QTimer;
class OpenCVInterface;

class QtGLWidget: public QGLWidget
{
    Q_OBJECT
public:
    QtGLWidget(QWidget *_parent);
    ~QtGLWidget();

public slots:
    void startKinect();
    void startCapture();
    void testOpenCV();
    void setHoughTreshold(int _treshold);
    void setMaxLineGap(double _gap);
    void setMinLineLength(double _length);
    void toggleDepthFilter(int _state);
    void demandGesture();
    void setTrackingOn();
    void toggleDebugCanny(int _state);
    void toggleDebugHough(int _state);

protected:
    void initializeGL();
    void paintGL();
    void paintKinectImage();
    void paintEvent(QPaintEvent *_event);

private:
    Kinect* m_kinect;
    bool m_mayCaptureImage;
    bool m_mayCaptureDepth;
    QTimer *m_timer;
    QImage *m_cameraImage;
    int m_frameCounter;
    OpenCVInterface *openCVif;
    bool m_isTrackingHand;
};


#endif //QTGLWIDGET_H
