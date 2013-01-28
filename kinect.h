/*
 * kinect.h - Discover connected kinects...
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

#ifndef KINECT_H
#define KINECT_H

#define SAMPLE_XML_PATH_LOCAL "SamplesConfig.xml"
#define MAX_DEPTH 10000

#include <QtCore/QObject>
#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>

using namespace xn;

class Kinect: QObject
{
    Q_OBJECT
public:
    Kinect();
    ~Kinect();
    int getDepthBuffer(float** _outBuffer)  {
        *_outBuffer = m_depthBuffer; 
        return MAX_DEPTH;
    };
    unsigned short* getImageMap(XnRGB24Pixel** _outImage) {
        *_outImage = m_imageMap;
        return m_imgSize;
    };
    DepthMetaData* getDepthMetaData() {
        return &m_depthMD;
    };
    ImageMetaData* getImageMetaData() {
        return &m_imageMD;
    };
    void enumerateErrors();

public slots:
    int startKinectDiscovery();
    int acquireFrame();

private:
	Context m_context;
	ScriptNode m_scriptNode;
	EnumerationErrors m_errors;
    DepthGenerator m_depth;
    ImageGenerator m_image;
    bool hasDepthData;
    DepthMetaData m_depthMD;
    ImageMetaData m_imageMD;
    bool hasImageData;
    
    
    float m_depthBuffer[MAX_DEPTH];
    XnRGB24Pixel* m_imageMap;
    unsigned short m_imgSize[2];

    bool findNode(XnProductionNodeType type, ProductionNode &node);
};

#endif //KINECT_H


