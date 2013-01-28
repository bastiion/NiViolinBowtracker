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


XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

Kinect::Kinect()
{
    hasDepthData = true;
    hasImageData = true;
}

Kinect::~Kinect()
{
   delete m_imageMap;
   delete[] m_depthBuffer;
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
    if(nRetVal != XN_STATUS_OK){ qDebug() << "Could not init Context from Sample Configuration file "; return nRetVal; }

    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_DEPTH, m_depth);
    if(nRetVal != XN_STATUS_OK){ qDebug() << "Could not find a Depth Node "; return nRetVal; }
    m_depth.GetMetaData(m_depthMD);

    nRetVal = m_context.FindExistingNode(XN_NODE_TYPE_IMAGE, m_image);
    if(nRetVal != XN_STATUS_OK){ qDebug() << "Could not find an Image Node"; return nRetVal; }
    m_image.GetMetaData(m_imageMD);


	if (m_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24) {
        qDebug() << "Pixel Format not in RGB24. Unsupported PixelFormat! " << m_imageMD.PixelFormat();
        return nRetVal; }

    m_imgSize[0] = (((unsigned short)(m_imageMD.FullXRes()-1) / 512) + 1) * 512;
	m_imgSize[1] = (((unsigned short)(m_imageMD.FullYRes()-1) / 512) + 1) * 512;
	m_imageMap = (XnRGB24Pixel*)malloc(m_imgSize[0] * m_imgSize[1] * sizeof(XnRGB24Pixel));



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
	    xnOSMemSet(m_depthBuffer, 0, MAX_DEPTH*sizeof(float));
        
        //the following Code is copied from the SimpleViewer sample of OpenNI
        //it uses pointer arithmetic (post-incremental) to iterate through the dynamic array
	    unsigned int nNumberOfPoints = 0;
	    for (XnUInt y = 0; y < m_depthMD.YRes(); ++y) {
		    for (XnUInt x = 0; x < m_depthMD.XRes(); ++x, ++pDepthData) {
			    if (*pDepthData != 0) {
			    	m_depthBuffer[*pDepthData]++;
				    nNumberOfPoints++;
			    }
		    }
	    }
        for (int i=1; i<MAX_DEPTH; i++) {
            m_depthBuffer[i] += m_depthBuffer[i-1];
        }
        if (nNumberOfPoints) {
            for (int i=1; i<MAX_DEPTH; i++) {
                m_depthBuffer[i] = (unsigned int)(256 * (1.0f - (m_depthBuffer[i] / nNumberOfPoints)));
            }
        }
    }

	const XnUInt8* pImageData;
    if(hasImageData) {
        pImageData = m_imageMD.Data();
        m_image.GetMetaData(m_imageMD);
	    xnOSMemSet(m_imageMap, 0, m_imgSize[0] * m_imgSize[1] * sizeof(XnRGB24Pixel));
    
        //the following Code is copied from the SimpleViewer sample of OpenNI
        //it uses pointer arithmetic (post-incremental) to iterate through the dynamic array
		const XnRGB24Pixel* pImageRow = m_imageMD.RGB24Data();
		XnRGB24Pixel* pTexRow = m_imageMap + m_imageMD.YOffset() * m_imgSize[0];

		for (XnUInt y = 0; y < m_imageMD.YRes(); ++y)
		{
			const XnRGB24Pixel* pImage = pImageRow;
			XnRGB24Pixel* pTex = pTexRow + m_imageMD.XOffset();

			for (XnUInt x = 0; x < m_imageMD.XRes(); ++x, ++pImage, ++pTex)
			{
				*pTex = *pImage;
			}

			pImageRow += m_imageMD.XRes();
			pTexRow += m_imgSize[0];
		}
    }
    return nRetVal;
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


