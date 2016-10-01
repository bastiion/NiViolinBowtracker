/*
 * imagehelper.cpp - OpenCV Operations...
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

#include "imagehelper.h"
#include <QtCore/QDebug>

IplImage* ImageHelper::convertToIplImage(const QImage& _img) {
    int width = _img.width();
    int height = _img.height();
 
    // Creates a iplImage with 3 channels
    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    char * imgBuffer = img->imageData;
 
    //Remove alpha channel
    int jump = (_img.hasAlphaChannel()) ? 4 : 3;
 
    for (int y=0;y<img->height;y++){
      QByteArray a((const char*)_img.scanLine(y), _img.bytesPerLine());
      for (int i=0; i<a.size(); i+=jump){
          //Swap from RGB to BGR
          imgBuffer[2] = a[i];
          imgBuffer[1] = a[i+1];
          imgBuffer[0] = a[i+2];
          imgBuffer+=3;
      }
    }
    return img;
}

QImage* ImageHelper::convertToQImage(const IplImage* _img) {
    int height = _img->height;
    int width = _img->width;
 
    if  (_img->depth == IPL_DEPTH_8U && _img->nChannels == 3)
    {
        const uchar *_imgBuffer = (const uchar*)_img->imageData;
        QImage *img = new QImage(_imgBuffer, width, height, QImage::Format_RGB888);
        return new QImage(img->rgbSwapped());
    } else if  (_img->depth == IPL_DEPTH_8U && _img->nChannels == 1){
        const uchar *_imgBuffer = (const uchar*)_img->imageData;
        QImage *img = new QImage(_imgBuffer, width, height, QImage::Format_Indexed8);
     
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++){
            colorTable.push_back(qRgb(i, i, i));
        }
        img->setColorTable(colorTable);
        return img;
    }else{
        qWarning() << "Image cannot be converted.";
        return NULL;
    }
}

