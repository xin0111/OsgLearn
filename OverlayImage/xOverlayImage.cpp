#include "xOverlayImage.h"

#include <osgEarthAnnotation/ImageOverlay>
#include <QOpenGLWidget>
#include <QtOpenGL/QGLWidget>
#include <osgEarth/Bounds>
#include "QEarthManager.h"

xOverlayImage::xOverlayImage(osgEarth::MapNode* mapNode)
	:m_mapNode(mapNode),
	m_refOverLay(new osg::Group)
{
	m_refOverLay->setName("xOverlayImage");
	QEarthManager::instance()->getScreenRoot()->addChild(m_refOverLay);
}

xOverlayImage::~xOverlayImage()
{
	QEarthManager::instance()->getScreenRoot()->removeChild(m_refOverLay);
}

using namespace osgEarth::Annotation;
void xOverlayImage::DrawCoverImage(QImage image, double dtopLeftLon,
	double dtopLeftLat, double dbtRightLon, double dbtRightLat, std::string imgID)
{
	osgEarth::MapNode* mapNode = m_mapNode;
	//得到四角坐标 贴到地球上
	osg::Image* osgImage = CovertImage(image);
	if (!osgImage) return;
	ImageOverlay* overlay = new ImageOverlay(mapNode);
	overlay->setImage(osgImage);
	overlay->setAlpha(0.6);
	/*
	...tl
	......center
	.............br
	*/
	overlay->setWest(dtopLeftLon);
	overlay->setEast(dbtRightLon);
	overlay->setNorth(dbtRightLat);
	overlay->setSouth(dtopLeftLat);

	overlay->setBounds(Bounds(dtopLeftLon, dbtRightLat, dbtRightLon, dtopLeftLat));
	overlay->setMinFilter(osg::Texture::LINEAR);
	overlay->setMagFilter(osg::Texture::LINEAR);

	RemoveCoverImg(imgID);
	overlay->setName(imgID);
	m_refOverLay->addChild(overlay);
}

osg::Image *xOverlayImage::CovertImage(const  QImage& image)
{
	QImage gImage = QGLWidget::convertToGLFormat(image);
	gImage = gImage.mirrored(false, true);
	unsigned char* data = new unsigned char[gImage.byteCount()];
	for (int i = 0; i < gImage.byteCount(); i++)
	{
		data[i] = gImage.bits()[i];
	}

	osg::Image *markerImage = new osg::Image();
	markerImage->setImage(gImage.width(), gImage.height(),
		1, 4, GL_RGBA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE, 1);
	return markerImage;
}

void xOverlayImage::RemoveCoverImg(std::string imgID)
{
	osg::ref_ptr<osg::Group> m_group = NULL;
	int nCount = m_refOverLay->getNumChildren();
	for (unsigned int i = 0; i < nCount; i++)
	{
		if (m_refOverLay->getChild(i)->getName() == imgID)
		{
			m_refOverLay->removeChild(i);
			break;
		}
	}
}

void xOverlayImage::SetCoverImagVisiable(std::string imgID, bool bShow)
{
	int nCount = m_refOverLay->getNumChildren();
	for (unsigned int i = 0; i < nCount; i++)
	{
		auto overlay = m_refOverLay->getChild(i);
		if (overlay->getName() == imgID)
		{
			overlay->setNodeMask(bShow);
			break;
		}
	}
}
