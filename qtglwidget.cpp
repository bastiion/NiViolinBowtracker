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

QtGLWidget::QtGLWidget(QWidget *_parent): QGLWidget(_parent),
    m_mayCaptureImage(false),
    m_mayCaptureDepth(false),
    m_cameraImage(NULL),
    m_frameCounter(0)
{
   m_kinect = new Kinect(); 
   setMinimumSize(QSize(640, 480));
   m_timer = new QTimer(this);
   connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
   setAutoFillBackground(false);
   openCVif = new OpenCVInterface();
}

void QtGLWidget::startCapture()
{
    m_timer->start(1000/20);
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
    m_frameCounter++;
    if(m_kinect->acquireFrame() != XN_STATUS_OK) return;
    //delete m_cameraImage;
    m_cameraImage = m_kinect->getCameraQImage();
    QPainter painter;
    painter.begin(this);
    if(m_cameraImage != NULL) {
        painter.drawImage(0,0,*m_cameraImage);
        painter.setPen(Qt::yellow);
        painter.drawEllipse(QRect(*(m_kinect->getHandPos()), QSize(10,10)));
        testOpenCV();
    }
    painter.end();

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

    glPushMatrix();
        glBegin(GL_POLYGON);

                //     This is the top face
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, 0.0f, 0.0f);

                //      This is the front face
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, 0.0f);

                //      This is the right face
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);
                glVertex3f(0.0f, 0.0f, -1.0f);

                //      This is the left face
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);

                //      This is the bottom face
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);

                //      This is the back face
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);

            glEnd();
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
    XnRGB24Pixel* pImageRef = NULL;
    QSize imgSize = m_kinect->getTextureMap(&pImageRef);
    openCVif->loadXnImage(pImageRef, imgSize, QRect(0,0, 640,480));
    openCVif->findLines();
}

