/*
 * kinect.cpp - Discover connected kinects...
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

#include "kinect.h"
#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtGui/QColor>


XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

Kinect::Kinect():
    hasDepthData(false),
    hasImageData(false),
    m_qImageMap(NULL),
    m_startGesture("RaiseHand"),
    isHandTracking(false),
    m_applyDepthFilter(true)
{
    m_texSize = QSize();
    m_rightHandVec = new QVector3D();
}

Kinect::~Kinect()
{
   delete m_textureMap;
   delete m_rightHandVec;
   // delete[] m_depthHisto;
   //m_user.Release();
   m_depth.Release();
   m_image.Release();
   m_gesture.Release();
   m_hands.Release();
   m_scriptNode.Release();
   m_context.Release();
}

Kinect* castCookie(void* _cookie) {
    return static_cast<Kinect*>(_cookie);
}

void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& _user, XnUserID nId, void* /*pCookie*/)
{
    _user.GetSkeletonCap().RequestCalibration(nId, TRUE);
    qDebug() << "Detected User";
}

void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& _user, XnUserID nId, void* /*pCookie*/)
{
    qDebug() << "Lost User";
}

void XN_CALLBACK_TYPE User_CalibrationStart(xn::SkeletonCapability& /*SkeletonCapability*/, XnUserID nId, void* /*pCookie*/)
{
    qDebug() << "Calibration started";
}

void XN_CALLBACK_TYPE User_CalibrationComplete(xn::SkeletonCapability& _skelCap, XnUserID nId, XnCalibrationStatus eStatus, void* /*pCookie*/)
{

    qDebug() << "Calibration complete?" << nId;		
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        qDebug() << "Calibration complete, start tracking user " << nId;		
        _skelCap.StartTracking(nId);
    }
}

void XN_CALLBACK_TYPE Gesture_IntermediateStageComplete(xn::GestureGenerator& _gc, const XnChar* _strGesture,  const XnPoint3D* _pos, void* _cookie) {
    qDebug() << "Intermediate Stage complete " << _strGesture;
}
void XN_CALLBACK_TYPE Gesture_ReadyForNextIntermediateStage(xn::GestureGenerator& _gc, const XnChar* _strGesture,  const XnPoint3D* _pos, void* _cookie) {
    qDebug() << "Ready for next Intermediate Stage " << _strGesture;
}
void XN_CALLBACK_TYPE Gesture_Progress(xn::GestureGenerator& _gc, const XnChar* _strGesture,  const XnPoint3D* _pos, XnFloat _progress, void* _cookie) {
    qDebug() << "Progress (" << _strGesture << ") " << _progress;
}

void XN_CALLBACK_TYPE Gesture_Recognized(xn::GestureGenerator& _gc, const XnChar* _strGesture,  const XnPoint3D* _idPos, const XnPoint3D* _pos, void* _cookie) {
    qDebug() << "Recognized " << _strGesture;
    Kinect* _this = castCookie(_cookie);
    _this->stopGestureTracking();
    _this->startHandTracking(_pos);
    _this->sendHandDetected();

}

void  XN_CALLBACK_TYPE Hands_Create2(HandsGenerator& _hg, XnUserID _user, const XnPoint3D* _pos, XnFloat _time, void* _cookie)
{
    qDebug() << "Hand Created ";
}
void XN_CALLBACK_TYPE Hands_Update2(HandsGenerator& _hg, XnUserID _user, const XnPoint3D* _pos, XnFloat _time, void* _cookie)
{
    //qDebug() << "Hand Updated";
    Kinect* _this = castCookie(_cookie);
    _this->updateHandPosition(_pos);

}
void XN_CALLBACK_TYPE Hands_Destroy2(HandsGenerator& _hg, XnUserID _user, XnFloat _time, void* _cookie)
{
    qDebug() << "Hand Destroyed";
    Kinect* _this = castCookie(_cookie);
    _this->stopHandTracking();
    _this->startGestureTracking();
    _this->sendHandLost();

}

void Kinect::startHandTracking(const XnPoint3D* _pos)
{
     if(!isHandTracking) {
        m_hands.StartTracking(*_pos);
        isHandTracking = true;
     }
}
void Kinect::stopHandTracking()
{

    isHandTracking = false;
    //m_hands.StopTrackingAll();
}

void Kinect::sendHandLost()
{
    emit handLost();
}
void Kinect::sendHandDetected()
{
    emit handDetected();
}


void Kinect::startGestureTracking()
{
    m_gesture.AddGesture(m_startGesture.toLatin1(), NULL);

    m_gesture.RegisterToGestureIntermediateStageCompleted(Gesture_IntermediateStageComplete, this, hGestureIntermediateStageCompleted);
    m_gesture.RegisterToGestureReadyForNextIntermediateStage(Gesture_ReadyForNextIntermediateStage, this, hGestureReadyForNextIntermediateStage);
    m_gesture.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Progress, this, hGestureProgress);
       

}
void Kinect::stopGestureTracking()
{
    m_gesture.RemoveGesture(m_startGesture.toLatin1());
    m_gesture.UnregisterFromGestureIntermediateStageCompleted(hGestureIntermediateStageCompleted);
    m_gesture.UnregisterFromGestureReadyForNextIntermediateStageCallbacks(hGestureReadyForNextIntermediateStage);
    m_gesture.UnregisterGestureCallbacks(hGestureProgress);
}


