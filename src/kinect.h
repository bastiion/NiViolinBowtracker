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
#define MAX_NUM_USERS 15

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtGui/QVector3D>
#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
class QImage;

using namespace xn;

class Kinect: public  QObject
{
    Q_OBJECT
public:
    Kinect();
    ~Kinect();
    int getDepthBuffer(float** _outBuffer)  {
        *_outBuffer = m_depthHisto; 
        return MAX_DEPTH;
    };
    QSize getTextureMap(XnRGB24Pixel** _outImage) {
        *_outImage = m_textureMap;
        return m_texSize;
    };

    QImage *getCameraQImage();
    QPoint& getHandPos() {
        return   m_rightHandP; 
    };

    DepthMetaData* getDepthMetaData() {
        return &m_depthMD;
    };
    ImageMetaData* getImageMetaData() {
        return &m_imageMD;
    };
    void enumerateErrors();
    void startHandTracking(const XnPoint3D* _pos);
    void stopHandTracking();
    void startGestureTracking();
    void stopGestureTracking();
    void sendHandLost();
    void sendHandDetected();

    void updateHandPosition(const XnPoint3D* _pos);
    
    bool m_applyDepthFilter;
    bool isHandTracking;

public slots:
    int startKinectDiscovery();
    int acquireFrame();
    void detectUsers();

signals:
    void handLost();
    void handDetected();
        
private:
	Context m_context;
	ScriptNode m_scriptNode;
	EnumerationErrors m_errors;
    DepthGenerator m_depth;
    ImageGenerator m_image;
    UserGenerator m_user;
    HandsGenerator m_hands;
    GestureGenerator m_gesture;

    XnCallbackHandle hGestureIntermediateStageCompleted, hGestureProgress, hGestureReadyForNextIntermediateStage;


    DepthMetaData m_depthMD;
    ImageMetaData m_imageMD;
    bool hasDepthData;
    bool hasImageData;
    QSize m_texSize;
    QPoint m_rightHandP; 
    QVector3D *m_rightHandVec; 
    
    float m_depthHisto[MAX_DEPTH];
    XnRGB24Pixel* m_textureMap;
    QImage *m_qImageMap;

    QString m_startGesture;




//    void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& , XnUserID nId, void*);
//    void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& , XnUserID nId, void*);
//    void XN_CALLBACK_TYPE User_CalibrationComplete(xn::SkeletonCapability& , XnUserID nId, XnCalibrationStatus eStatus, void*);
    bool findNode(XnProductionNodeType type, ProductionNode &node);
};

#endif //KINECT_H


