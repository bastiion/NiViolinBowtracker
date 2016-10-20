/*
 * qtglwidget.cpp - Widget to display opengl
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

#include "qtglwidget.h"
#include "kinect.h"
#include "houghDetection.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QList>
#include <QtGui/QVector2D>


ArcoVelocity::ArcoVelocity(float _velo = 0.0f, float _acc = 0.0f):
    timestamp(QTime::currentTime()),
    velocityPixPerMS(_velo),
    accelerationPixPerMS2(_acc)
{
}

QtGLWidget::QtGLWidget(QWidget *_parent): QGLWidget(_parent),
    m_mayCaptureImage(false),
    m_mayCaptureDepth(false),
    m_cameraImage(NULL),
    m_frameCounter(0),
    m_isTrackingHand(false),
    m_useMeanVelocity(true),
    m_maxVelocityBufLength(100),
    m_velocityStopThreshold(0.5)
                           
{
   m_kinect = new Kinect(); 
   connect(m_kinect, SIGNAL(handLost()), this, SLOT(demandGesture()));
   connect(m_kinect, SIGNAL(handDetected()), this, SLOT((setTrackingOn())));

   setMinimumSize(QSize(640, 480));
   m_timer = new QTimer(this);
   connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
   setAutoFillBackground(false);
   openCVif = new OpenCVInterface();
}

QtGLWidget::~QtGLWidget()
{
    // m_timer->stop();
    // delete m_timer;
    // delete openCVif;
    // delete m_kinect;
}

void QtGLWidget::startCapture()
{
    m_timer->start(1000/20);
}

void QtGLWidget::demandGesture()
{
    m_isTrackingHand = false;
}

void QtGLWidget::setTrackingOn()
{
    m_isTrackingHand = true;
}



void QtGLWidget::initializeGL()
{
    /* glEnable(GL_DEPTH_TEST);
     glEnable(GL_CULL_FACE);
 #ifndef QT_OPENGL_ES_2
     glEnable(GL_TEXTURE_2D);
 #endif
*/
}

void QtGLWidget::paintEvent(QPaintEvent *_event) {
    //m_frameCounter++;
    if(m_kinect->acquireFrame() != XN_STATUS_OK) return;
    //delete m_cameraImage;
    m_cameraImage = m_kinect->getCameraQImage();
    ArcoLine* arco = NULL; 
    
    if(!openCVif->getArcoHistory().isEmpty()) {
        arco = openCVif->getArcoHistory().last();
    }
    QPainter painter;
    QPen bowPen(Qt::red);
    bowPen.setWidth(5);
    painter.begin(this);
    if(m_cameraImage != NULL) {
        painter.drawImage(0,0,*m_cameraImage);
        if(arco != NULL) {
            painter.setPen(Qt::yellow);
            painter.drawEllipse(QRect((m_kinect->getHandPos()), QSize(10,10)));
            painter.setPen(bowPen);
            painter.drawLine(* arco->line);
        }
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Helvetica", 14));
        if(!m_kinect->isHandTracking) {
            painter.drawText(QPoint(50,50), tr("Please raise your bow-hand to detect your hand"));
        } else {
            painter.drawText(QPoint(50,50), tr("Hand Detected"));
        }
        if(!velocityHistory.isEmpty()) {
            painter.drawText(QPoint(50,80), tr("Bow Speed: ") + velocityHistory.head()->velocityPixPerMS + tr(" px/ms"));
            painter.drawText(QPoint(50,100), tr("Bow Accelaration: ") + velocityHistory.head()->accelerationPixPerMS2 + tr(" px/m²"));
        }
        

        testOpenCV();
    }
    painter.end();

}
bool QtGLWidget::calculateArcoProperties()
{


    QQueue<ArcoLine*>& arcBuf = openCVif->getArcoHistory();
    int size = arcBuf.size();
    if(size < 3)
        return false;
    int num = size > 10 ? 10 : size;
    QList<float> velocityPixPerMSList;
    float velocityPixPerMSSum  = 0.0f;
    
    for(int i = num-1;i >= 1; i--) {
        ArcoLine* arco = arcBuf.at(i);
        ArcoLine* arcoNext = arcBuf.at(i-1);
        QVector2D movementVector(arcoNext->line->x2() - arco->line->x2(), arcoNext->line->y2() - arco->line->y2());
        float mvmtLength = movementVector.length();
        const int durationMS = arcoNext->timestamp.msecsTo(arco->timestamp);
        const float velocityPixPerMS = ((float)mvmtLength)/durationMS;
        velocityPixPerMSList += velocityPixPerMS;
        velocityPixPerMSSum + velocityPixPerMS;
    }
    float averageVelocityPixPerMS = 0.0f;
    if(m_useMeanVelocity) {
        qSort(velocityPixPerMSList.begin(), velocityPixPerMSList.end());
        averageVelocityPixPerMS = velocityPixPerMSList[qRound(velocityPixPerMSList.length()/2.0f)];
    } else {
        averageVelocityPixPerMS = velocityPixPerMSSum/ velocityPixPerMSList.length();
    }

    // if there are already velocity values, calculate the accelaration
    if(!velocityHistory.isEmpty()) {
        ArcoVelocity* lastVelo = velocityHistory.head();
        ArcoVelocity* newVelo = new ArcoVelocity(averageVelocityPixPerMS);
        velocityHistory.enqueue(newVelo);
        newVelo->accelerationPixPerMS2 = (averageVelocityPixPerMS - velocityHistory.head()->velocityPixPerMS) / (float) lastVelo->timestamp.msecsTo(newVelo->timestamp);
    } else {
        velocityHistory.enqueue(new ArcoVelocity(averageVelocityPixPerMS));
    }
    if(velocityHistory.length() > m_maxVelocityBufLength) {
        delete velocityHistory.dequeue();
    }


    //now that we have all data, lets see, whether we ned to emit some signals

   

    return true;

}

