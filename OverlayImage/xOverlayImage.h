#pragma once

#include <QImage>
#include <osg\Group>
#include <osgEarth\MapNode>

class xOverlayImage
{
public:
	xOverlayImage(osgEarth::MapNode* mapNode);
	~xOverlayImage();

	void DrawCoverImage(QImage image, double dtopLeftLon,
		double dtopLeftLat, double dbtRightLon, double dbtRightLat, std::string imgID);
	
	void RemoveCoverImg(std::string imgID);

	void SetCoverImagVisiable(std::string imgID, bool bShow);
protected:
	osg::Image *CovertImage(const QImage& image);
private:
	osgEarth::MapNode* m_mapNode;
	osg::ref_ptr<osg::Group> m_refOverLay;//Í¼²ã
};

