#include "QEarthManager.h"

#include <osgEarth/ElevationQuery>
#include <osgEarthUtil/Controls>
#include <osg/Multisample>

#include "QEarthMath.h"
#include "QueryElevationHandler.h"

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

QEarthManager::QEarthManager():
m_mapNode(NULL),
m_pRoot(NULL)
{
}


QEarthManager::~QEarthManager()
{
	m_manipulator.release();
	m_skyNode.release();
	m_mapNode.release();
	m_pRoot.release();
}

QEarthManager* QEarthManager::instance()
{
	static QEarthManager e;
	return &e;
}

void QEarthManager::convertObjMatrix(const double dLon, const double dLat,
	const double dAlt, const double dAzimuth,
	const double dPitch, const double dRoll, osg::MatrixTransform* matrixNode)
{
	if (!m_mapNode)
	{
		OE_FATAL << "earth map node is invalid." << std::endl;
		return ;
	}
	//高度赋值后台站倾斜
	osg::Matrixd geoToWorldMat;
	/**1.地理坐标系变换到世界坐标系*/
	QEarthMath::GEOCoordFrameToWorldCoordFrame(dLon, dLat,
		dAlt, 0.0, 0.0, 0.0, dAzimuth, dPitch, dRoll,
		geoToWorldMat, *m_mapNode->getMapSRS());
	/**2.物体坐标系下的偏移*/
	geoToWorldMat.preMultTranslate(osg::Vec3d(0, 0, 0));

	matrixNode->setMatrix(geoToWorldMat);
}


void QEarthManager::worldPointToMapPoint(const double x, const double y, const double z, double& lon, double& lat, double& alt)
{
	osg::Vec3d input(x, y, z);
	osg::Vec3d output(0.0f, 0.0f, 0.0f);
	m_mapNode->getMapSRS()->transformFromWorld(input, output);
	lon = output.x();
	lat = output.y();
	alt = output.z();
}

void QEarthManager::mapPointToWorldPoint(const double lon, const double lat, const double alt, double& x, double& y, double& z)
{
	osg::Vec3d input(lon, lat, alt);
	osg::Vec3d output(0.0f, 0.0f, 0.0f);
	m_mapNode->getMapSRS()->transformToWorld(input, output);
	x = output.x();
	y = output.y();
	z = output.z();
}

double QEarthManager::getElevation(const double dLon, const double dLat)
{
	//  期望高程精度。单位：米
	double query_resolution = 0.00001f;
	// 得到的高程值
	double out_elevation = 0.0f;
	// 实际的高程精度。单位：米
	double out_resolution = 0.0f;
	osgEarth::ElevationQuery query(m_mapNode->getMap());
	// convert to map coords:
	GeoPoint mapPoint(m_mapNode->getMapSRS(), dLon, dLat);
	bool bOk = query.getElevation(mapPoint,
		out_elevation,
		query_resolution,
		&out_resolution);

	return bOk ? out_elevation : -10000.0f;
}

bool QEarthManager::initEarthView(const QString& earthFile, osgViewer::Viewer* pViewer)
{
	if (!pViewer) return false;
	// 地球仪节点
	osg::Node* mapNode = osgDB::readNodeFile(earthFile.toStdString());
	//创建MapNode
	m_mapNode = osgEarth::MapNode::findMapNode(mapNode);
	if (!m_mapNode)
	{
		OE_WARN << "Unable to load earth file." << std::endl;
		return false;
	}
	//建立一个根节点
	m_pRoot = new osg::Group();
	//将MapNode添加到组节点中去
	m_pRoot->addChild(m_mapNode);

	//实现抗锯齿
	osg::Multisample *pms = new osg::Multisample;
	pms->setSampleCoverage(GL_MULTISAMPLE_ARB, true);
	m_pRoot->getOrCreateStateSet()->setAttributeAndModes(pms, osg::StateAttribute::ON);

	//将组节点设置为场景节点
	pViewer->setSceneData(m_pRoot);

	addEarthManipulator(pViewer);

	addSkyNode(pViewer);

	//添加自定义高程提示
	pViewer->addEventHandler(new QueryElevationHandler(m_mapNode, m_pRoot));
	return true;
}


void QEarthManager::addEarthManipulator(osgViewer::Viewer* pViewer)
{
	//设置earth操作器
	m_manipulator = new osgEarth::Util::EarthManipulator();
	m_manipulator->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_UP);
	m_manipulator->getSettings()->bindScroll(osgEarth::Util::EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_DOWN);
	m_manipulator->getSettings()->setArcViewpointTransitions(true);
	m_manipulator->getSettings()->setTetherMode(osgEarth::Util::EarthManipulator::TETHER_CENTER);
	pViewer->setCameraManipulator(m_manipulator.get());
	m_manipulator->setViewpoint(osgEarth::Viewpoint(QString("默认位置").toLocal8Bit().data(),
		115.971, 30.85, 444.02, -1.84, -70.01, 8028182.75), 0.0);
}

void QEarthManager::addSkyNode(osgViewer::Viewer* pViewer)
{
	/**初始化天空*/
	m_skyNode = SkyNode::create(m_mapNode);
	if (m_skyNode)
	{
		DateTime dTime;//自动时间
		m_skyNode->setDateTime(dTime);
		m_skyNode->attach(pViewer);
		m_skyNode->setSunVisible(true);
		m_skyNode->setMoonVisible(true);
		m_skyNode->setStarsVisible(true);
		m_skyNode->setLighting(osg::StateAttribute::OFF);

		osgEarth::insertGroup(m_skyNode, m_mapNode->getParent(0));
	}
}


osg::ref_ptr<osg::Group> QEarthManager::createLight(osg::ref_ptr<osg::Node> node)
{
	// 向场景中添加光源
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

	//计算包围盒
	osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();

	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//设置位置	
	light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线形衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);

	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());

	lightRoot->addChild(lightSource.get());

	return lightRoot.get();
}