int Kinect::startKinectDiscovery()
{

	XnStatus nRetVal = XN_STATUS_OK;
	const char *fn = NULL;
    if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
	else {
		qDebug() << "Could not find " << SAMPLE_XML_PATH_LOCAL << " Aborting.";
		return XN_STATUS_ERROR;
	}

    nRetVal = m_context.InitFromXmlFile(fn, m_scriptNode, &m_errors);
    if(nRetVal != XN_STATUS_OK){
        qDebug() << "Could not init Context from Sample Configuration file "; return nRetVal; 
    }

    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_DEPTH, m_depth);
    if(nRetVal != XN_STATUS_OK){ 
        qDebug() << "Could not find a Depth Node "; 
    } else {
        hasDepthData = true;
        m_depth.GetMetaData(m_depthMD);

    }

    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_IMAGE, m_image);
    if(nRetVal != XN_STATUS_OK){ 
        qDebug() << "Could not find an Image Node"; 
    } else {
        m_image.GetMetaData(m_imageMD);
        m_texSize.setWidth((((m_imageMD.FullXRes()-1) / 512) + 1) * 512);
        m_texSize.setHeight((((m_imageMD.FullYRes()-1) / 512) + 1) * 512);
        m_textureMap = (XnRGB24Pixel*)malloc(m_texSize.width() * m_texSize.height() * sizeof(XnRGB24Pixel));
        if (m_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24) {
            qDebug() << "Pixel Format not in RGB24. Unsupported PixelFormat! " << m_imageMD.PixelFormat();
        } else {
            hasImageData = true;
        }
    }

        

    // nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_USER, m_user);   
    // if(nRetVal != XN_STATUS_OK){ 
    //     qDebug() << "Could not find an User Node"; /*return nRetVal;*/ 
    // } else {
    //     bool skeletonCapabilities = true;
    //     if (!m_user.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
    //         qDebug() << "Skeletontracking not supported by user node";
    //     }
    //     if(m_user.GetSkeletonCap().NeedPoseForCalibration()) {
    //         qDebug() << "Sorry.I cannot do Pose Calibration currently";
    //         skeletonCapabilities = false;
    //     }
    //     if(skeletonCapabilities) {
    //         XnCallbackHandle hUserCallbacks, hCalibrationComplete, hCalibrationStart;
    //         nRetVal = m_user.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    //         nRetVal = m_user.GetSkeletonCap().RegisterToCalibrationStart(User_CalibrationStart, NULL, hCalibrationStart);
    //         nRetVal = m_user.GetSkeletonCap().RegisterToCalibrationComplete(User_CalibrationComplete, NULL, hCalibrationComplete);
    //        
    //         m_user.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    //     }
    // }

    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_GESTURE, m_gesture);
    if(nRetVal != XN_STATUS_OK){ 
        qDebug() << "Could not find an Gesture Node";
    } else {
        startGestureTracking(); 
    }


    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_HANDS, m_hands);
    if(nRetVal != XN_STATUS_OK){ 
        qDebug() << "Could not find an Hands Node";
    } else {
        XnCallbackHandle handHandle;
        m_hands.SetSmoothing(0.1);
       
        m_hands.RegisterHandCallbacks(Hands_Create2, Hands_Update2, Hands_Destroy2, this, handHandle);
    }



    nRetVal = m_context.StartGeneratingAll();

    return nRetVal;

}

QImage* Kinect::getCameraQImage() {
    return m_qImageMap;
    /*QImage *img = new QImage( m_texSize.width(), m_texSize.height(), QImage::Format_ARGB32 );
    for( int y = 0; y < img->height(); ++ y )
    {
        for( int x = 0; x < img->width(); ++ x )
        {
          const XnRGB24Pixel& pVal = m_textureMap[ y * img->width() + x ];
          img->setPixel( x, y, 
                    QColor::fromRgb(pVal.nRed,pVal.nGreen,pVal.nBlue,255).rgba() );
        }
    }*/
}

