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

#include <QtCore/QDebug>

QtGLWidget::QtGLWidget(QWidget *_parent): QGLWidget(_parent),
    m_mayCaptureImage(false),
    m_mayCaptureDepth(false)
{
   m_kinect = new Kinect(); 
   setMinimumSize(QSize(640, 480));
}

void QtGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    if(m_mayCaptureImage)
        paintKinectImage();

    glPushMatrix();
        glBegin(GL_POLYGON);

                /*      This is the top face*/
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, 0.0f, 0.0f);

                /*      This is the front face*/
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, 0.0f);

                /*      This is the right face*/
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);
                glVertex3f(0.0f, 0.0f, -1.0f);

                /*      This is the left face*/
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);

                /*      This is the bottom face*/
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);

                /*      This is the back face*/
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(-1.0f, 0.0f, -1.0f);
                glVertex3f(-1.0f, -1.0f, -1.0f);
                glVertex3f(0.0f, -1.0f, -1.0f);

            glEnd();
        glPopMatrix();
}

void QtGLWidget::paintKinectImage() {
    if(m_kinect->acquireFrame() != XN_STATUS_OK) return;

    XnRGB24Pixel** pImageRef;
    unsigned short *imgSize = m_kinect->getImageMap(pImageRef);
    XnRGB24Pixel* pImage = *pImageRef;

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgSize[0], imgSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);

	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glBegin(GL_QUADS);
        int nXRes = m_kinect->getImageMetaData()->FullXRes();
        int nYRes = m_kinect->getImageMetaData()->FullYRes();

        // upper left
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);
        // upper right
        glTexCoord2f((float)nXRes/(float)imgSize[0], 0);
        glVertex2f(this->size().width(), 0);
        // bottom right
        glTexCoord2f((float)nXRes/(float)imgSize[0], (float)nYRes/(float)imgSize[1]);
        glVertex2f(this->size().width(), this->size().height());
        // bottom left
        glTexCoord2f(0, (float)nYRes/(float)imgSize[1]);
        glVertex2f(0, this->size().height());

	glEnd();
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

