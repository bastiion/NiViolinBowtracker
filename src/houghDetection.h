/*
 * houghDetection.h - HoughLine Detection with OpenCV...
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

#ifndef HOUGHDETECTION_H
#define HOUGHDETECTION_H

#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QList>
#include <QtCore/QLine>
#include <QtCore/QQueue>
#include <QtCore/QTime>

#include <XnOpenNI.h>
#include "opencv2/opencv.hpp"
class QImage;

using namespace cv;

class ArcoLine {
public:
    ArcoLine();
    ~ArcoLine();
    float radAngle;
    float degAngle;
    QLine* line;
    QPoint* handPos;
    int length;
    QTime timestamp;
};

class OpenCVInterface {
public:
    OpenCVInterface();
    ~OpenCVInterface();
    void loadXnImage(XnRGB24Pixel *_input, QSize& _inputSize, QRect _region);
    QImage* convertToQImage();
    void findLines();
    void restrictBoundingBox(const QRect& _box);
    QQueue<ArcoLine*>& getArcoHistory();

    int m_treshold;
    double m_maxLineGap;
    double m_minLineLength;
    bool m_debugCanny;
    bool m_debugHough;
    
private:
    QRect m_boundingBox; 
    Mat m_cvMat;
    QQueue<ArcoLine*> m_arcoBuffer;
    int m_maxBufLength;
    
   
};

#endif //HOUGHDETECTION_H
