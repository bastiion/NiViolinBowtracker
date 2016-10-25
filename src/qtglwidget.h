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
#include <QtCore/QQueue>
#include <QtCore/QTime>

class Kinect;
class QTimer;
class OpenCVInterface;

class ArcoVelocity {
    public:
        ArcoVelocity(float _velo, float _acc);
        float velocityPixPerMS;
        float accelerationPixPerMS2;
        QTime timestamp;
};

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
    void toggleMeanAverage(int _state);
    
signals:
    void upBowStart(float velocity, float acceleration);
    void downBowStart(float velocity, float acceleration);
    void bowing(float velocity, float acceleration);
    void bowStop();
    void angleChange(int angle);

protected:
    void initializeGL();
    void paintGL();
    void paintKinectImage();
    void paintEvent(QPaintEvent *_event);

private:
    enum BowState {
        Up,
        Down,
        Stop
    };
    Kinect* m_kinect;
    bool m_mayCaptureImage;
    bool m_mayCaptureDepth;
    bool m_useMeanVelocity;
    QTimer *m_timer;
    QImage *m_cameraImage;
    int m_frameCounter;
    OpenCVInterface *openCVif;
    bool m_isTrackingHand;
    BowState m_bowState;
    int angle;

    void changeBowState(BowState _state, ArcoVelocity* _velocity);
    bool calculateArcoProperties();

    
    //some thresholds
    int m_maxVelocityBufLength;
    float m_velocityStopThreshold;


    QQueue<ArcoVelocity*> velocityHistory;
};


#endif //QTGLWIDGET_H