int Kinect::acquireFrame()
{
	XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = m_context.WaitAnyUpdateAll();
    if(nRetVal != XN_STATUS_OK){ qDebug() << "Could not acquire a Frame"; return nRetVal; }

	const XnDepthPixel* pDepthData;
    if(hasDepthData) {
        pDepthData = m_depthMD.Data();
        m_depth.GetMetaData(m_depthMD);
	    xnOSMemSet(m_depthHisto, 0, MAX_DEPTH*sizeof(float));
        
        //the following Code is copied from the SimpleViewer sample of OpenNI
        //it uses pointer arithmetic (post-incremental) to iterate through the dynamic array
	    unsigned int nNumberOfPoints = 0;
	    for (XnUInt y = 0; y < m_depthMD.YRes(); ++y) {
		    for (XnUInt x = 0; x < m_depthMD.XRes(); ++x, ++pDepthData) {
			    if (*pDepthData != 0) {
			    	m_depthHisto[*pDepthData]++;
				    nNumberOfPoints++;
			    }
		    }
	    }
        for (int i=1; i<MAX_DEPTH; i++) {
            m_depthHisto[i] += m_depthHisto[i-1];
        }
        if (nNumberOfPoints) {
            for (int i=1; i<MAX_DEPTH; i++) {
                m_depthHisto[i] = (unsigned int)(256 * (1.0f - (m_depthHisto[i] / nNumberOfPoints)));
            }
        }
    }

	const XnUInt8* pImageData;
    if(hasImageData) {
        pImageData = m_imageMD.Data();
        m_image.GetMetaData(m_imageMD);
	    xnOSMemSet(m_textureMap, 0, m_texSize.width() * m_texSize.height() * sizeof(XnRGB24Pixel));
        delete m_qImageMap;
        m_qImageMap = new QImage(m_imageMD.XRes(), m_imageMD.YRes(),  QImage::Format_ARGB32); 
        //the following Code is copied from the SimpleViewer sample of OpenNI
        //it uses pointer arithmetic (post-incremental) to iterate through the dynamic array
		const XnRGB24Pixel* pImageRow = m_imageMD.RGB24Data();
		XnRGB24Pixel* pTexRow = m_textureMap + m_imageMD.YOffset() * m_texSize.width();

		for (XnUInt y = 0; y < m_imageMD.YRes(); ++y)
		{
			const XnRGB24Pixel* pImage = pImageRow;
			XnRGB24Pixel* pTex = pTexRow + m_imageMD.XOffset();

			for (XnUInt x = 0; x < m_imageMD.XRes(); ++x, ++pImage, ++pTex)
			{
				*pTex = *pImage;
                XnDepthPixel depth = m_depthMD(x,y);
                if(!m_applyDepthFilter || !isHandTracking || qAbs(m_rightHandVec->z() - depth) < 50) {
                    m_qImageMap->setPixel( x, y, 
                        QColor::fromRgb(pImage->nRed,pImage->nGreen,pImage->nBlue,255).rgba() );
                } else {
                    m_qImageMap->setPixel( x, y, (new QColor("black"))->rgba());


                }

			}

			pImageRow += m_imageMD.XRes();
			pTexRow += m_texSize.width();
		}
    }

   // detectUsers();
    return nRetVal;
}

void Kinect::updateHandPosition(const XnPoint3D* _pos)
{
    XnPoint3D handPosProj = {};
    m_depth.ConvertRealWorldToProjective(1, _pos, &handPosProj);
    m_rightHandP.setX(handPosProj.X);
    m_rightHandP.setY(handPosProj.Y);
    m_rightHandVec->setX(handPosProj.X);
    m_rightHandVec->setY(handPosProj.Y);
    m_rightHandVec->setZ(handPosProj.Z);

}


void Kinect::detectUsers() {
    XnUserID aUsers[MAX_NUM_USERS];
    XnUInt16 nUsers = m_user.GetNumberOfUsers();
    XnSkeletonJointTransformation handJoint;
    m_user.GetUsers(aUsers, nUsers);
    for(XnUInt16 i=0; i<nUsers; i++)
    {
        if(m_user.GetSkeletonCap().IsTracking(aUsers[i])==FALSE)
            continue;

        m_user.GetSkeletonCap().SetSmoothing( 0.1);
        m_user.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_RIGHT_HAND,handJoint);

        XnPoint3D handPosRW, handPosPj;
        handPosRW.X = handJoint.position.position.X; 
        handPosRW.Y = handJoint.position.position.Y; 
        handPosRW.Z = handJoint.position.position.Z;
        m_depth.ConvertRealWorldToProjective(1, &handPosRW, &handPosPj);
        m_rightHandP.setX(handPosPj.X);
        m_rightHandP.setY(handPosPj.Y);
        //    printf("user %d: hand at (x %6.2f  y %6.2f z %6.2f)\n",aUsers[i],
                                                            // handJoint.position.position.X,
                                                            // handJoint.position.position.Y,
                                                            // handJoint.position.position.Z);
    }
}


void Kinect::enumerateErrors() {
    EnumerationErrors::Iterator eIt = m_errors.Begin();
    while(eIt != m_errors.End()) {
        qDebug() << "Kinect Errror: " << eIt.Description().strName << " Vendor: " << eIt.Description().strVendor;
        eIt++;
    }
}

bool Kinect::findNode(XnProductionNodeType type, ProductionNode &node) {



}


