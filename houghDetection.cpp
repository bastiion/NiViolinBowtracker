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
#include <QtCore/QDebug>
#include <QtGui/QImage>

OpenCVInterface::OpenCVInterface()
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
    Mat cvCanMat, grayMat;
    cvtColor(m_cvMat, grayMat, CV_RGB2GRAY);
    try {
        Canny(grayMat, cvCanMat, 50, 200, 3);
    } catch(Exception& e) {
        qDebug() << "Sorry exception in Canny caught: " << e.what();
        return;
    }
    imshow("canny", cvCanMat);
    vector<Vec4i> lines;
    HoughLinesP( cvCanMat, lines, 1, CV_PI/180, 100, 80, 10);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( m_cvMat, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }
    imshow("lines", m_cvMat);
}

QImage* OpenCVInterface::convertToQImage()
{
    QImage *img = new QImage((uchar *) m_cvMat.data, m_cvMat.size().width, m_cvMat.size().height, QImage::Format_RGB888);
    return img;
}




