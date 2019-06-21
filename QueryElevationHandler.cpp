#include "QueryElevationHandler.h"

#include <osg/group>   
#include <osgQt/GraphicsWindowQt>
#include <osgDB/ReadFile>
#include <osg/Multisample>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Formatter>
#include <osgEarthUtil/Controls>
#include <osgEarth/SpatialReference>
#include <osgEarth/Terrain>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarth/ElevationQuery>
#include <osg/Node>
#include <osgEarth/TerrainEngineNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/GeoData>

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

QueryElevationHandler::QueryElevationHandler(osgEarth::MapNode* mapNode,
	osg::ref_ptr<osg::Group> pRoot) :
_mouseDown(false),
_mapNode(mapNode),
_terrain(mapNode->getTerrain()),
_query(mapNode->getMap())
{
	displayPosLabel(pRoot);
	//��ʼ������ѯLOD����
	_query.setMaxTilesToCache(10);
	_path.push_back(mapNode->getTerrainEngine());
}

//����һ�����¼��Ķ���������һ���Ƕ�Ӧ�Ĳ���
bool QueryElevationHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	//�ж�������ƶ�����¼��Ž��и��µ�ǰ��������ʾ
	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		osgUtil::LineSegmentIntersector::Intersections intersections;
		osg::Vec3d mapIntersectionPoint;
		osg::Vec3d worldIntersectionPoint;
		osgEarth::Util::EarthManipulator * em = dynamic_cast<osgEarth::Util::EarthManipulator*>(view->getCameraManipulator());
		if (view->computeIntersections(ea.getX(), ea.getY(), _path, intersections))
		{
			osgUtil::LineSegmentIntersector::Intersection fistIntersection = (*intersections.begin());
			// ������������
			worldIntersectionPoint = fistIntersection.getWorldIntersectPoint();
			// ���㾭γ������
			_mapNode->getMapSRS()->transformFromWorld(worldIntersectionPoint, mapIntersectionPoint);
			double lon = mapIntersectionPoint.x();
			double lat = mapIntersectionPoint.y();
			double alt = mapIntersectionPoint.z();

			QString strText = QString::fromLocal8Bit("Lon:%1 Lat:%2 Height(MSL):%3").arg(lon).arg(lat).arg(alt);
			m_mouseCoords->setText(strText.toStdString());
		}
	}

	return false;
}

void QueryElevationHandler::displayPosLabel(osg::ref_ptr<osg::Group> pRoot)
{
	m_mouseCoords = new osgEarth::Util::Controls::LabelControl("Label", 18.0, osg::Vec4(1.0, 1.0, 1.0, 1.0));
	m_mouseCoords->setEncoding(osgText::String::ENCODING_UTF8);
	m_mouseCoords->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_RIGHT);
	m_mouseCoords->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
	m_mouseCoords->setBackColor(0, 0, 0, 0.5);
	m_mouseCoords->setSize(320, 50);
	m_mouseCoords->setMargin(10);
	//�ؼ���������
	ControlCanvas* canvas = new ControlCanvas();
	//��Ҫ��ʾ�Ŀؼ����뵽root��ڵ���ȥ
	pRoot->addChild(canvas);
	canvas->addControl(m_mouseCoords);
}