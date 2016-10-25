/*
 * houghDetection.cpp - HoughLine Detection with OpenCV...
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

#include "houghDetection.h"
#include <string.h>
#include <stdio.h>
#include <chrono>

#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <QtGui/QImage>
#include <QtGui/QVector2D>

#define PI 3.141592653589793238462f

ArcoLine::ArcoLine():
    timestamp(QTime::currentTime())
{
}
ArcoLine::~ArcoLine()
{
    delete line;
    delete handPos;
}


OpenCVInterface::OpenCVInterface():
    m_treshold(100),
    m_maxLineGap(20.0),
    m_minLineLength(80.0),
    m_maxBufLength(100),
    m_debugCanny(false),
    m_debugHough(false)
{


}

OpenCVInterface::~OpenCVInterface()
{

}

void OpenCVInterface::loadXnImage(XnRGB24Pixel *_inputImg, QSize& _inputSize, QRect _region)
{
    m_cvMat = Mat(_region.height(), _region.width(), CV_8UC3);
    XnRGB24Pixel *pRow = _inputImg + _region.y() * _inputSize.width() * sizeof(XnRGB24Pixel);
    uchar *matRow = m_cvMat.data;
    for(int y = 0; y < _region.height(); y++) {
        XnRGB24Pixel *pRowCol = pRow + _region.x() * sizeof(XnRGB24Pixel);
        memcpy(matRow , pRowCol, _region.width() * sizeof(XnRGB24Pixel));
        pRow += _inputSize.width();
        matRow += _region.width() * sizeof(XnRGB24Pixel);
    }

}


void OpenCVInterface::findLines() {
    Mat cuttedMat, cvCanMat, grayMat;

    cuttedMat = Mat(m_cvMat, Rect(m_boundingBox.left(), m_boundingBox.top(), m_boundingBox.width(), m_boundingBox.height()));
    cvtColor(cuttedMat, grayMat, CV_RGB2GRAY);
    // First we apply a canny edge filter over the whole image to get a relief of the contrasts in the image
    try {
        Canny(grayMat, cvCanMat, 50, 200, 3);
    } catch(Exception& e) {
        qDebug() << "Sorry exception in Canny caught: " << e.what();
        return;
    }
    if(m_debugCanny) imshow("canny", cvCanMat);

    // next we try to find all straight lines using a hough transformation
    /* HoughLinesP(
            dst: Output of the edge detector. It should be a grayscale image (although in fact it is a binary one)
            lines: A vector that will store the parameters (r,theta) of the detected lines
            rho : The resolution of the parameter r in pixels. We use 1 pixel.
            theta: The resolution of the parameter theta in radians. We use 1 degree (CV_PI/180)
            threshold: The minimum number of intersections to detect a line
            minLinLength: The minimum number of points that can form a line. Lines with less than this number of points are disregarded. )
            maxLineGap: The maximum gap between two points to be considered in the same line.
    */    
    std::vector<Vec4i> lines;
    HoughLinesP( cvCanMat, lines, 1, CV_PI/180, 100, 80, 20);

    if(m_debugHough) {
        // draw the bounding box in which the Arco is beeing expected
        rectangle( m_cvMat, Point(m_boundingBox.left(), m_boundingBox.top()), Point(m_boundingBox.right(), m_boundingBox.bottom()), Scalar(0,255,255), 1, 8);
        char strLeft[10], strRight[10];
        sprintf(strLeft, "%d x %d", m_boundingBox.left(), m_boundingBox.top());
        sprintf(strRight, "%d x %d", m_boundingBox.right(), m_boundingBox.bottom());
        putText( m_cvMat, strLeft,  Point(m_boundingBox.left(), m_boundingBox.top()), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,255,255));
        putText( m_cvMat, strRight,  Point(m_boundingBox.right(), m_boundingBox.bottom()), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,255,255));
    }



    QLine *closestLine = NULL;
    int minDistanceToHand = 0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        
        QLine* _line = new QLine(lines[i][0] + m_boundingBox.left(), lines[i][1] + m_boundingBox.top(), lines[i][2] + m_boundingBox.left(), lines[i][3] + m_boundingBox.top());
        if(m_debugHough) {// DEBUGGING IMAGE
            line( m_cvMat, Point(_line->x1(), _line->y1()), Point(_line->x2(), _line->y2()), Scalar(60,200,255), 1, 8 );
        }
        int distanceToHand = QVector2D(_line->x2() - m_boundingBox.right(), _line->y2() - m_boundingBox.bottom()).length();
        if(closestLine == NULL || distanceToHand < minDistanceToHand) {
            closestLine = _line;
            minDistanceToHand = distanceToHand;
        } else {
            delete _line;
        }
    }

    if(closestLine != NULL) {
        QLine& _line = *closestLine;
        QVector2D vec(_line.x1() - _line.x2(), _line.y1() - _line.y2());
        QVector2D horizontal(-1,0);
        qreal angle = qAcos(QVector2D::dotProduct(horizontal, vec)/(horizontal.length() * vec.length()));
        int degAngle = qFloor(angle * (180.0f/CV_PI));

            putText( m_cvMat, QString::number(degAngle).toLatin1().data(),  Point(_line.x1(), _line.y1()), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,255));
            line( m_cvMat, Point(_line.x1(), _line.y1()), Point(_line.x2(), _line.y2()), Scalar(0,0,255), 3, 8 );
        if(m_debugHough) { // DEBUGGING IMAGE
            putText( m_cvMat, QString::number(degAngle).toLatin1().data(),  Point(_line.x1(), _line.y1()), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,255));
            line( m_cvMat, Point(_line.x1(), _line.y1()), Point(_line.x2(), _line.y2()), Scalar(0,0,255), 3, 8 );
        }

        ArcoLine * arco = new ArcoLine();
        arco->length = vec.length();
        arco->radAngle = angle;
        arco->degAngle = angle * (180.0f/CV_PI);
        arco->line = closestLine;
        arco->handPos = new QPoint(m_boundingBox.right(), m_boundingBox.bottom());
        m_arcoBuffer.enqueue(arco);
        if(m_arcoBuffer.length() > m_maxBufLength) {
            delete m_arcoBuffer.dequeue();
        }
    }
    if(m_debugHough) imshow("lines", m_cvMat);
}

QImage* OpenCVInterface::convertToQImage()
{
    QImage *img = new QImage((uchar *) m_cvMat.data, m_cvMat.size().width, m_cvMat.size().height, QImage::Format_RGB888);
    return img;
}


void OpenCVInterface::restrictBoundingBox(const QRect& _box)
{
    m_boundingBox.setCoords(_box.topLeft().x(), _box.topLeft().y(), _box.bottomRight().x(), _box.bottomRight().y());
}
QQueue<ArcoLine*>& OpenCVInterface::getArcoHistory()
{
    return  m_arcoBuffer;
}