void QtGLWidget::paintGL()
{
   /* qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, this->size().width(), this->size().height(), 0, -1.0, 1.0);
    //glOrtho(0, 1, 1, 0, -1.0, 1.0);
    //if(m_mayCaptureImage)
    //    paintKinectImage();
    
    glPopMatrix();

        */
}

void QtGLWidget::paintKinectImage() {
    if(m_kinect->acquireFrame() != XN_STATUS_OK) return;
    delete m_cameraImage;
    m_cameraImage = m_kinect->getCameraQImage();

    XnRGB24Pixel** pImageRef;
    QSize imgSize = m_kinect->getTextureMap(pImageRef);
    XnRGB24Pixel* pImage = *pImageRef;
    
    qDebug() << "Size " << imgSize.width() << " x " << imgSize.height();
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgSize.width(), imgSize.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glBegin(GL_QUADS);
        int nXRes = m_kinect->getImageMetaData()->FullXRes();
        int nYRes = m_kinect->getImageMetaData()->FullYRes();

        // upper left
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);
        // upper right
        glTexCoord2f((float)nXRes/(float)imgSize.width(), 0);
        glVertex2f(this->size().width(), 0);
        // bottom right
        glTexCoord2f((float)nXRes/(float)imgSize.width(), (float)nYRes/(float)imgSize.height());
        glVertex2f(this->size().width(), this->size().height());
        // bottom left
        glTexCoord2f(0, (float)nYRes/(float)imgSize.height());
        glVertex2f(0, this->size().height());

	glEnd();
    qDebug() << "frame end";
    
}

void QtGLWidget::startKinect() {
   int nRetVal = m_kinect->startKinectDiscovery(); 
   if(nRetVal == XN_STATUS_OK)
        m_mayCaptureImage = true;
   else {
       qDebug() << "Sorry, could not start kinect: " << xnGetStatusString(nRetVal);
       m_kinect->enumerateErrors();
   }

}

void QtGLWidget::testOpenCV() {
    int boundingWidth = 300,
        boundingHeight = 270;
    XnRGB24Pixel* pImageRef = NULL;
    QSize imgSize = m_kinect->getTextureMap(&pImageRef);
    openCVif->loadXnImage(pImageRef, imgSize, QRect(0,0, 640,480));
    QPoint topLeft(0,0);
    QPoint bottomRight = QPoint(m_kinect->getHandPos());
    
    bottomRight.setX(bottomRight.x() + 10);
    bottomRight.setY(bottomRight.y() + 50);


    int boundingLeft = m_kinect->getHandPos().x() - boundingWidth,
        boundingTop = m_kinect->getHandPos().y() - boundingHeight;
    if(boundingLeft > 0) topLeft.setX(boundingLeft);
    if(boundingTop > 0) topLeft.setY(boundingTop);

    openCVif->restrictBoundingBox(QRect(topLeft, bottomRight));
    openCVif->findLines();
}

void QtGLWidget::setHoughTreshold(int _treshold)
{
    openCVif->m_treshold = _treshold;
}
void QtGLWidget::setMaxLineGap(double _gap)
{
    openCVif->m_maxLineGap = _gap;
}
void QtGLWidget::setMinLineLength(double _length)
{
    openCVif->m_minLineLength = _length;
}

void QtGLWidget::toggleDepthFilter(int _state)
{
    m_kinect->m_applyDepthFilter = _state == Qt::Checked;
}

void QtGLWidget::toggleDebugCanny(int _state)
{
    openCVif->m_debugCanny = _state == Qt::Checked;
}

void QtGLWidget::toggleDebugHough(int _state)
{
    openCVif->m_debugHough = _state == Qt::Checked;
}

void QtGLWidget::toggleMeanAverage(int _state)
{
    m_useMeanVelocity = _state == Qt::Checked;
}

